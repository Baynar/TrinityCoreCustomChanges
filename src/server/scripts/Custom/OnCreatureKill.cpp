#include "ScriptPCH.h"
#include "Group.h"
#include "World.h"
#include "Player.h"
#include "WorldSession.h"
#include "Map.h"
#include <algorithm>

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

class dungeon_rewards : public PlayerScript
{
public:
    dungeon_rewards() : PlayerScript("dungeon_rewards") {}
 
   void OnCreatureKill(Player * killer, Creature* killed)
   {
       if (killed->GetEntry() == 214114) {
           Group* group = killer->GetGroup();
           if (!group) {
               killer->CastSpell(killer, 24752, true);
               killer->CastSpell(killer, 48074, true);
               killer->CastSpell(killer, 43223, true);
               killer->CastSpell(killer, 36880, true);

               killer->CastSpell(killer, 467, true);
               killer->CastSpell(killer, 48469, true);
               killer->CastSpell(killer, 48162, true);
               killer->CastSpell(killer, 48170, true);

               killer->CastSpell(killer, 16877, true);
               killer->CastSpell(killer, 10220, true);
               killer->CastSpell(killer, 13033, true);
               killer->CastSpell(killer, 11735, true);

               killer->CastSpell(killer, 10952, true);
               killer->CastSpell(killer, 23948, true);
               killer->CastSpell(killer, 47440, true);
               killer->CastSpell(killer, 53307, true);

               killer->CastSpell(killer, 132, true);
               killer->CastSpell(killer, 48470, true);
               killer->CastSpell(killer, 43002, true);
               killer->CastSpell(killer, 69994, true);
               return;
           }
           for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
               if (Player* player = itr->GetSource())
                   if (player->IsAtGroupRewardDistance(killer)) {
                       player->CastSpell(player, 24752, true);
                       player->CastSpell(player, 48074, true);
                       player->CastSpell(player, 43223, true);
                       player->CastSpell(player, 36880, true);

                       player->CastSpell(player, 467, true);
                       player->CastSpell(player, 48469, true);
                       player->CastSpell(player, 48162, true);
                       player->CastSpell(player, 48170, true);

                       player->CastSpell(player, 16877, true);
                       player->CastSpell(player, 10220, true);
                       player->CastSpell(player, 13033, true);
                       player->CastSpell(player, 11735, true);

                       player->CastSpell(player, 10952, true);
                       player->CastSpell(player, 23948, true);
                       player->CastSpell(player, 47440, true);
                       player->CastSpell(player, 53307, true);

                       player->CastSpell(player, 132, true);
                       player->CastSpell(player, 48470, true);
                       player->CastSpell(player, 43002, true);
                       player->CastSpell(player, 69994, true);
                   }
       }

       if (killed->GetEntry() == 215115) {
           SessionMap sessions = sWorld->GetAllSessions();
           for (SessionMap::iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
           {
               if (!itr->second)
                   continue;
               if (Player *player = itr->second->GetPlayer())
               {
                   if (!player->IsInWorld())
                       continue;

                   player->CastSpell(player, 24752, true);
                   player->CastSpell(player, 48074, true);
                   player->CastSpell(player, 43223, true);
                   player->CastSpell(player, 36880, true);

                   player->CastSpell(player, 467, true);
                   player->CastSpell(player, 48469, true);
                   player->CastSpell(player, 48162, true);
                   player->CastSpell(player, 48170, true);

                   player->CastSpell(player, 16877, true);
                   player->CastSpell(player, 10220, true);
                   player->CastSpell(player, 13033, true);
                   player->CastSpell(player, 11735, true);

                   player->CastSpell(player, 10952, true);
                   player->CastSpell(player, 23948, true);
                   player->CastSpell(player, 47440, true);
                   player->CastSpell(player, 53307, true);

                   player->CastSpell(player, 132, true);
                   player->CastSpell(player, 48470, true);
                   player->CastSpell(player, 43002, true);
                   player->CastSpell(player, 69994, true);

                   player->CastSpell(player, 35912, true);
                   player->CastSpell(player, 35874, true);
                   player->CastSpell(player, 9175, true);
                   player->CastSpell(player, 38734, true);

                   player->CastSpell(player, 26393, true);
                   player->CastSpell(player, 16609, true);
                   player->CastSpell(player, 34410, true);
                   player->CastSpell(player, 24705, true);

                   player->CastSpell(player, 23737, true);
                   player->CastSpell(player, 23738, true);
                   player->CastSpell(player, 23766, true);
                   player->CastSpell(player, 23767, true);

                   player->CastSpell(player, 23768, true);
                   player->CastSpell(player, 23769, true);
                   player->CastSpell(player, 23735, true);
                   player->CastSpell(player, 23736, true);
               }
           }
       }

       int VanillaDungeonBosses[] = { 61408, 61412, 61463, 61528, 47162, 47296, 43778, 47626, 47739, 49541, 3671, 3669, 3653,
            3670, 3674, 3673, 5775, 3654, 46962, 3887, 4278, 46963, 46964, 74458, 4831, 6243,
            12902, 12876, 4832, 4830, 75155, 46383, 46264, 46254, 6231, 7079, 7361, 6235, 6229,
            7800, 6168, 4424, 4428, 4420, 4422, 4421, 13282, 12258, 12236, 12225, 12203, 13601,
            13596, 12201, 7355, 7357, 8567, 7358, 6910, 6906, 7228, 7023, 7206, 7291, 4854, 2748,
            6907, 6908, 11490, 13280,14327, 11492, 14326, 14322, 14321, 14323, 14325, 11501, 14324,
            11489, 11488, 11487, 11496, 11486, 8127, 7272, 7271, 7796, 7795, 7273, 7275, 7797, 7267,
            10516, 10558, 10808, 11032, 10997, 10811, 10813, 10436, 10437, 10438, 10435, 10439, 45412,
            5710, 8443, 5709,
            644 };
       int TbcDungeonBosses[] = { 17306, 17308, 17536, 17537, 17381, 17380, 17377, 17941, 17991, 17942, 17770, 18105, 17826,
            17882, 18341, 18343, 18344, 22930, 18371, 18373, 17848, 17862, 18096, 18472, 18473, 23035,
            17879, 17880, 17881, 18731, 18667, 18732, 18708, 17976, 17975, 17978, 17980, 17977, 19219,
            19218, 19710, 19221, 19220, 16807, 20923, 16809, 16808, 17797, 17796, 17798, 24723, 24744,
            24560, 24557, 24558, 24554, 24561, 24559, 24555, 24553, 24556, 24664, 20870, 20885, 20886, 20912 };
       int WotlkDungeonBosses[] = { 23953, 24200, 24201, 23954, 26731, 26763, 26794, 26723, 28684, 28921, 29120, 29309, 29308,
            29310, 29311, 30258, 26630, 26631, 27483, 26632, 29315, 29316, 29313, 29266, 29312, 29314,
            31134, 29304, 29307, 29573, 29305, 29306, 29932, 27975, 27977, 28234, 27978, 28586, 28587,
            28546, 28923, 27654, 27447, 27655, 27656, 26668, 26687, 26693, 26861, 26529, 26530, 26532,
            26533, 32273, 35119, 34928, 35451, 38112, 38113, 37226, 36494, 36476, 36477, 36658, 36497, 36502 };
       int VanillaRaidBosses[] = { 12118, 11982, 12259, 12057, 12264, 12056, 11988, 12098, 12018, 11502, 15348, 15341, 15391,
            15392, 15389, 15390, 15386, 15388, 15385, 15340, 15370, 15369, 15339, 15263, 15516, 15510,
            15509, 15276, 15275, 15727, 15543, 15544, 15511, 15299, 15517 };
       int TbcRaidBosses[] = { 16181, 16152, 15687, 16457, 16812, 15691, 15688, 16524, 15689, 17225, 15690, 18831, 19044,
            17257, 21216, 21217, 21215, 21214, 21213, 21212, 24850, 24892, 24882, 25038, 25166, 25165,
            25741, 25840, 25315, 19516, 19514, 18805, 19622, 17767, 17808, 17888, 17842, 17968, 22887,
            22898, 22841, 22871, 22948, 22856, 22947, 23426, 22949, 22950, 22951, 22952, 22917 };
       int WotlkRaidBosses[] = { 31125, 33993, 35013, 38433, 10184, 36612, 36855, 37813, 36626, 36627, 36678, 37970, 37955,
            36789, 36853, 36597, 15956, 15953, 15952, 15954, 15936, 16011, 16061, 16060, 16028, 15931,
            15932, 15928, 15989, 15990, 30452, 30451, 30449, 28860, 28859, 33113, 33118, 33186, 33293,
            32857, 32930, 33515, 32906, 32845, 33350, 32865, 33271, 33288, 32871, 34797, 34780, 34497,
            34496, 34564, 39751, 39746, 39747, 39863 };
       size_t VanillaDungeonSize = sizeof(VanillaDungeonBosses) / sizeof(int);
       int *endVD = VanillaDungeonBosses + VanillaDungeonSize;
       size_t TbcDungeonSize = sizeof(TbcDungeonBosses) / sizeof(int);
       int *endTD = TbcDungeonBosses + TbcDungeonSize;
       size_t WotlkDungeonSize = sizeof(WotlkDungeonBosses) / sizeof(int);
       int *endWD = WotlkDungeonBosses + WotlkDungeonSize;
       size_t VanillaRaidSize = sizeof(VanillaRaidBosses) / sizeof(int);
       int *endVR = VanillaRaidBosses + VanillaRaidSize;
       size_t TbcRaidSize = sizeof(TbcRaidBosses) / sizeof(int);
       int *endTR = TbcRaidBosses + TbcRaidSize;
       size_t WotlkRaidSize = sizeof(WotlkRaidBosses) / sizeof(int);
       int *endWR = WotlkRaidBosses + WotlkRaidSize;

       int *resultVD = std::find(VanillaDungeonBosses, endVD, killed->GetEntry());
       if (resultVD != endVD) {
           Group* group = killer->GetGroup();
           if (!group) {
               killer->CastSpell(killer, 11540, true);
               killer->CastSpell(killer, 11541, true);
               killer->CastSpell(killer, 6668, true);
               killer->CastSpell(killer, 11544, true);
               return;
           }
               
           for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
               if (Player* player = itr->GetSource())
                   if (player->IsAtGroupRewardDistance(killer)) {
                       player->CastSpell(player, 11540, true);
                       player->CastSpell(player, 11541, true);
                       player->CastSpell(player, 6668, true);
                       player->CastSpell(player, 11544, true);
                   }
       }
       int *resultTD = std::find(TbcDungeonBosses, endTD, killed->GetEntry());
       if (resultTD != endTD) {
           Group* group = killer->GetGroup();
           if (!group) {
               killer->CastSpell(killer, 11540, true);
               killer->CastSpell(killer, 11541, true);
               killer->CastSpell(killer, 6668, true);
               killer->CastSpell(killer, 11544, true);
               return;
           }
           for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
               if (Player* player = itr->GetSource())
                   if (player->IsAtGroupRewardDistance(killer)) {
                       player->CastSpell(player, 11540, true);
                       player->CastSpell(player, 11541, true);
                       player->CastSpell(player, 6668, true);
                       player->CastSpell(player, 11544, true);
                   }
       }
       int *resultWD = std::find(WotlkDungeonBosses, endWD, killed->GetEntry());
       if (resultWD != endWD) {
           Group* group = killer->GetGroup();
           if (!group) {
               killer->CastSpell(killer, 11540, true);
               killer->CastSpell(killer, 11541, true);
               killer->CastSpell(killer, 6668, true);
               killer->CastSpell(killer, 11544, true);
               return;
           }
           for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
               if (Player* player = itr->GetSource())
                   if (player->IsAtGroupRewardDistance(killer)) {
                       player->CastSpell(player, 11540, true);
                       player->CastSpell(player, 11541, true);
                       player->CastSpell(player, 6668, true);
                       player->CastSpell(player, 11544, true);
                   }
       }
       int *resultVR = std::find(VanillaRaidBosses, endVR, killed->GetEntry());
       if (resultVR != endVR) {
           Group* group = killer->GetGroup();
           if (!group) {
               killer->CastSpell(killer, 11540, true);
               killer->CastSpell(killer, 11541, true);
               killer->CastSpell(killer, 6668, true);
               killer->CastSpell(killer, 11544, true);
               return;
           }
           for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
               if (Player* player = itr->GetSource())
                   if (player->IsAtGroupRewardDistance(killer)) {
                       player->CastSpell(player, 11540, true);
                       player->CastSpell(player, 11541, true);
                       player->CastSpell(player, 6668, true);
                       player->CastSpell(player, 11544, true);
                   }
       }
       int *resultTR = std::find(TbcRaidBosses, endTR, killed->GetEntry());
       if (resultTR != endTR) {
           Group* group = killer->GetGroup();
           if (!group) {
               killer->CastSpell(killer, 11540, true);
               killer->CastSpell(killer, 11541, true);
               killer->CastSpell(killer, 6668, true);
               killer->CastSpell(killer, 11544, true);
               return;
           }
           for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
               if (Player* player = itr->GetSource())
                   if (player->IsAtGroupRewardDistance(killer)) {
                       player->CastSpell(player, 11540, true);
                       player->CastSpell(player, 11541, true);
                       player->CastSpell(player, 6668, true);
                       player->CastSpell(player, 11544, true);
                   }
       }
       int *resultWR = std::find(WotlkRaidBosses, endWR, killed->GetEntry());
       if (resultWR != endWR) {
           Group* group = killer->GetGroup();
           if (!group) {
               killer->CastSpell(killer, 11540, true);
               killer->CastSpell(killer, 11541, true);
               killer->CastSpell(killer, 6668, true);
               killer->CastSpell(killer, 11544, true);
               return;
           }
           for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
               if (Player* player = itr->GetSource())
                   if (player->IsAtGroupRewardDistance(killer)) {
                       player->CastSpell(player, 11540, true);
                       player->CastSpell(player, 11541, true);
                       player->CastSpell(player, 6668, true);
                       player->CastSpell(player, 11544, true);
                   }
       }
   }
};
 
void AddSC_dungeon_rewards()
{
   new dungeon_rewards();
}
