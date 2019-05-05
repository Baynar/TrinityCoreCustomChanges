#include "ScriptedGossip.h"

enum Spells
{
	VISUAL_1 = 60451, //spell 1
	VISUAL_2 = 56075, //spell 2
	VISUAL_3 = 42438, //spell 3
	VISUAL_4 = 45948, //spell 4
	VISUAL_5 = 37805, //spell 5
	VISUAL_6 = 35466, //spell 6
	VISUAL_7 = 37810, //spell 7
	VISUAL_8 = 20373, //spell 8
	VISUAL_9 = 20374, //spell 9
	VISUAL_10 = 20370, //spell 10

    VISUAL_11 = 22580, //spell 1
    VISUAL_12 = 56572, //spell 2
    VISUAL_13 = 36006, //spell 3
    VISUAL_14 = 12898, //spell 4
    VISUAL_15 = 46934, //spell 5
    VISUAL_16 = 46933, //spell 6
    VISUAL_17 = 19502, //spell 7
    VISUAL_18 = 40570, //spell 8
    VISUAL_19 = 58812, //spell 9
    VISUAL_20 = 48200, //spell 10

    VISUAL_21 = 13913, //spell 1
    VISUAL_22 = 33569, //spell 2
    VISUAL_23 = 38841, //spell 3
    VISUAL_24 = 28330, //spell 4
    VISUAL_25 = 71986, //spell 5
    VISUAL_26 = 40849, //spell 6
    VISUAL_27 = 43866, //spell 7
    VISUAL_28 = 49757, //spell 8
    VISUAL_29 = 47840, //spell 9
    VISUAL_30 = 60044, //spell 10

    VISUAL_31 = 42344, //spell 1
    VISUAL_32 = 59069, //spell 2
    VISUAL_33 = 42709, //spell 3
    VISUAL_34 = 46679, //spell 4
    VISUAL_35 = 36945, //spell 5
    VISUAL_36 = 62192, //spell 6
    VISUAL_37 = 61023, //spell 7
    VISUAL_38 = 35850, //spell 8
    VISUAL_39 = 72523, //spell 9
    VISUAL_40 = 45870, //spell 10

    VISUAL_41 = 51193, //spell 1
    VISUAL_42 = 47760, //spell 2
    VISUAL_43 = 42294, //spell 3
    VISUAL_44 = 39284, //spell 4
    VISUAL_45 = 51283, //spell 5
    VISUAL_46 = 59069, //spell 6
    VISUAL_47 = 35926, //spell 7
    VISUAL_48 = 58226 //spell 8
};

enum Items
{
	REQUIRED_ITEM = 200019 //item required to cast spells
};


class VisualNPC : public CreatureScript
{
public:
	VisualNPC() : CreatureScript("Visual_NPC") { }

    class VisualNPCAI : public ScriptedAI
    {
    public:
        VisualNPCAI(Creature* creature) : ScriptedAI(creature) { }

        bool GossipHello(Player* player) override
        {
            return OnGossipHello(player, me);
            return true;
        }

	    bool OnGossipHello(Player* player, Creature* creature)
	    {
            player->PlayerTalkClass->ClearMenus();
            AddGossipItemFor(player, 4, "Visuals Page - 1", GOSSIP_SENDER_MAIN, 996);
            AddGossipItemFor(player, 4, "Visuals Page - 2", GOSSIP_SENDER_MAIN, 997);
            AddGossipItemFor(player, 4, "Visuals Page - 3", GOSSIP_SENDER_MAIN, 998);
            AddGossipItemFor(player, 4, "Visuals Page - 4", GOSSIP_SENDER_MAIN, 999);
            AddGossipItemFor(player, 4, "Visuals Page - 5", GOSSIP_SENDER_MAIN, 1000);
            AddGossipItemFor(player, 4, "Price is x5 [Visual Token]!", GOSSIP_SENDER_MAIN, 500);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
		    return true;
	    }

        bool GossipSelect(Player* player, uint32 /*menu_id*/, uint32 gossipListId) override
        {
            uint32 sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
            uint32 action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            return OnGossipSelect(player, me, sender, action);
        }

        bool OnGossipSelect(Player* player, Creature* pCreature, uint32 uiSender, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
            case 50:
                CloseGossipMenuFor(player);
                break;
            case 996:
                AddGossipItemFor(player, 4, "Corruption of Time", GOSSIP_SENDER_MAIN, 1001);
                AddGossipItemFor(player, 4, "Sphere Visual", GOSSIP_SENDER_MAIN, 1002);
                AddGossipItemFor(player, 4, "Headless Horseman Climax - Enraged Visual", GOSSIP_SENDER_MAIN, 1003);
                AddGossipItemFor(player, 4, "Oil Coat", GOSSIP_SENDER_MAIN, 1004);
                //AddGossipItemFor(player, 4, "Human Female Illusion TEST", GOSSIP_SENDER_MAIN, 1005);
                //AddGossipItemFor(player, 4, "Human Male Illusion TEST", GOSSIP_SENDER_MAIN, 1006);
                //AddGossipItemFor(player, 4, "Tauren Male Illusion TEST", GOSSIP_SENDER_MAIN, 1007);
                AddGossipItemFor(player, 4, "Tag: Sleep", GOSSIP_SENDER_MAIN, 1008);
                AddGossipItemFor(player, 4, "Tag: Arrow", GOSSIP_SENDER_MAIN, 1009);
                AddGossipItemFor(player, 4, "Tag: Red Glow", GOSSIP_SENDER_MAIN, 1010);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
                break;
            case 997:
                AddGossipItemFor(player, 4, "Glowy (Yellow)", GOSSIP_SENDER_MAIN, 1011);
                AddGossipItemFor(player, 4, "Dark Aura", GOSSIP_SENDER_MAIN, 1012);
                AddGossipItemFor(player, 4, "Fel Fire Aura", GOSSIP_SENDER_MAIN, 1013);
                AddGossipItemFor(player, 4, "Smoke Aura Visual", GOSSIP_SENDER_MAIN, 1014);
                AddGossipItemFor(player, 4, "Cosmetic - Arcane Force Shield (Blue x2)", GOSSIP_SENDER_MAIN, 1015);
                AddGossipItemFor(player, 4, "Cosmetic - Arcane Force Shield (Blue)", GOSSIP_SENDER_MAIN, 1016);
                AddGossipItemFor(player, 4, "Sickly Critter Aura", GOSSIP_SENDER_MAIN, 1017);
                AddGossipItemFor(player, 4, "Kronk's Aura", GOSSIP_SENDER_MAIN, 1018);
                AddGossipItemFor(player, 4, "Ghoul Aura", GOSSIP_SENDER_MAIN, 1019);
                AddGossipItemFor(player, 4, "Healer Aura", GOSSIP_SENDER_MAIN, 1020);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
                break;
            case 998:
                //AddGossipItemFor(player, 4, "Blazerunner's Aura", GOSSIP_SENDER_MAIN, 1021);
                AddGossipItemFor(player, 4, "Void Portal Visual", GOSSIP_SENDER_MAIN, 1022);
                AddGossipItemFor(player, 4, "Fiery Aura", GOSSIP_SENDER_MAIN, 1023);
                AddGossipItemFor(player, 4, "Flameshocker - Immolate Visual", GOSSIP_SENDER_MAIN, 1024);
                AddGossipItemFor(player, 4, "Nightmare Portal Visual (Pre-effect)", GOSSIP_SENDER_MAIN, 1025);
                //AddGossipItemFor(player, 4, "Banish the Demons: Banishment Periodic Aura", GOSSIP_SENDER_MAIN, 1026);
                AddGossipItemFor(player, 4, "Blue Floating Rune Bunny: Rune Aura", GOSSIP_SENDER_MAIN, 1027);
                AddGossipItemFor(player, 4, "BOTM - Woodland Brew - Squirrel Love Aura", GOSSIP_SENDER_MAIN, 1028);
                AddGossipItemFor(player, 4, "Cosmetic - Purple Ghost Aura", GOSSIP_SENDER_MAIN, 1029);
                AddGossipItemFor(player, 4, "Cosmetic - Wintergrasp Victory Aura", GOSSIP_SENDER_MAIN, 1030);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
                break;
            case 999:
                AddGossipItemFor(player, 4, "Cosmetic - Flame Patch 0.5", GOSSIP_SENDER_MAIN, 1031);
                AddGossipItemFor(player, 4, "Cosmetic - Azure Manashaper Channel", GOSSIP_SENDER_MAIN, 1032);
                AddGossipItemFor(player, 4, "Cosmetic - Blue Flame Patch 0.5", GOSSIP_SENDER_MAIN, 1033);
                AddGossipItemFor(player, 4, "Cosmetic - Low Poly Blue Fire", GOSSIP_SENDER_MAIN, 1034);
                AddGossipItemFor(player, 4, "Cosmetic - Purple Glow - Bone Wastes", GOSSIP_SENDER_MAIN, 1035);
                AddGossipItemFor(player, 4, "Cosmetic - Ribbon Trail State Visual", GOSSIP_SENDER_MAIN, 1036);
                AddGossipItemFor(player, 4, "Alexstrasza's Gift Visual", GOSSIP_SENDER_MAIN, 1037);
                AddGossipItemFor(player, 4, "Draenei Spirit Visual 2", GOSSIP_SENDER_MAIN, 1038);
                AddGossipItemFor(player, 4, "Shadowmourne Visual High", GOSSIP_SENDER_MAIN, 1039);
                AddGossipItemFor(player, 4, "Arcane / Lightning Charge Power State", GOSSIP_SENDER_MAIN, 1040);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
                break;
            case 1000:
                AddGossipItemFor(player, 4, "Cosmetic - Fire Force Shield (Red)", GOSSIP_SENDER_MAIN, 1041);
                AddGossipItemFor(player, 4, "Dark Iron Light Aura, Red", GOSSIP_SENDER_MAIN, 1042);
                AddGossipItemFor(player, 4, "Headless Horseman - Pumpkin Aura", GOSSIP_SENDER_MAIN, 1043);
                //AddGossipItemFor(player, 4, "Visual 44", GOSSIP_SENDER_MAIN, 1044);
                AddGossipItemFor(player, 4, "Sanguine Spirit Pre-Aura2", GOSSIP_SENDER_MAIN, 1045);
                AddGossipItemFor(player, 4, "Cosmetic - Azure Manashaper Channel", GOSSIP_SENDER_MAIN, 1046);
                AddGossipItemFor(player, 4, "Pet Book Aura", GOSSIP_SENDER_MAIN, 1047);
                AddGossipItemFor(player, 4, "Scourge Bomb Aura", GOSSIP_SENDER_MAIN, 1048);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
                break;

                ///////////START ACTIONS//////////////////
            case 1001: //visual #1
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_1, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1002: //visual #2
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_2, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1003: //visual #3
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_3, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1004: //visual #4
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_4, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1005: //visual #5
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_5, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1006: //visual #6
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_6, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1007: //visual #7
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_7, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1008: //visual #8
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_8, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1009: //visual #9
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_9, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1010: //visual #10
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_10, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1011: //visual #1
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_11, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1012: //visual #2
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_12, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1013: //visual #3
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_13, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1014: //visual #4
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_14, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1015: //visual #5
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_15, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1016: //visual #6
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_16, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1017: //visual #7
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_17, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1018: //visual #8
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_18, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1019: //visual #9
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_19, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1020: //visual #10
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_20, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1021: //visual #1
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_21, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1022: //visual #2
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_22, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1023: //visual #3
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_23, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1024: //visual #4
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_24, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1025: //visual #5
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_25, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1026: //visual #6
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_26, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1027: //visual #7
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_27, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1028: //visual #8
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_28, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1029: //visual #9
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_29, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1030: //visual #10
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_30, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1031: //visual #1
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_31, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1032: //visual #2
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_32, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1033: //visual #3
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_33, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1034: //visual #4
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_34, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1035: //visual #5
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_35, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1036: //visual #6
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_36, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1037: //visual #7
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_37, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1038: //visual #8
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_38, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1039: //visual #9
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_39, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1040: //visual #10
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_40, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1041: //visual #1
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_41, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1042: //visual #2
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_42, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1043: //visual #3
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_43, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1044: //visual #4
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_44, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1045: //visual #5
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_45, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1046: //visual #6
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_46, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1047: //visual #7
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_47, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;

            case 1048: //visual #8
                if (player->HasItemCount(REQUIRED_ITEM, 5, true))
                {
                    player->DestroyItemCount(REQUIRED_ITEM, 5, true, false);
                    player->AddAura(VISUAL_48, player);
                    pCreature->Whisper("You got the visual you wanted.", LANG_UNIVERSAL, player);
                    GossipHello(player);
                }
                else
                {
                    pCreature->Whisper("You do not have enough [Visual Token] to continue.", LANG_UNIVERSAL, player), CloseGossipMenuFor(player);
                }
                break;  
            }
            return true;
        }   
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new VisualNPCAI(creature);
    }
};

void AddSC_VisualNPC()
{
	new VisualNPC();
}
