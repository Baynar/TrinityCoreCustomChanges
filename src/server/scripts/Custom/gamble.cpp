//CastleDEV
#include "ScriptedGossip.h"
#include "Player.h"
#include "Chat.h"

#define ARMOR_PART 60013
#define WEAPON_PART 60014
#define SOULS_COPON 60028      //illusion dust
#define UNHOLI_COPON 60034   //pheonix pheather
 
class gamble_npc : public CreatureScript
{
public:
    gamble_npc() : CreatureScript("gamble_npc") { }

    class gamble_npcAI : public ScriptedAI
    {
    public:
        gamble_npcAI(Creature* creature) : ScriptedAI(creature) { }

        bool GossipHello(Player* player) override
        {
            return OnGossipHello(player, me);
            return true;
        }

        bool OnGossipHello(Player * player, Creature * creature)
        {
            AddGossipItemFor(player, 3, "|cffFF0000I want to play with |CFFFFCC99Armor Part!", GOSSIP_SENDER_MAIN, ARMOR_PART, "Insert amount of Armor Part", 0, true);
            AddGossipItemFor(player, 3, "|cffFF0000I want to play with |CFFFFCC99Weapon Part!", GOSSIP_SENDER_MAIN, WEAPON_PART, "Insert amount of Weapon Part", 0, true);
            AddGossipItemFor(player, 3, "|cffFF0000I want to play with |cff6699FFIllusion Dust!", GOSSIP_SENDER_MAIN, SOULS_COPON, "Insert amount of Illusion Dust", 0, true);
            AddGossipItemFor(player, 3, "|cffFF0000I want to play with |cffFF0000Phoenix Feather!", GOSSIP_SENDER_MAIN, UNHOLI_COPON, "Insert amount of Phoenix Feather", 0, true);
            AddGossipItemFor(player, 3, "The game works as following. you add an amount of the chosen currency and you can multiplay it!", GOSSIP_SENDER_MAIN, 0);

            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            return true;
        }

        bool GossipSelect(Player* player, uint32 /*menu_id*/, uint32 gossipListId) override
        {
            uint32 sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
            uint32 action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            return OnGossipSelect(player, me, sender, action);
        }

        bool OnGossipSelect(Player * player, Creature * creature, uint32 sender, uint32 uiAction)
        {
            player->PlayerTalkClass->ClearMenus();

            if (uiAction == 0)
            {
                AddGossipItemFor(player, 7, "Back..", GOSSIP_SENDER_MAIN, 1);
                SendGossipMenuFor(player, 123432, creature->GetGUID());
            }
            else if (uiAction == 1)
            {
                OnGossipHello(player, creature);
            }
            else if (uiAction == 2)
            {
                CloseGossipMenuFor(player);
            }
            return true;
        }

        bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 uiAction, const char* code)
        {
            player->PlayerTalkClass->ClearMenus();


            uint32 amount = 0;
            amount = uint32(atol(code));

            if (amount < 1 || !player->HasItemCount(uiAction, amount, false))
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Invalid amount inserted.");
            }
            else
            {
                if (urand(1, 2) == 1)
                {
                    GiveItemAmount(player, uiAction, amount);
                    ChatHandler(player->GetSession()).PSendSysMessage("You are lucky. You have won!");
                }
                else
                {
                    player->DestroyItemCount(uiAction, amount, true);
                    ChatHandler(player->GetSession()).PSendSysMessage("You have lost.. Bet again and u might win!!");
                }
            }
            OnGossipSelect(player, creature, sender, 0);
            return true;
        }


        bool GiveItemAmount(Player * player, uint32 itemEntry, uint32 amount)
        {
            if (amount < 0)
            {
                player->DestroyItemCount(itemEntry, -amount, true, false);
                return true;
            }
            if (amount == 0)
                amount = 1;

            uint32 noSpaceForCount = 0;

            ItemPosCountVec dest;
            uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemEntry, amount, &noSpaceForCount);

            if (msg != EQUIP_ERR_OK)
                amount -= noSpaceForCount;

            if (amount == 0 || dest.empty())
                return false;

            Item* item = player->StoreNewItem(dest, itemEntry, true);
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new gamble_npcAI(creature);
    }
};
 
void AddSC_gamble_npc()
{
    new gamble_npc();
}
