#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "DatabaseEnv.h"
#include "Player.h"
#include "Creature.h"
#include "ObjectMgr.h"
#include <cstring>
#include <stdio.h>
#include <time.h>

#define OFFSET_THEME 10000 

int GetLastThemeTime()
{
  	QueryResult result;
	  result = WorldDatabase.PQuery("SELECT `time` FROM `gurubashi_lastspawned`");

	  if (result)
	  {
	   Field *fields = result->Fetch();
	   return fields[0].GetInt32();
	  }
	  else 
		  return 0;

}

void GossipObjects(Player *player, Creature *m_creature)
{
   if (GetLastThemeTime() + 600 <= time (NULL))
    {
        QueryResult result;
    	  result = WorldDatabase.PQuery("SELECT `id`, `name` FROM `gurubashi_themes`");
        if (result)
        {
        	do
          {
          	Field *fields = result->Fetch();
            AddGossipItemFor(player ,4 , fields[1].GetString(), GOSSIP_SENDER_MAIN, OFFSET_THEME + fields[0].GetInt32());
          }
          while (result->NextRow());
        }
    }
    else
    {
        char msg[100];
      	int time2 = GetLastThemeTime() + 600 - time (NULL);
      	if (time2 < 60)
          sprintf(msg, "Next change possible in less than minute.");
        else
          sprintf(msg, "Next change possible in %u minute/s.", time2 / 60);		  
          AddGossipItemFor(player, 0, msg, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    }
    AddGossipItemFor(player, 0, "Good bye", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    SendGossipMenuFor(player, 1,m_creature->GetGUID());
}


class Theme_NPC : public CreatureScript
{
	public:
		Theme_NPC() : CreatureScript("Theme_NPC") {}

        struct Theme_NPC_gossipAI : public ScriptedAI
        {
            Theme_NPC_gossipAI(Creature* creature) : ScriptedAI(creature) { }

            bool GossipHello(Player* player) override
            {
                return OnGossipHello(player, me);
            }

            bool OnGossipHello(Player * pPlayer, Creature * pCreature)
            {
                GossipObjects(pPlayer, pCreature);
                return true;
            }

            bool GossipSelect(Player* player, uint32 /*menu_id*/, uint32 gossipListId) override
            {
                uint32 sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
                uint32 action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
                return OnGossipSelect(player, me, sender, action);
            }

            bool OnGossipSelect(Player *player, Creature * m_creature, uint32 sender, uint32 action)
            {
                if (action > OFFSET_THEME)
                {
                    QueryResult result;
                    result = WorldDatabase.PQuery("DELETE FROM `gurubashi_lastspawned`");
                    result = WorldDatabase.PQuery("INSERT INTO `gurubashi_lastspawned` VALUES (%u)", time(NULL));
                    result = WorldDatabase.PQuery("SELECT `x`, `y`, `z`, `o`, `entry` FROM `gurubashi_spawns` WHERE `theme` = %u", action - OFFSET_THEME);
                    if (result)
                    {
                        m_creature->Say("Spawning gameobjects..", LANG_UNIVERSAL, player);
                        do
                        {
                            Field *fields = result->Fetch();
                            QuaternionData rotation = QuaternionData::fromEulerAnglesZYX(fields[3].GetFloat(), 0.f, 0.f);
                            player->SummonGameObject(fields[4].GetInt32(), *player, rotation, 600);
                        } while (result->NextRow());
                    }
                    else
                    {
                        m_creature->Say("No gameobjects found.", LANG_UNIVERSAL, player);
                    }
                    player->PlayerTalkClass->SendCloseGossip();
                }
                else
                {
                    switch (action)
                    {
                    case GOSSIP_ACTION_INFO_DEF + 1:
                        player->PlayerTalkClass->SendCloseGossip();
                        break;
                    case GOSSIP_ACTION_INFO_DEF + 2:
                        GossipObjects(player, m_creature);
                        break;
                    }
                }
                return true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new Theme_NPC_gossipAI(creature);
        }
};


void AddSC_npc_gurubashi_theme()
{
	new Theme_NPC();
}
