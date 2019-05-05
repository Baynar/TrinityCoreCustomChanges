/*
 * Copyright (C) 2008-2019 TrinityCore <https://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "AnticheatMgr.h"
#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "Corpse.h"
#include "Player.h"
#include "MapManager.h"
#include "MotionMaster.h"
#include "MovementGenerator.h"
#include "Transport.h"
#include "Battleground.h"
#include "InstanceSaveMgr.h"
#include "ObjectMgr.h"
#include "Vehicle.h"
#include "GameTime.h"
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

#include "World.h"
#include "WardenWin.h"

// Movement anticheat defines
//#define ANTICHEAT_DEBUG
#define ANTICHEAT_EXCEPTION_INFO
// End Movement anticheat defines


void WorldSession::HandleMoveWorldportAckOpcode(WorldPacket & /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: got MSG_MOVE_WORLDPORT_ACK.");
    HandleMoveWorldportAck();
}

void WorldSession::HandleMoveWorldportAck()
{
    Player* player = GetPlayer();
    // ignore unexpected far teleports
    if (!player->IsBeingTeleportedFar())
        return;

    player->SetSemaphoreTeleportFar(false);

    // get the teleport destination
    WorldLocation const& loc = player->GetTeleportDest();

    // possible errors in the coordinate validity check
    if (!MapManager::IsValidMapCoord(loc))
    {
        LogoutPlayer(false);
        return;
    }

    // get the destination map entry, not the current one, this will fix homebind and reset greeting
    MapEntry const* mEntry = sMapStore.LookupEntry(loc.GetMapId());
    InstanceTemplate const* mInstance = sObjectMgr->GetInstanceTemplate(loc.GetMapId());

    // reset instance validity, except if going to an instance inside an instance
    if (player->m_InstanceValid == false && !mInstance)
        player->m_InstanceValid = true;

    Map* oldMap = player->GetMap();
    Map* newMap = sMapMgr->CreateMap(loc.GetMapId(), player);

    if (player->IsInWorld())
    {
        TC_LOG_ERROR("network", "%s %s is still in world when teleported from map %s (%u) to new map %s (%u)", player->GetGUID().ToString().c_str(), player->GetName().c_str(), oldMap->GetMapName(), oldMap->GetId(), newMap ? newMap->GetMapName() : "Unknown", loc.GetMapId());
        oldMap->RemovePlayerFromMap(player, false);
    }

    // relocate the player to the teleport destination
    // the CannotEnter checks are done in TeleporTo but conditions may change
    // while the player is in transit, for example the map may get full
    if (!newMap || newMap->CannotEnter(player))
    {
        TC_LOG_ERROR("network", "Map %d (%s) could not be created for player %d (%s), porting player to homebind", loc.GetMapId(), newMap ? newMap->GetMapName() : "Unknown", player->GetGUID().GetCounter(), player->GetName().c_str());
        player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, player->GetOrientation());
        return;
    }

    float z = loc.GetPositionZ() + player->GetHoverOffset();
    player->Relocate(loc.GetPositionX(), loc.GetPositionY(), z, loc.GetOrientation());
    player->SetFallInformation(0, player->GetPositionZ());

    player->ResetMap();
    player->SetMap(newMap);

    player->SendInitialPacketsBeforeAddToMap();
    if (!player->GetMap()->AddPlayerToMap(player))
    {
        TC_LOG_ERROR("network", "WORLD: failed to teleport player %s (%d) to map %d (%s) because of unknown reason!",
            player->GetName().c_str(), player->GetGUID().GetCounter(), loc.GetMapId(), newMap ? newMap->GetMapName() : "Unknown");
        player->ResetMap();
        player->SetMap(oldMap);
        player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, player->GetOrientation());
        return;
    }

    // battleground state prepare (in case join to BG), at relogin/tele player not invited
    // only add to bg group and object, if the player was invited (else he entered through command)
    if (player->InBattleground())
    {
        // cleanup setting if outdated
        if (!mEntry->IsBattlegroundOrArena())
        {
            // We're not in BG
            player->SetBattlegroundId(0, BATTLEGROUND_TYPE_NONE);
            // reset destination bg team
            player->SetBGTeam(0);
        }
        // join to bg case
        else if (Battleground* bg = player->GetBattleground())
        {
            if (player->IsInvitedForBattlegroundInstance(player->GetBattlegroundId()))
                bg->AddPlayer(player);
        }
    }

    player->SendInitialPacketsAfterAddToMap();

    // flight fast teleport case
    if (player->IsInFlight())
    {
        if (!player->InBattleground())
        {
            // short preparations to continue flight
            MovementGenerator* movementGenerator = player->GetMotionMaster()->GetCurrentMovementGenerator();
            movementGenerator->Initialize(player);
            return;
        }

        // battleground state prepare, stop flight
        player->FinishTaxiFlight();
    }

    if (!player->IsAlive() && player->GetTeleportOptions() & TELE_REVIVE_AT_TELEPORT)
        player->ResurrectPlayer(0.5f);

    // resurrect character at enter into instance where his corpse exist after add to map
    if (mEntry->IsDungeon() && !player->IsAlive())
    {
        if (player->GetCorpseLocation().GetMapId() == mEntry->MapID)
        {
            player->ResurrectPlayer(0.5f);
            player->SpawnCorpseBones();
        }
    }

    bool allowMount = !mEntry->IsDungeon() || mEntry->IsBattlegroundOrArena();
    if (mInstance)
    {
        // check if this instance has a reset time and send it to player if so
        Difficulty diff = player->GetDifficulty(mEntry->IsRaid());
        if (MapDifficulty const* mapDiff = GetMapDifficultyData(mEntry->MapID, diff))
        {
            if (mapDiff->resetTime)
            {
                if (time_t timeReset = sInstanceSaveMgr->GetResetTimeFor(mEntry->MapID, diff))
                {
                    uint32 timeleft = uint32(timeReset - GameTime::GetGameTime());
                    player->SendInstanceResetWarning(mEntry->MapID, diff, timeleft, true);
                }
            }
        }

        // check if instance is valid
        if (!player->CheckInstanceValidity(false))
            player->m_InstanceValid = false;

        // instance mounting is handled in InstanceTemplate
        allowMount = mInstance->AllowMount;
    }

    // mount allow check
    if (!allowMount)
        player->RemoveAurasByType(SPELL_AURA_MOUNTED);

    // update zone immediately, otherwise leave channel will cause crash in mtmap
    uint32 newzone, newarea;
    player->GetZoneAndAreaId(newzone, newarea);
    player->UpdateZone(newzone, newarea);

    // honorless target
    if (player->pvpInfo.IsHostile)
        player->CastSpell(player, 2479, true);

    // in friendly area
    else if (player->IsPvP() && !player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP))
        player->UpdatePvP(false, false);

    // resummon pet
    player->ResummonPetTemporaryUnSummonedIfAny();

    //lets process all delayed operations on successful teleport
    player->ProcessDelayedOperations();
}

void WorldSession::HandleMoveTeleportAck(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "MSG_MOVE_TELEPORT_ACK");
    ObjectGuid guid;

    recvData >> guid.ReadAsPacked();

    uint32 sequenceIndex, time;
    recvData >> sequenceIndex >> time;

    Player* plMover = _player->m_unitMovedByMe->ToPlayer();

    if (!plMover || !plMover->IsBeingTeleportedNear())
        return;

    if (guid != plMover->GetGUID())
        return;

    plMover->SetSemaphoreTeleportNear(false);

    uint32 old_zone = plMover->GetZoneId();

    WorldLocation const& dest = plMover->GetTeleportDest();

    plMover->UpdatePosition(dest, true);
    plMover->SetFallInformation(0, GetPlayer()->GetPositionZ());

    uint32 newzone, newarea;
    plMover->GetZoneAndAreaId(newzone, newarea);
    plMover->UpdateZone(newzone, newarea);

    // new zone
    if (old_zone != newzone)
    {
        // honorless target
        if (plMover->pvpInfo.IsHostile)
            plMover->CastSpell(plMover, 2479, true);

        // in friendly area
        else if (plMover->IsPvP() && !plMover->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP))
            plMover->UpdatePvP(false, false);
    }

    // resummon pet
    GetPlayer()->ResummonPetTemporaryUnSummonedIfAny();

    //lets process all delayed operations on successful teleport
    GetPlayer()->ProcessDelayedOperations();
}

void WorldSession::HandleMovementOpcodes(WorldPacket& recvData)
{
    uint16 opcode = recvData.GetOpcode();

    Unit* mover = _player->m_unitMovedByMe;

    ASSERT(mover != nullptr);                      // there must always be a mover

    //Player* plrMover = mover->ToPlayer();
    Player* plrMover = mover->GetTypeId() == TYPEID_PLAYER ? (Player*)mover : NULL;
    Vehicle* vehMover = mover->GetVehicleKit();
    if (vehMover)
        if (mover->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
            if (Unit* charmer = mover->GetCharmer())
                if (charmer->GetTypeId() == TYPEID_PLAYER)
                    plrMover = (Player*)charmer;

    // ignore, waiting processing in WorldSession::HandleMoveWorldportAckOpcode and WorldSession::HandleMoveTeleportAck
    if (plrMover && plrMover->IsBeingTeleported())
    {
        recvData.rfinish();                     // prevent warnings spam
        return;
    }

    /* extract packet */
    ObjectGuid guid;

    recvData >> guid.ReadAsPacked();

    MovementInfo movementInfo;
    movementInfo.guid = guid;
    ReadMovementInfo(recvData, &movementInfo);

    recvData.rfinish();                         // prevent warnings spam

    // prevent tampered movement data
    if (guid != mover->GetGUID())
        return;

    if (!movementInfo.pos.IsPositionValid())
    {
        recvData.rfinish();                     // prevent warnings spam
        return;
    }

    /* handle special cases */
    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_ONTRANSPORT))
    {
        // We were teleported, skip packets that were broadcast before teleport
        if (movementInfo.pos.GetExactDist2d(mover) > SIZE_OF_GRIDS)
        {
            recvData.rfinish();                 // prevent warnings spam
            return;
        }

        // transports size limited
        // (also received at zeppelin leave by some reason with t_* as absolute in continent coordinates, can be safely skipped)
        if (fabs(movementInfo.transport.pos.GetPositionX()) > 75.0f || fabs(movementInfo.transport.pos.GetPositionY()) > 75.0f || fabs(movementInfo.transport.pos.GetPositionZ()) > 75.0f)
        {
            recvData.rfinish();                 // prevent warnings spam
            return;
        }

        if (!Trinity::IsValidMapCoord(movementInfo.pos.GetPositionX() + movementInfo.transport.pos.GetPositionX(), movementInfo.pos.GetPositionY() + movementInfo.transport.pos.GetPositionY(),
            movementInfo.pos.GetPositionZ() + movementInfo.transport.pos.GetPositionZ(), movementInfo.pos.GetOrientation() + movementInfo.transport.pos.GetOrientation()))
        {
            recvData.rfinish();                 // prevent warnings spam
            return;
        }

        // if we boarded a transport, add us to it
        //if (plrMover)
        if (plrMover && !plrMover->m_transport && !plrMover->m_temp_transport)
        {
            if (!plrMover->GetTransport())
            {
                if (Transport* transport = plrMover->GetMap()->GetTransport(movementInfo.transport.guid))
                    transport->AddPassenger(plrMover);
            }
            else if (plrMover->GetTransport()->GetGUID() != movementInfo.transport.guid)
            {
                plrMover->GetTransport()->RemovePassenger(plrMover);
                if (Transport* transport = plrMover->GetMap()->GetTransport(movementInfo.transport.guid))
                    transport->AddPassenger(plrMover);
                else
                    movementInfo.transport.Reset();
            }
        }

        //if (!mover->GetTransport() && !mover->GetVehicle())
        if (!plrMover->m_transport)
            if (Map *tempMap = mover->GetMap())
                if (GameObject *tempTransport = tempMap->GetGameObject(movementInfo.transport.guid))
                    if (tempTransport->IsTransport())
                        plrMover->m_temp_transport = tempTransport;

        if ((!plrMover && !mover->GetTransport() && !mover->GetVehicle()) || (plrMover && !plrMover->m_vehicle && !plrMover->m_transport && !plrMover->m_temp_transport)) // Not sure if the first part is needed. Just added it for verbosity.
                movementInfo.RemoveMovementFlag(MOVEMENTFLAG_ONTRANSPORT);
        //}
    }
    //else if (plrMover && plrMover->GetTransport())                // if we were on a transport, leave
    else if (plrMover && (plrMover->m_transport || plrMover->m_temp_transport)) // if we were on a transport, leave
    {
        //plrMover->GetTransport()->RemovePassenger(plrMover);
        if (plrMover->m_transport)
        {
            plrMover->m_transport->RemovePassenger(plrMover);
            plrMover->m_transport = NULL;
        }
        plrMover->m_temp_transport = NULL;
        movementInfo.transport.Reset();
    }

    // fall damage generation (ignore in flight case that can be triggered also at lags in moment teleportation to another map).
    if (opcode == MSG_MOVE_FALL_LAND && plrMover && !plrMover->IsInFlight())
        plrMover->HandleFall(movementInfo);

    // interrupt parachutes upon falling or landing in water
    if (opcode == MSG_MOVE_FALL_LAND || opcode == MSG_MOVE_START_SWIM)
        mover->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_LANDING); // Parachutes

    if (plrMover && ((movementInfo.flags & MOVEMENTFLAG_SWIMMING) != 0) != plrMover->IsInWater())
    {
        // now client not include swimming flag in case jumping under water
        plrMover->SetInWater(!plrMover->IsInWater() || plrMover->GetBaseMap()->IsUnderWater(movementInfo.pos.GetPositionX(), movementInfo.pos.GetPositionY(), movementInfo.pos.GetPositionZ()));
    }

    if (plrMover)
        sAnticheatMgr->StartHackDetection(plrMover, movementInfo, opcode);

    /* process position-change */
    /*WorldPacket data(opcode, recvData.size());
    int64 movementTime = (int64) movementInfo.time + _timeSyncClockDelta;
    if (_timeSyncClockDelta == 0 || movementTime < 0 || movementTime > 0xFFFFFFFF)
    {
        TC_LOG_WARN("misc", "The computed movement time using clockDelta is erronous. Using fallback instead");
        movementInfo.time = GameTime::GetGameTimeMS();
    }
    else
    {
        movementInfo.time = (uint32)movementTime;
    }*/

    // begin anti cheat
    bool check_passed = true;
#ifdef ANTICHEAT_DEBUG
    TC_LOG_WARN("cheat", "AC2-%s > time: %d fall-time: %d | xyzo: %f, %f, %fo(%f) flags[%X] opcode[%s] | transport (xyzo): %f, %f, %fo(%f)",
        plrMover->GetName(), movementInfo.time, movementInfo.fallTime, movementInfo.pos.m_positionX, movementInfo.pos.m_positionY, movementInfo.pos.m_positionZ, movementInfo.pos.m_orientation,
        movementInfo.flags, LookupOpcodeName(opcode), movementInfo.transport.pos.m_positionX, movementInfo.transport.pos.m_positionY, movementInfo.transport.pos.m_positionZ, movementInfo.transport.pos.m_orientation);
    TC_LOG_WARN("cheat", "AC2-%s Transport > GUID: (low)%d - (high)%d",
        plrMover->GetName(), GUID_LOPART(movementInfo.transport.guid), GUID_HIPART(movementInfo.transport.guid));
#endif

    if (plrMover)
    {
        if (World::GetEnableMvAnticheat() && !plrMover->IsGameMaster() && !plrMover->GetCharmerOrOwnerPlayerOrPlayerItself()->IsGameMaster() && !plrMover->GetCharmerOrOwnerPlayerOrPlayerItself()->GetVehicle())
        {
            // calc time deltas
            int32 cClientTimeDelta = 1500;
            if (plrMover->m_anti_LastClientTime != 0)
            {
                cClientTimeDelta = movementInfo.time - plrMover->m_anti_LastClientTime;
                plrMover->m_anti_DeltaClientTime += cClientTimeDelta;
                plrMover->m_anti_LastClientTime = movementInfo.time;
            }
            else
                plrMover->m_anti_LastClientTime = movementInfo.time;

            const uint64 cServerTime = getMSTime();
            uint32 cServerTimeDelta = 1500;
            if (plrMover->m_anti_LastServerTime != 0)
            {
                cServerTimeDelta = cServerTime - plrMover->m_anti_LastServerTime;
                plrMover->m_anti_DeltaServerTime += cServerTimeDelta;
                plrMover->m_anti_LastServerTime = cServerTime;
            }
            else
                plrMover->m_anti_LastServerTime = cServerTime;

    /*movementInfo.guid = mover->GetGUID();
    WriteMovementInfo(&data, &movementInfo);
    mover->SendMessageToSet(&data, _player);*/
            // resync times on client login (first 15 sec for heavy areas)
            if (plrMover->m_anti_DeltaServerTime < 15000 && plrMover->m_anti_DeltaClientTime < 15000)
                plrMover->m_anti_DeltaClientTime = plrMover->m_anti_DeltaServerTime;

    //mover->m_movementInfo = movementInfo;
            const int32 sync_time = plrMover->m_anti_DeltaClientTime - plrMover->m_anti_DeltaServerTime;

    // Some vehicles allow the passenger to turn by himself
    /*if (Vehicle* vehicle = mover->GetVehicle())
    {
        if (VehicleSeatEntry const* seat = vehicle->GetSeatForPassenger(mover))
        {
            if (seat->m_flags & VEHICLE_SEAT_FLAG_ALLOW_TURNING)
            {
                if (movementInfo.pos.GetOrientation() != mover->GetOrientation())
                {
                    mover->SetOrientation(movementInfo.pos.GetOrientation());
                    mover->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TURNING);
                }
            }
        }
        return;
    }*/

    #ifdef ANTICHEAT_DEBUG
        TC_LOG_WARN("cheat", "AC2-%s Time > cClientTimeDelta: %d, cServerTime: %d | deltaC: %d - deltaS: %d | SyncTime: %d", plrMover->GetName(), cClientTimeDelta, cServerTime, plrMover->m_anti_DeltaClientTime, plrMover->m_anti_DeltaServerTime, sync_time);
    #endif

    //mover->UpdatePosition(movementInfo.pos);
        // mistiming checks
        const int32 GetMistimingDelta = abs(int32(World::GetMistimingDelta()));
        if (sync_time > GetMistimingDelta)
        {
            cClientTimeDelta = cServerTimeDelta;
            ++(plrMover->m_anti_MistimingCount);

    const bool bMistimingModulo = plrMover->m_anti_MistimingCount % 50 == 0;
 
                if (bMistimingModulo)
                {
                    check_passed = false;
                }
                // Tell the player "Sure, you can fly!"
                {
                    WorldPacket data(SMSG_MOVE_SET_CAN_FLY, 12);
                    data << uint64(plrMover->GetGUID());
                    data << uint32(0);
                    SendPacket(&data);
                }
                // Then tell the player "Wait, no, you can't."
                {
                    WorldPacket data(SMSG_MOVE_UNSET_CAN_FLY, 12);
                    data << uint64(plrMover->GetGUID());
                    data << uint32(0);
                    SendPacket(&data);
                }
                //plrMover->FallGround(2);
            }
            // end mistiming checks

            const uint32 curDest = plrMover->m_taxi.GetTaxiDestination(); // check taxi flight
            if (!curDest)
            {
                UnitMoveType move_type;

                // calculating section
                // current speed
                if (movementInfo.flags & MOVEMENTFLAG_FLYING)
                    move_type = movementInfo.flags & MOVEMENTFLAG_BACKWARD ? MOVE_FLIGHT_BACK : MOVE_FLIGHT;
                else if (movementInfo.flags & MOVEMENTFLAG_SWIMMING)
                    move_type = movementInfo.flags & MOVEMENTFLAG_BACKWARD ? MOVE_SWIM_BACK : MOVE_SWIM;
                else if (movementInfo.flags & MOVEMENTFLAG_WALKING)
                    move_type = MOVE_WALK;
                // hmm... in first time after login player has MOVE_SWIMBACK instead MOVE_WALKBACK
                else
                    move_type = movementInfo.flags & MOVEMENTFLAG_BACKWARD ? MOVE_SWIM_BACK : MOVE_RUN;

                const float current_speed = mover->GetSpeed(move_type);
                // end current speed

                // movement distance
                const float delta_x = plrMover->m_transport || plrMover->m_temp_transport ? 0 : plrMover->GetPositionX() - movementInfo.pos.GetPositionX();
                const float delta_y = plrMover->m_transport || plrMover->m_temp_transport ? 0 : plrMover->GetPositionY() - movementInfo.pos.GetPositionY();
                const float delta_z = plrMover->m_transport || plrMover->m_temp_transport ? 0 : plrMover->GetPositionZ() - movementInfo.pos.GetPositionZ();
                const float real_delta = plrMover->m_transport || plrMover->m_temp_transport ? 0 : pow(delta_x, 2) + pow(delta_y, 2);
                // end movement distance

                const bool no_fly_auras = !(plrMover->HasAuraType(SPELL_AURA_FLY) || plrMover->HasAuraType(SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED)
                    || plrMover->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) || plrMover->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED)
                    || plrMover->HasAuraType(SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS) || plrMover->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK));
                const bool no_fly_flags = (movementInfo.flags & (MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING)) == 0;

                const bool no_swim_flags = (movementInfo.flags & MOVEMENTFLAG_SWIMMING) == 0;
                const bool no_swim_in_water = !mover->IsInWater();
                const bool no_swim_above_water = movementInfo.pos.GetPositionZ() - 7.0f >= mover->GetBaseMap()->GetWaterLevel(movementInfo.pos.GetPositionX(), movementInfo.pos.GetPositionY());
                const bool no_swim_water = no_swim_in_water && no_swim_above_water;

                const bool no_waterwalk_flags = (movementInfo.flags & MOVEMENTFLAG_WATERWALKING) == 0;
                const bool no_waterwalk_auras = !(plrMover->HasAuraType(SPELL_AURA_WATER_WALK) || plrMover->HasAuraType(SPELL_AURA_GHOST));

                if (cClientTimeDelta < 0)
                    cClientTimeDelta = 0;
                const float time_delta = cClientTimeDelta < 1500 ? float(cClientTimeDelta) / 1000.0f : 1.5f; // normalize time - 1.5 second allowed for heavy loaded server

                const float tg_z = (real_delta != 0 && no_fly_auras && no_swim_flags) ? (pow(delta_z, 2) / real_delta) : -99999; // movement distance tangents

                if (current_speed < plrMover->m_anti_Last_HSpeed && plrMover->m_anti_LastSpeedChangeTime == 0)
                    plrMover->m_anti_LastSpeedChangeTime = movementInfo.time + uint32(floor(((plrMover->m_anti_Last_HSpeed / current_speed) * 1500)) + 100); // 100ms above for random fluctuation

                const float allowed_delta = plrMover->m_transport || plrMover->m_temp_transport ? 2 : // movement distance allowed delta
                    pow(std::max(current_speed, plrMover->m_anti_Last_HSpeed) * time_delta, 2)
                    + 2                                                                             // minimum allowed delta
                    + (tg_z > 2.2 ? pow(delta_z, 2) / 2.37f : 0);                                     // mountain fall allowed delta

                if (movementInfo.time > plrMover->m_anti_LastSpeedChangeTime)
                {
                    plrMover->m_anti_Last_HSpeed = current_speed;                                    // store current speed
                    plrMover->m_anti_Last_VSpeed = -2.3f;
                    plrMover->m_anti_LastSpeedChangeTime = 0;
                }
                // end calculating section

                // AntiGravity (thanks to Meekro)
                const float JumpHeight = plrMover->m_anti_JumpBaseZ - movementInfo.pos.GetPositionZ();
                if (no_fly_auras && no_swim_in_water && plrMover->m_anti_JumpBaseZ != 0 && JumpHeight < plrMover->m_anti_Last_VSpeed)
                {
#ifdef ANTICHEAT_EXCEPTION_INFO
                    //TC_LOG_WARN("cheat", "AC2-%s, AntiGravity exception. JumpHeight = %f, Allowed Vertical Speed = %f",
                    //plrMover->GetName().c_str(), JumpHeight, plrMover->m_anti_Last_VSpeed);
#endif
                    check_passed = false;

                    // Tell the player "Sure, you can fly!"
                    {
                        WorldPacket data(SMSG_MOVE_SET_CAN_FLY, 12);
                        data << uint64(plrMover->GetGUID());
                        data << uint32(0);
                        SendPacket(&data);
                    }
                    // Then tell the player "Wait, no, you can't."
                    {
                        WorldPacket data(SMSG_MOVE_UNSET_CAN_FLY, 12);
                        data << uint64(plrMover->GetGUID());
                        data << uint32(0);
                        SendPacket(&data);
                    }
                    //plrMover->FallGround(2);
                }

    //if (plrMover)                                            // nothing is charmed, or player charmed
    // multi jump checks
                if (opcode == MSG_MOVE_JUMP)
                {
                    if (no_fly_auras && no_swim_water)
                    {
                        if (plrMover->m_anti_JumpCount >= 1)
                        {
                            // don't process new jump packet
                            check_passed = false;
#ifdef ANTICHEAT_EXCEPTION_INFO
                            //TC_LOG_WARN("cheat", "AC2-%s, Multijump exception.", plrMover->GetName().c_str(), JumpHeight, plrMover->m_anti_Last_VSpeed);
#endif

                            // Tell the player "Sure, you can fly!"
                            {
                                WorldPacket data(SMSG_MOVE_SET_CAN_FLY, 12);
                                data << uint64(plrMover->GetGUID());
                                data << uint32(0);
                                SendPacket(&data);
                            }
                            // Then tell the player "Wait, no, you can't."
                            {
                                WorldPacket data(SMSG_MOVE_UNSET_CAN_FLY, 12);
                                data << uint64(plrMover->GetGUID());
                                data << uint32(0);
                                SendPacket(&data);
                            }
                            //plrMover->FallGround(2);
                            plrMover->m_anti_JumpCount = 0;
                        }
                        else
                        {
                            plrMover->m_anti_JumpCount += 1;
                            plrMover->m_anti_JumpBaseZ = movementInfo.pos.GetPositionZ();
                        }
                    }
                    else
                        plrMover->m_anti_JumpCount = 0;
                }

                // speed and teleport hack checks
                if (real_delta > allowed_delta)
                {
                    #ifdef ANTICHEAT_EXCEPTION_INFO
                    if (real_delta < 4900.0f)
                    {
                        //TC_LOG_WARN("cheat", "AC2-%s, speed exception | cDelta=%f aDelta=%f | cSpeed=%f lSpeed=%f deltaTime=%f", plrMover->GetName().c_str(), real_delta, allowed_delta, current_speed, plrMover->m_anti_Last_HSpeed, time_delta);
                    } else {
                        check_passed = false;
                        TC_LOG_WARN("cheat", "AC2-%s, teleport exception | cDelta=%f aDelta=%f | cSpeed=%f lSpeed=%f deltaTime=%f", plrMover->GetName().c_str(), real_delta, allowed_delta, current_speed, plrMover->m_anti_Last_HSpeed, time_delta);
                    }
                    #endif
                }

                // mountain hack checks // 1.56f (delta_z < GetPlayer()->m_anti_Last_VSpeed))
                if (delta_z < plrMover->m_anti_Last_VSpeed && plrMover->m_anti_JumpCount == 0 && tg_z > 2.37f)
                {
                    #ifdef ANTICHEAT_EXCEPTION_INFO
                    //TC_LOG_WARN("cheat", "AC2-%s, mountain exception | tg_z=%f", plrMover->GetName().c_str(), tg_z);
                    #endif
                    check_passed = false;
                }
				
                // Fly hack checks
                if (no_fly_auras && !no_fly_flags)
                {
                    #ifdef ANTICHEAT_EXCEPTION_INFO // Aura numbers: 201, 206, 207, 208, 209, 211
                    //TC_LOG_WARN("cheat", "AC2-%s, flight exception. {SPELL_AURA_FLY=[%X]} {SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED=[%X]} {SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED=[%X]} {SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS=[%X]} {SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK=[%X]} {plrMover->GetVehicle()=[%X]}",
                        //plrMover->GetName().c_str(),
    					 //Last Updated By ShopWoW.ir                    
						 plrMover->HasAuraType(SPELL_AURA_FLY), plrMover->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED),
                        plrMover->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED), plrMover->HasAuraType(SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS),
                        plrMover->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK), plrMover->GetVehicle();
                    #endif
                    check_passed = false;

                    // Tell the player "Sure, you can fly!"
                    {
                        WorldPacket data(SMSG_MOVE_SET_CAN_FLY, 12);
                        data << uint64(plrMover->GetGUID());
                        data << uint32(0);
                        SendPacket(&data);
                    }
                    // Then tell the player "Wait, no, you can't."
                    {
                        WorldPacket data(SMSG_MOVE_UNSET_CAN_FLY, 12);
                        data << uint64(plrMover->GetGUID());
                        data << uint32(0);
                        SendPacket(&data);
                    }
                    //plrMover->FallGround(2);
                }

                // Waterwalk checks
                if (no_waterwalk_auras && !no_waterwalk_flags)
                {
                    #ifdef ANTICHEAT_EXCEPTION_INFO
                    //TC_LOG_WARN("cheat", "AC2-%s, waterwalk exception. [%X]{SPELL_AURA_WATER_WALK=[%X]}",
                    //    plrMover->GetName().c_str(), movementInfo.flags, plrMover->HasAuraType(SPELL_AURA_WATER_WALK));
                    #endif
                    check_passed = false;
                    // Tell the player "Sure, you can fly!"
                    {
                        WorldPacket data(SMSG_MOVE_SET_CAN_FLY, 12);
                        data << uint64(plrMover->GetGUID());
                        data << uint32(0);
                        SendPacket(&data);
                    }
                    // Then tell the player "Wait, no, you can't."
                    {
                        WorldPacket data(SMSG_MOVE_UNSET_CAN_FLY, 12);
                        data << uint64(plrMover->GetGUID());
                        data << uint32(0);
                        SendPacket(&data);
                    }
                }

                // Teleport To Plane checks
                if (no_swim_in_water && movementInfo.pos.GetPositionZ() < 0.0001f && movementInfo.pos.GetPositionZ() > -0.0001f)
                {
                    if (const Map *map = plrMover->GetMap())
                    {
                        float plane_z = map->GetHeight(movementInfo.pos.GetPositionX(), movementInfo.pos.GetPositionY(), MAX_HEIGHT) - movementInfo.pos.GetPositionZ();
                        plane_z = (plane_z < -500.0f) ? 0.0f : plane_z; // check holes in height map
                        if (plane_z > 0.1f || plane_z < -0.1f)
                        {
                            #ifdef ANTICHEAT_DEBUG
                            TC_LOG_WARN("cheat", "AC2-%s, teleport to plane exception. plane_z: %f", plrMover->GetName(), plane_z);
                            #endif
                            #ifdef ANTICHEAT_EXCEPTION_INFO
                            if (plrMover->m_anti_TeleToPlane_Count > World::GetTeleportToPlaneAlarms())
                            {
                                TC_LOG_WARN("cheat", "AC2-%s, teleport to plane exception. Exception count: %d", plrMover->GetName().c_str(), plrMover->m_anti_TeleToPlane_Count);
                            }
                            #endif
                            ++(plrMover->m_anti_TeleToPlane_Count);
                            check_passed = false;
                        }
                    }
                }
                else
                    plrMover->m_anti_TeleToPlane_Count = 0;
            }
        }
    }
     /* process position-change */
    if (check_passed)
    {
        //if (plrMover->IsSitState() && (movementInfo.flags & (MOVEMENTFLAG_MASK_MOVING | MOVEMENTFLAG_MASK_TURNING)))
        //    plrMover->SetStandState(UNIT_STAND_STATE_STAND);

        WorldPacket data(opcode, recvData.size());
        //movementInfo.time = movementInfo.time + m_clientTimeDelay + MOVEMENT_PACKET_TIME_DELAY;
        movementInfo.time = getMSTime(); // pussywizard: set to time of relocation (server time), constant addition may smoothen movement clientside, but client sees target on different position than the real serverside position

        movementInfo.guid = mover->GetGUID();
        WriteMovementInfo(&data, &movementInfo);
        mover->SendMessageToSet(&data, _player);

        //plrMover->UpdateFallInformationIfNeed(movementInfo, opcode);
        mover->m_movementInfo = movementInfo;

        //if (movementInfo.pos.GetPositionZ() < plrMover->GetMap()->GetMinHeight(movementInfo.pos.GetPositionX(), movementInfo.pos.GetPositionY()))
        //{
            //if (!(plrMover->GetBattleground() && plrMover->GetBattleground()->HandlePlayerUnderMap(_player)))
        // this is almost never true (pussywizard: only one packet when entering vehicle), normally use mover->IsVehicle()
        if (mover->GetVehicle())
        {
            mover->SetOrientation(movementInfo.pos.GetOrientation());
		    mover->UpdatePosition(movementInfo.pos);
            return;
        }

	    // pussywizard: previously always mover->UpdatePosition(movementInfo.pos);
	    if (movementInfo.flags & MOVEMENTFLAG_ONTRANSPORT && mover->GetTransport())
	    {
		    float x, y, z, o;
		    movementInfo.transport.pos.GetPosition(x, y, z, o);
		    mover->GetTransport()->CalculatePassengerPosition(x, y, z, &o);
		    mover->UpdatePosition(x, y, z, o);
	    }
	    else
		    mover->UpdatePosition(movementInfo.pos);

	    // fall damage generation (ignore in flight case that can be triggered also at lags in moment teleportation to another map).
	    // Xinef: moved it here, previously StopMoving function called when player died relocated him to last saved coordinates (which were in air)
        if (opcode == MSG_MOVE_FALL_LAND && plrMover && !plrMover->IsInFlight() && (!plrMover->GetTransport()))
        {
            // movement anticheat
            plrMover->m_anti_JumpCount = 0;
            plrMover->m_anti_JumpBaseZ = 0;
            if (!vehMover)
                plrMover->HandleFall(movementInfo);
        }
	    // Xinef: interrupt parachutes upon falling or landing in water
	    if (opcode == MSG_MOVE_FALL_LAND || opcode == MSG_MOVE_START_SWIM)
		    mover->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_LANDING); // Parachutes
        
        if (plrMover && !vehMover)                               // nothing is charmed, or player charmed
        {
            if (plrMover->IsSitState() && (movementInfo.flags & (MOVEMENTFLAG_MASK_MOVING | MOVEMENTFLAG_MASK_TURNING)))
                plrMover->SetStandState(UNIT_STAND_STATE_STAND);

            plrMover->UpdateFallInformationIfNeed(movementInfo, opcode);

            float underMapValueZ;

            switch (plrMover->GetMapId())
            {
                // NOTE: this is actually called many times while falling
                // even after the player has been teleported away
                /// @todo discard movement packets after the player is rooted
                //if (plrMover->IsAlive())
            case 617: underMapValueZ = 3.0f; break; // Dalaran Sewers
            case 618: underMapValueZ = 28.0f; break; // Ring of Valor
            default: underMapValueZ = -500.0f; break;
            }

            if (movementInfo.pos.GetPositionZ() < underMapValueZ)
                if (!plrMover->GetBattleground() || !plrMover->GetBattleground()->HandlePlayerUnderMap(_player))
                {
                    //plrMover->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_IS_OUT_OF_BOUNDS);
                    //plrMover->EnvironmentalDamage(DAMAGE_FALL_TO_VOID, GetPlayer()->GetMaxHealth());
                    // player can be alive if GM/etc
                    // change the death state to CORPSE to prevent the death timer from
                    // starting in the next player update
                    if (plrMover->IsAlive())
                    {
                        plrMover->EnvironmentalDamage(DAMAGE_FALL_TO_VOID, GetPlayer()->GetMaxHealth());
                        // player can be alive if GM
                        if (plrMover->IsAlive())
                            plrMover->KillPlayer();
                    }
                        //plrMover->KillPlayer();
                    // movement anticheat
                if (plrMover->m_anti_AlarmCount > 0)
                {
                    //TC_LOG_WARN("cheat", "AC2-%s produce %d anticheat alarms.", plrMover->GetName().c_str(), plrMover->m_anti_AlarmCount);
                    plrMover->m_anti_AlarmCount = 0;
                }
            }
            // end movement anticheat
        }
    }
    else if (plrMover)
    {
        if (plrMover->m_transport)
        {
            plrMover->m_transport->RemovePassenger(plrMover);
            plrMover->m_transport = NULL;
        }
        plrMover->m_temp_transport = NULL;
        ++(plrMover->m_anti_AlarmCount);
        WorldPacket data;
        plrMover->SetUnitMovementFlags(0);
        plrMover->SendTeleportPacket(movementInfo.pos, false);
        plrMover->BuildHeartBeatMsg(&data);
        plrMover->SendMessageToSet(&data, true);
        //}
    }
}

void WorldSession::HandleForceSpeedChangeAck(WorldPacket &recvData)
{
    /* extract packet */
    ObjectGuid guid;
    uint32 unk1;
    float  newspeed;

    recvData >> guid.ReadAsPacked();

    // now can skip not our packet
    if (_player->GetGUID() != guid)
    {
        recvData.rfinish();                   // prevent warnings spam
        return;
    }

    // continue parse packet

    recvData >> unk1;                                      // counter or moveEvent

    MovementInfo movementInfo;
    movementInfo.guid = guid;
    ReadMovementInfo(recvData, &movementInfo);

    recvData >> newspeed;
    /*----------------*/

    // client ACK send one packet for mounted/run case and need skip all except last from its
    // in other cases anti-cheat check can be fail in false case
    UnitMoveType move_type;
    UnitMoveType force_move_type;

    static char const* move_type_name[MAX_MOVE_TYPE] = {  "Walk", "Run", "RunBack", "Swim", "SwimBack", "TurnRate", "Flight", "FlightBack", "PitchRate" };

    switch (recvData.GetOpcode())
    {
        case CMSG_FORCE_WALK_SPEED_CHANGE_ACK:          move_type = MOVE_WALK;          force_move_type = MOVE_WALK;        break;
        case CMSG_FORCE_RUN_SPEED_CHANGE_ACK:           move_type = MOVE_RUN;           force_move_type = MOVE_RUN;         break;
        case CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK:      move_type = MOVE_RUN_BACK;      force_move_type = MOVE_RUN_BACK;    break;
        case CMSG_FORCE_SWIM_SPEED_CHANGE_ACK:          move_type = MOVE_SWIM;          force_move_type = MOVE_SWIM;        break;
        case CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK:     move_type = MOVE_SWIM_BACK;     force_move_type = MOVE_SWIM_BACK;   break;
        case CMSG_FORCE_TURN_RATE_CHANGE_ACK:           move_type = MOVE_TURN_RATE;     force_move_type = MOVE_TURN_RATE;   break;
        case CMSG_FORCE_FLIGHT_SPEED_CHANGE_ACK:        move_type = MOVE_FLIGHT;        force_move_type = MOVE_FLIGHT;      break;
        case CMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK:   move_type = MOVE_FLIGHT_BACK;   force_move_type = MOVE_FLIGHT_BACK; break;
        case CMSG_FORCE_PITCH_RATE_CHANGE_ACK:          move_type = MOVE_PITCH_RATE;    force_move_type = MOVE_PITCH_RATE;  break;
        default:
            TC_LOG_ERROR("network", "WorldSession::HandleForceSpeedChangeAck: Unknown move type opcode: %u", recvData.GetOpcode());
            return;
    }

    // skip all forced speed changes except last and unexpected
    // in run/mounted case used one ACK and it must be skipped.m_forced_speed_changes[MOVE_RUN} store both.
    if (_player->m_forced_speed_changes[force_move_type] > 0)
    {
        --_player->m_forced_speed_changes[force_move_type];
        if (_player->m_forced_speed_changes[force_move_type] > 0)
            return;
    }

    if (!_player->GetTransport() && std::fabs(_player->GetSpeed(move_type) - newspeed) > 0.01f)
    {
        if (_player->GetSpeed(move_type) > newspeed)         // must be greater - just correct
        {
            TC_LOG_ERROR("network", "%sSpeedChange player %s is NOT correct (must be %f instead %f), force set to correct value",
                move_type_name[move_type], _player->GetName().c_str(), _player->GetSpeed(move_type), newspeed);
            _player->SetSpeedRate(move_type, _player->GetSpeedRate(move_type));
        }
        else                                                // must be lesser - cheating
        {
            TC_LOG_DEBUG("misc", "Player %s from account id %u kicked for incorrect speed (must be %f instead %f)",
                _player->GetName().c_str(), _player->GetSession()->GetAccountId(), _player->GetSpeed(move_type), newspeed);
            _player->GetSession()->KickPlayer();
        }
    }
}

void WorldSession::HandleSetActiveMoverOpcode(WorldPacket &recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_SET_ACTIVE_MOVER");

    ObjectGuid guid;
    recvData >> guid;

    if (GetPlayer()->IsInWorld())
        if (_player->m_unitMovedByMe->GetGUID() != guid)
            TC_LOG_DEBUG("network", "HandleSetActiveMoverOpcode: incorrect mover guid: mover is %s and should be %s" , guid.ToString().c_str(), _player->m_unitMovedByMe->GetGUID().ToString().c_str());
}

void WorldSession::HandleMoveNotActiveMover(WorldPacket &recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_MOVE_NOT_ACTIVE_MOVER");

    ObjectGuid old_mover_guid;
    recvData >> old_mover_guid.ReadAsPacked();

    MovementInfo mi;
    ReadMovementInfo(recvData, &mi);

    mi.guid = old_mover_guid;

    _player->m_movementInfo = mi;
}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket& /*recvData*/)
{
    WorldPacket data(SMSG_MOUNTSPECIAL_ANIM, 8);
    data << uint64(GetPlayer()->GetGUID());

    GetPlayer()->SendMessageToSet(&data, false);
}

void WorldSession::HandleMoveKnockBackAck(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_MOVE_KNOCK_BACK_ACK");

    ObjectGuid guid;
    recvData >> guid.ReadAsPacked();

    if (_player->m_unitMovedByMe->GetGUID() != guid)
        return;

    recvData.read_skip<uint32>();                          // unk

    MovementInfo movementInfo;
    ReadMovementInfo(recvData, &movementInfo);

    // Save movement flags
    _player->SetUnitMovementFlags(movementInfo.flags);
    #ifdef ANTICHEAT_DEBUG
    //TC_LOG_WARN("cheat", "%s CMSG_MOVE_KNOCK_BACK_ACK: time: %d, fall time: %d | xyzo: %f,%f,%fo(%f) flags[%X] Vspeed: %f, Hspeed: %f", GetPlayer()->GetName(), movementInfo.time, movementInfo.fallTime, movementInfo.pos.m_positionX, movementInfo.pos.m_positionY, movementInfo.pos.m_positionZ, movementInfo.pos.m_orientation, movementInfo.flags, movementInfo.jump.zspeed, movementInfo.jump.xyspeed);
    #endif

    _player->m_movementInfo = movementInfo;

    _player->m_anti_Last_HSpeed = movementInfo.jump.xyspeed;
    _player->m_anti_Last_VSpeed = movementInfo.jump.zspeed < 3.2f ? movementInfo.jump.zspeed - 1.0f : 3.2f;

    const uint32 dt = (_player->m_anti_Last_VSpeed < 0) ? int(ceil(_player->m_anti_Last_VSpeed/-25)*1000) : int(ceil(_player->m_anti_Last_VSpeed/25)*1000);
    _player->m_anti_LastSpeedChangeTime = movementInfo.time + dt + 1000;

    WorldPacket data(MSG_MOVE_KNOCK_BACK, 66);
    data << guid.WriteAsPacked();
    _player->BuildMovementPacket(&data);

    // knockback specific info
    data << movementInfo.jump.sinAngle;
    data << movementInfo.jump.cosAngle;
    data << movementInfo.jump.xyspeed;
    data << movementInfo.jump.zspeed;

    _player->SendMessageToSet(&data, false);
}

void WorldSession::HandleMoveHoverAck(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_MOVE_HOVER_ACK");

    ObjectGuid guid;                                        // guid - unused
    recvData >> guid.ReadAsPacked();

    recvData.read_skip<uint32>();                           // unk

    MovementInfo movementInfo;
    ReadMovementInfo(recvData, &movementInfo);

    recvData.read_skip<uint32>();                           // unk2
}

void WorldSession::HandleMoveWaterWalkAck(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_MOVE_WATER_WALK_ACK");

    ObjectGuid guid;                                        // guid - unused
    recvData >> guid.ReadAsPacked();

    recvData.read_skip<uint32>();                           // unk

    MovementInfo movementInfo;
    ReadMovementInfo(recvData, &movementInfo);

    recvData.read_skip<uint32>();                           // unk2
}

void WorldSession::HandleSummonResponseOpcode(WorldPacket& recvData)
{
    if (!_player->IsAlive() || _player->IsInCombat())
        return;

    ObjectGuid summoner_guid;
    bool agree;
    recvData >> summoner_guid;
    recvData >> agree;

    _player->SummonIfPossible(agree);
}

void WorldSession::HandleMoveTimeSkippedOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_MOVE_TIME_SKIPPED");

    ObjectGuid guid;
    uint32 timeSkipped;
    recvData >> guid.ReadAsPacked();
    recvData >> timeSkipped;

    Unit* mover = GetPlayer()->m_unitMovedByMe;

    if (!mover)
    {
        TC_LOG_WARN("entities.player", "WorldSession::HandleMoveTimeSkippedOpcode wrong mover state from the unit moved by the player %s", GetPlayer()->GetGUID().ToString().c_str());
        return;
    }

    // prevent tampered movement data
    if (guid != mover->GetGUID())
    {
        TC_LOG_WARN("entities.player", "WorldSession::HandleMoveTimeSkippedOpcode wrong guid from the unit moved by the player %s", GetPlayer()->GetGUID().ToString().c_str());
        return;
    }

    mover->m_movementInfo.time += timeSkipped;

    WorldPacket data(MSG_MOVE_TIME_SKIPPED, recvData.size());
    data << guid.WriteAsPacked();
    data << timeSkipped;
    GetPlayer()->SendMessageToSet(&data, false);
}

void WorldSession::HandleTimeSyncResp(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_TIME_SYNC_RESP");

    uint32 counter, clientTimestamp;
    recvData >> counter >> clientTimestamp;

    if (_pendingTimeSyncRequests.count(counter) == 0)
        return;

    uint32 serverTimeAtSent = _pendingTimeSyncRequests.at(counter);
    _pendingTimeSyncRequests.erase(counter);

    // time it took for the request to travel to the client, for the client to process it and reply and for response to travel back to the server.
    // we are going to make 2 assumptions:
    // 1) we assume that the request processing time equals 0.
    // 2) we assume that the packet took as much time to travel from server to client than it took to travel from client to server.
    uint32 roundTripDuration = getMSTimeDiff(serverTimeAtSent, recvData.GetReceivedTime());
    uint32 lagDelay = roundTripDuration / 2;

    /*
    clockDelta = serverTime - clientTime
    where
    serverTime: time that was displayed on the clock of the SERVER at the moment when the client processed the SMSG_TIME_SYNC_REQUEST packet.
    clientTime:  time that was displayed on the clock of the CLIENT at the moment when the client processed the SMSG_TIME_SYNC_REQUEST packet.

    Once clockDelta has been computed, we can compute the time of an event on server clock when we know the time of that same event on the client clock,
    using the following relation:
    serverTime = clockDelta + clientTime
    */
    int64 clockDelta = (int64)(serverTimeAtSent + lagDelay) - (int64)clientTimestamp;
    _timeSyncClockDeltaQueue.push_back(std::pair<int64, uint32>(clockDelta, roundTripDuration));
    ComputeNewClockDelta();
}

void WorldSession::ComputeNewClockDelta()
{
    // implementation of the technique described here: https://web.archive.org/web/20180430214420/http://www.mine-control.com/zack/timesync/timesync.html
    // to reduce the skew induced by dropped TCP packets that get resent.

    using namespace boost::accumulators;

    accumulator_set<uint32, features<tag::mean, tag::median, tag::variance(lazy)> > latencyAccumulator;

    for (auto pair : _timeSyncClockDeltaQueue)
        latencyAccumulator(pair.second);

    uint32 latencyMedian = static_cast<uint32>(std::round(median(latencyAccumulator)));
    uint32 latencyStandardDeviation = static_cast<uint32>(std::round(sqrt(variance(latencyAccumulator))));

    accumulator_set<int64, features<tag::mean> > clockDeltasAfterFiltering;
    uint32 sampleSizeAfterFiltering = 0;
    for (auto pair : _timeSyncClockDeltaQueue)
    {
        if (pair.second < latencyStandardDeviation + latencyMedian) {
            clockDeltasAfterFiltering(pair.first);
            sampleSizeAfterFiltering++;
        }
    }

    if (sampleSizeAfterFiltering != 0)
    {
        int64 meanClockDelta = static_cast<int64>(std::round(mean(clockDeltasAfterFiltering)));
        if (std::abs(meanClockDelta - _timeSyncClockDelta) > 25)
            _timeSyncClockDelta = meanClockDelta;
    }
    else if (_timeSyncClockDelta == 0)
    {
        std::pair<int64, uint32> back = _timeSyncClockDeltaQueue.back();
        _timeSyncClockDelta = back.first;
    }
}
