#include "ScriptedGossip.h"
#include "Player.h"
#include "Chat.h"

#define DT_VANILLA 200001
#define DT_TBC 200002
#define DT_WOTLK 200003    
#define RT_VANILLA 200004  
#define RT_TBC 200005
#define RT_WOTLK 200006
#define VISUAL_TOKEN 200019    
 
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
            AddGossipItemFor(player, 3, "|cffFF0000I want to play with |CFFFFCC99Dungeon Token Vanilla!", GOSSIP_SENDER_MAIN, DT_VANILLA, "Insert amount of items to gamble.", 0, true);
            AddGossipItemFor(player, 3, "|cffFF0000I want to play with |CFFFFCC99Dungeon Token Tbc!", GOSSIP_SENDER_MAIN, DT_TBC, "Insert amount of items to gamble.", 0, true);
            AddGossipItemFor(player, 3, "|cffFF0000I want to play with |cff6699FFDungeon Token Wotlk!", GOSSIP_SENDER_MAIN, DT_WOTLK, "Insert amount of items to gamble.", 0, true);
            AddGossipItemFor(player, 3, "|cffFF0000I want to play with |cffFF0000Raid Token Vanilla!", GOSSIP_SENDER_MAIN, RT_VANILLA, "Insert amount of items to gamble.", 0, true);
            AddGossipItemFor(player, 3, "|cffFF0000I want to play with |CFFFFCC99Raid Token Tbc!", GOSSIP_SENDER_MAIN, RT_TBC, "Insert amount of items to gamble.", 0, true);
            AddGossipItemFor(player, 3, "|cffFF0000I want to play with |cff6699FFRaid Token Wotlk!", GOSSIP_SENDER_MAIN, RT_WOTLK, "Insert amount of items to gamble.", 0, true);
            AddGossipItemFor(player, 3, "|cffFF0000I want to play with |cffFF0000Visual Token!", GOSSIP_SENDER_MAIN, VISUAL_TOKEN, "Insert amount of items to gamble.", 0, true);
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
                OnGossipHello(player, creature);
            }
            else
            {
                OnGossipSelectCode(player, creature, sender, uiAction, 0);
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
            OnGossipHello(player, creature);
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
