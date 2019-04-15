#include "ScriptPCH.h"
#include "Chat.h"
#include "World.h"
#include <cstring>

class System_OnPvPKill : public PlayerScript
{
    public:
        System_OnPvPKill() : PlayerScript("System_OnPvPKill") {}

        
        void OnPVPKill(Player *pKiller, Player *pVictim)
        {
            // Send in Chat = SMSG_SERVER_MESSAGE
            // Send as Raid Warning = SMSG_NOTIFICATION
            WorldPacket data(SMSG_NOTIFICATION, 100);
            WorldPacket data2(SMSG_NOTIFICATION, 100);
            int32 ItemReward = 29434;
            int32 AmountOfRewardsOnKillStreak[4] = { 5, 10, 15, 30 };
            // Announce on X kills
            const int32 pKillerStreak1 = 5;
            const int32 pKillerStreak2 = 10;
            const int32 pKillerStreak3 = 25;
            const int32 pKillerStreak4 = 50;
            int32 KillStreaks[4] = { pKillerStreak1, pKillerStreak2, pKillerStreak3, pKillerStreak4 };

            char msg[500];
            uint32 pKillerGUID = pKiller->GetGUID();
            uint32 VictimGUID = pVictim->GetGUID();
            std::string pKillername = pKiller->GetName();
            std::string killedname = pVictim->GetName();

            struct SystemInfo
            {
                uint32 KillStreak;
                uint32 LastGUIDKill;
            };

            static std::map<uint32, SystemInfo> KillStreakData;

            //if (pKillerGUID == VictimGUID || KillStreakData[pKillerGUID].LastGUIDKill == VictimGUID)
                //return;

            if (KillStreakData[VictimGUID].KillStreak >= 5)
            {
                snprintf(msg, 500, "|cffFF0000[PVP SYSTEM]: |cff00FF00%s|r ended |cff00FF00%s|r's killing streak of |cff00FFFF%i|r!", pKillername.c_str(), killedname.c_str(), KillStreakData[VictimGUID].KillStreak);
                data << msg;
                sWorld->SendGlobalMessage(&data);
            }


            KillStreakData[pKillerGUID].KillStreak++;
            KillStreakData[VictimGUID].KillStreak = 0;
            KillStreakData[pKillerGUID].LastGUIDKill = VictimGUID;
            KillStreakData[VictimGUID].LastGUIDKill = 0;

            snprintf(msg, 500, "|cffFF0000[PVP SYSTEM]: |cff00FF00%s|r has killed |cff00FF00%s|r", pKillername.c_str(), killedname.c_str());
            data2 << msg;
            sWorld->SendGlobalMessage(&data2);

            switch (KillStreakData[pKillerGUID].KillStreak)
            {
            case pKillerStreak1:
                snprintf(msg, 500, "|cffFF0000[PVP SYSTEM]: |cff00FF00%s|r has killed |cff00FF00%s|r and is on |cff00FFFF%i|r killing streak!", pKillername.c_str(), killedname.c_str(), pKillerStreak1);
                data2 << msg;
                sWorld->SendGlobalMessage(&data2);
                pKiller->AddItem(ItemReward, AmountOfRewardsOnKillStreak[1]);
                pKiller->ModifyHonorPoints(500);
                pKiller->CastSpell(pKiller, 24378, true);
                break;

            case pKillerStreak2:
                snprintf(msg, 500, "|cffFF0000[PVP SYSTEM]: |cff00FF00%s|r has killed |cff00FF00%s|r and is on |cff00FFFF%i|r killing streak!", pKillername.c_str(), killedname.c_str(), pKillerStreak2);
                data2 << msg;
                sWorld->SendGlobalMessage(&data2);
                pKiller->AddItem(ItemReward, AmountOfRewardsOnKillStreak[2]);
                pKiller->ModifyHonorPoints(1000);
                pKiller->CastSpell(pKiller, 24378, true);
                break;

            case pKillerStreak3:
                snprintf(msg, 500, "|cffFF0000[PVP SYSTEM]: |cff00FF00%s|r has killed |cff00FF00%s|r and is on |cff00FFFF%i|r killing streak!", pKillername.c_str(), killedname.c_str(), pKillerStreak3);
                data2 << msg;
                sWorld->SendGlobalMessage(&data2);
                pKiller->AddItem(ItemReward, AmountOfRewardsOnKillStreak[3]);
                pKiller->ModifyHonorPoints(1500);
                pKiller->CastSpell(pKiller, 24378, true);
                break;

            case pKillerStreak4:
                snprintf(msg, 500, "|cffFF0000[PVP SYSTEM]: |cff00FF00%s|r has killed |cff00FF00%s|r and is on |cff00FFFF%i|r killing streak!", pKillername.c_str(), killedname.c_str(), pKillerStreak4);
                data2 << msg;
                sWorld->SendGlobalMessage(&data2);
                KillStreakData[pKillerGUID].KillStreak = 0;
                pKiller->AddItem(ItemReward, AmountOfRewardsOnKillStreak[4]);
                pKiller->ModifyArenaPoints(100);
                pKiller->CastSpell(pKiller, 24378, true);
                break;
            }
        }
};

void AddSC_System()
{
    new System_OnPvPKill;
}
