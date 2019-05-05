/*
morpher_npc
Developer: Project2k14 - Ac-web.org
Status: 100%
Comment: This NPC is used for Cataclysm, morphs into racial morphs (Changes the race but not the gear you wear)
Tested: Cataclysm: Yes [X] No [ ] WOTLK: Yes [ ] No [X]
Morph ID's: Thanks, http://hearthstone-wow.de/forum/index.php/Thread/693-Race-Morphs-IDs/
*/

enum Items
{
    REQUIRED_ITEM = 200024, //item required to cast spells
    REQUIRED_COUNT = 10 //amount of items required
};

#include "ScriptedGossip.h"

class MorphNPC : public CreatureScript
{
public:
    MorphNPC() : CreatureScript("morpher_npc") { }

    class MorphNPCAI : public ScriptedAI
    {
    public:
        MorphNPCAI(Creature* creature) : ScriptedAI(creature) { }

        bool GossipHello(Player* player) override
        {
            return OnGossipHello(player, me);
            return true;
        }

        bool OnGossipHello(Player * player, Creature * creature)
        {
            player->PlayerTalkClass->ClearMenus();
            AddGossipItemFor(player, 4, "|cff0000FF|TInterface\\icons\\inv_bannerpvp_02:30 |t  Alliance Morphs|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            AddGossipItemFor(player, 4, "|cffFF3300|TInterface\\icons\\inv_bannerpvp_01:30 |t  Horde Morphs|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            AddGossipItemFor(player, 4, "|cff990099|TInterface\\icons\\spell_holy_dispelmagic:30 |t Demorph|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 27);
            AddGossipItemFor(player, 4, "|cffFF0000|TInterface\\icons\\achievement_reputation_01:35 |t  Nevermind...|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            AddGossipItemFor(player, 4, "Price is x10 [Morph Token]!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            return true;
        }

        bool GossipSelect(Player* player, uint32 /*menu_id*/, uint32 gossipListId) override
        {
            uint32 sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
            uint32 action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            return OnGossipSelect(player, me, sender, action);
        }

        bool OnGossipSelect(Player * player, Creature * creature, uint32 sender, uint32 actions)
        {
            player->PlayerTalkClass->ClearMenus();

            if (sender != GOSSIP_SENDER_MAIN)
                return false;

            switch (actions)
            {
            case GOSSIP_ACTION_INFO_DEF + 1: /* Shows the Alliance Morphs menu */
                AddGossipItemFor(player, 0, "|cff0000FF|TInterface\\icons\\achievement_character_human_male:30 |t  Human Male|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                AddGossipItemFor(player, 0, "|cff0000FF|TInterface\\icons\\achievement_character_human_female:30 |t  Human Female|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                AddGossipItemFor(player, 0, "|cff0000FF|TInterface\\icons\\achievement_character_dwarf_male:30 |t  Dwarf Male|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
                AddGossipItemFor(player, 0, "|cff0000FF|TInterface\\icons\\achievement_character_dwarf_female:30 |t  Dwarf Female|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
                AddGossipItemFor(player, 0, "|cff0000FF|TInterface\\icons\\achievement_character_nightelf_male:30 |t  Nightelf Male|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
                AddGossipItemFor(player, 0, "|cff0000FF|TInterface\\icons\\achievement_character_nightelf_female:30 |t  Nightelf Female|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
                AddGossipItemFor(player, 0, "|cff0000FF|TInterface\\icons\\achievement_character_gnome_male:30 |t  Gnome Male|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
                AddGossipItemFor(player, 0, "|cff0000FF|TInterface\\icons\\achievement_character_gnome_female:30 |t  Gnome Female|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
                AddGossipItemFor(player, 0, "|cff0000FF|TInterface\\icons\\achievement_character_draenei_male:30 |t  Draenei Male|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
                AddGossipItemFor(player, 0, "|cff0000FF|TInterface\\icons\\achievement_character_draenei_female:30 |t  Draenei Female|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
                AddGossipItemFor(player, 0, "|cffFF0000|TInterface\\icons\\achievement_reputation_01:35 |t  Nevermind...|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                break;

            case GOSSIP_ACTION_INFO_DEF + 2: /* Shows the Horde Morphs menu */
                AddGossipItemFor(player, 0, "|cffFF3300|TInterface\\icons\\achievement_character_orc_male:30 |t  Orc Male|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 17);
                AddGossipItemFor(player, 0, "|cffFF3300|TInterface\\icons\\achievement_character_orc_female:30 |t  Orc Female|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 18);
                AddGossipItemFor(player, 0, "|cffFF3300|TInterface\\icons\\achievement_character_undead_male:30 |t  Undead Male|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 19);
                AddGossipItemFor(player, 0, "|cffFF3300|TInterface\\icons\\achievement_character_undead_female:30 |t  Undead Female|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 20);
                AddGossipItemFor(player, 0, "|cffFF3300|TInterface\\icons\\achievement_character_tauren_male:30 |t  Tauren Male|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
                AddGossipItemFor(player, 0, "|cffFF3300|TInterface\\icons\\achievement_character_tauren_female:30 |t  Tauren Female|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 22);
                AddGossipItemFor(player, 0, "|cffFF3300|TInterface\\icons\\achievement_character_troll_male:30 |t  Troll Male|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 23);
                AddGossipItemFor(player, 0, "|cffFF3300|TInterface\\icons\\achievement_character_troll_female:30 |t  Troll Female|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 24);
                AddGossipItemFor(player, 0, "|cffFF3300|TInterface\\icons\\achievement_character_bloodelf_male:30 |t  Bloodelf Male|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 25);
                AddGossipItemFor(player, 0, "|cffFF3300|TInterface\\icons\\achievement_character_bloodelf_female:30 |t  Bloodelf Female|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 26);
                AddGossipItemFor(player, 0, "|cffFF0000|TInterface\\icons\\achievement_reputation_01:35 |t  Nevermind...|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                break;


            case GOSSIP_ACTION_INFO_DEF + 4: /* Uses the Nevermind option and closes the menu */
                CloseGossipMenuFor(player);
                break;
                /* Alliance Morph List */
            case GOSSIP_ACTION_INFO_DEF + 5: /* Human Male Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(19723);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 6: /* Human Female Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(19724);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 7: /* Dwarf Male Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(20317);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 8: /* Dwarf Female Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(37918);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 9: /* Nightelf Male Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(20318);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 10:/* Nightelf Female Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(37919);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 11:/* Gnome Male Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(20580);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 12:/* Gnome Female Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(20320);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 13:/* Draenei Male Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(37916);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 14:/* Draenei Female Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(20323);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;
                /* Horde Morph List */
            case GOSSIP_ACTION_INFO_DEF + 17:/* Orc Male Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(37920);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 18:/* Orc Female Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(20316);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 19:/* Undead Male Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(37923);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 20:/* Undead Female Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(37924);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 21:/* Tauren Male Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(20319);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 22:/* Tauren Female Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(20584);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 23:/* Troll Male Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(20321);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 24:/* Troll Female Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(37922);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 26:/* Bloodelf Female Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(20370);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 25:/* Bloodelf Male Morph*/
                if (player->HasItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, REQUIRED_COUNT, true, false);
                    player->SetDisplayId(20368);
                    creature->Whisper("You got the morph you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    creature->Whisper("You do not have enough [Morph Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 27:/* Demorph option*/
                CloseGossipMenuFor(player);
                player->DeMorph();
                break;
            }
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new MorphNPCAI(creature);
    }
};

void AddSC_morpher_npc()
{
	new MorphNPC;
}
