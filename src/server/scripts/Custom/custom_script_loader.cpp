/*
 * Copyright (C) 2008-2019 TrinityCore <https://www.trinitycore.org/>
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

// This is where scripts' loading functions should be declared:
void AddSC_Transmogrification();
void AddSC_npc_teleport();
void AddSC_REFORGER_NPC();
void AddSC_Professions_NPC();
void AddSC_System();
void AddSC_npc_1v1arena();
void AddSC_login_announcer();
void AddSC_Premium_System();
void AddSC_start_guild();
void AddSC_System_Censure();
void AddSC_item_custom_summon();
void AddSC_Npc_Beastmaster();
void AddSC_npc_enchantment();
void AddSC_TitleNpc();
void AddSC_dungeon_rewards();
void AddSC_VisualNPC();
void AddSC_SpellRegulator();
void AddSC_custom_reload_commands();
void AddSC_morpher_npc();

// The name of this function should match:
// void Add${NameOfDirectory}Scripts()
void AddCustomScripts()
{
    AddSC_Professions_NPC();
    AddSC_System();
    AddSC_npc_1v1arena();
    AddSC_login_announcer();
    AddSC_SpellRegulator();
    AddSC_start_guild();
    AddSC_System_Censure();
    AddSC_item_custom_summon();
    AddSC_Npc_Beastmaster();
    AddSC_npc_teleport();
    AddSC_npc_enchantment();
    AddSC_morpher_npc();
    AddSC_TitleNpc();
    AddSC_dungeon_rewards();
    AddSC_VisualNPC();
    AddSC_Premium_System();
    AddSC_custom_reload_commands();
    // 19
    // 20
    // 21
    // 22
    // 23
    // 24
    // 25
    // 26
    // 27
    // 28
    // 29
    // 30
    // 31
    // 32
    // 33
    // 34
    // 35
    // 36
    // 37
    AddSC_Transmogrification();
    AddSC_REFORGER_NPC();
}
