/*Made by: ???
  Updated by: Erictemponi
*/

#include "ScriptMgr.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GameEventMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Unit.h"
#include "GameObject.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "InstanceScript.h"
#include "CombatAI.h"
#include "PassiveAI.h"
#include "Chat.h"
#include "World.h"
#include "DBCStructure.h"
#include "DBCStores.h"
#include "ObjectMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Language.h"
#include "SpellMgr.h"
#include "Log.h"

class Professions_NPC : public CreatureScript
{
public:
	Professions_NPC() : CreatureScript("Professions_NPC") { }

	class ProffAI : public ScriptedAI
	{
	public:
		ProffAI(Creature* creature) : ScriptedAI(creature) {}


		void CreatureWhisperBasedOnBool(const char *text, Creature *creature, Player *player, bool value)
		{
			if (value)
				creature->TextEmote(text, player);
		}

		uint32 PlayerMaxLevel() const
		{
			return sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
		}

		bool PlayerHasItemOrSpell(const Player *plr, uint32 itemId, uint32 spellId) const
		{
			return plr->HasItemCount(itemId, 1, true) || plr->HasSpell(spellId);
		}

		bool GossipHello(Player* player) override
		{
			return OnGossipHello(player, me);
		}

		bool OnGossipHello(Player *player, Creature* creature)
		{
			ClearGossipMenuFor(player);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "|TInterface\\icons\\trade_alchemy:30|t Alchemy.", GOSSIP_SENDER_MAIN, 1);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "|TInterface\\icons\\INV_Ingot_05:30|t Blacksmithing.", GOSSIP_SENDER_MAIN, 2);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "|TInterface\\icons\\INV_Misc_LeatherScrap_02:30|t Leatherworking.", GOSSIP_SENDER_MAIN, 3);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "|TInterface\\icons\\INV_Fabric_Felcloth_Ebon:30|t Tailoring.", GOSSIP_SENDER_MAIN, 4);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "|TInterface\\icons\\inv_misc_wrench_01:30|t Engineering.", GOSSIP_SENDER_MAIN, 5);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "|TInterface\\icons\\trade_engraving:30|t Enchanting.", GOSSIP_SENDER_MAIN, 6);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "|TInterface\\icons\\inv_misc_gem_01:30|t Jewelcrafting.", GOSSIP_SENDER_MAIN, 7);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "|TInterface\\icons\\INV_Scroll_08:30|t Inscription.", GOSSIP_SENDER_MAIN, 8);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "|TInterface\\icons\\INV_Misc_Herb_07:30|t Herbalism.", GOSSIP_SENDER_MAIN, 9);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "|TInterface\\icons\\inv_misc_pelt_wolf_01:30|t Skinning.", GOSSIP_SENDER_MAIN, 10);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "|TInterface\\icons\\trade_mining:30|t Mining.", GOSSIP_SENDER_MAIN, 11);
			AddGossipItemFor(player, GOSSIP_ICON_TALK, "|TInterface/ICONS/Thrown_1H_Harpoon_D_01Blue:30|t Nevermind!", GOSSIP_SENDER_MAIN, 12);
			SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
		}

		bool PlayerAlreadyHasTwoProfessions(const Player *player) const
		{
			uint32 skillCount = 0;

			if (player->HasSkill(SKILL_MINING))
				skillCount++;
			if (player->HasSkill(SKILL_SKINNING))
				skillCount++;
			if (player->HasSkill(SKILL_HERBALISM))
				skillCount++;

			if (skillCount >= 2)
				return true;

			for (uint32 i = 1; i < sSkillLineStore.GetNumRows(); ++i)
			{
				SkillLineEntry const *SkillInfo = sSkillLineStore.LookupEntry(i);
				if (!SkillInfo)
					continue;

				if (SkillInfo->categoryId == SKILL_CATEGORY_SECONDARY)
					continue;

				if ((SkillInfo->categoryId != SKILL_CATEGORY_PROFESSION) || !SkillInfo->canLink)
					continue;

				const uint32 skillID = SkillInfo->id;
				if (player->HasSkill(skillID))
					skillCount++;

				if (skillCount >= 2)
					return true;
			}
			return false;
		}

		bool LearnAllRecipesInProfession(Player *player, SkillType skill)
		{
			ChatHandler handler(player->GetSession());
			char* skill_name;

			SkillLineEntry const *SkillInfo = sSkillLineStore.LookupEntry(skill);
			skill_name = SkillInfo->name[handler.GetSessionDbcLocale()];

			if (!SkillInfo)
			{
				TC_LOG_ERROR("server.loading", "Profession NPC: received non-valid skill ID (LearnAllRecipesInProfession)");
			}

			LearnSkillRecipesHelper(player, SkillInfo->id);

			player->SetSkill(SkillInfo->id, player->GetSkillStep(SkillInfo->id), 450, 450);
			handler.PSendSysMessage(LANG_COMMAND_LEARN_ALL_RECIPES, skill_name);

			return true;
		}

		void LearnSkillRecipesHelper(Player *player, uint32 skill_id)
		{
			uint32 classmask = player->getClassMask();

			for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
			{
				SkillLineAbilityEntry const *skillLine = sSkillLineAbilityStore.LookupEntry(j);
				if (!skillLine)
					continue;

				// wrong skill
				if (skillLine->skillId != skill_id)
					continue;

				// not high rank
				if (skillLine->forward_spellid)
					continue;

				// skip racial skills
				if (skillLine->racemask != 0)
					continue;

				// skip wrong class skills
				if (skillLine->classmask && (skillLine->classmask & classmask) == 0)
					continue;

				SpellInfo const * spellInfo = sSpellMgr->GetSpellInfo(skillLine->spellId);
				if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, player, false))
					continue;

				player->LearnSpell(skillLine->spellId, false);
				
			}
		}

		bool IsSecondarySkill(SkillType skill) const
		{
			return skill == SKILL_COOKING || skill == SKILL_FIRST_AID;
		}

		void CompleteLearnProfession(Player *player, Creature *creature, SkillType skill)
		{
			if (PlayerAlreadyHasTwoProfessions(player) && !IsSecondarySkill(skill))
				creature->TextEmote("You already know two professions!", player);
			else
			{
				if (!LearnAllRecipesInProfession(player, skill))
					creature->TextEmote("Internal error occured!", player);
			}
		}


		bool GossipSelect(Player* player, uint32 /*menu_id*/, uint32 gossipListId) override
		{
			uint32 sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
			uint32 action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
			return OnGossipSelect(player, me, sender, action);
		}

		bool OnGossipSelect(Player* player, Creature* creature, uint32 uiSender, uint32 uiAction)
		{
			ClearGossipMenuFor(player);
			if (uiSender == GOSSIP_SENDER_MAIN)
			{

				switch (uiAction)
				{
				case 196:
				
					
					break;
				case 1:
					if (player->HasSkill(SKILL_ALCHEMY))
					{
						CloseGossipMenuFor(player);
						break;
					}

					CompleteLearnProfession(player, creature, SKILL_ALCHEMY);

					CloseGossipMenuFor(player);
					break;
				case 2:
					if (player->HasSkill(SKILL_BLACKSMITHING))
					{
						CloseGossipMenuFor(player);
						break;
					}
					CompleteLearnProfession(player, creature, SKILL_BLACKSMITHING);

					CloseGossipMenuFor(player);
					break;
				case 3:
					if (player->HasSkill(SKILL_LEATHERWORKING))
					{
						CloseGossipMenuFor(player);
						break;
					}
					CompleteLearnProfession(player, creature, SKILL_LEATHERWORKING);

					CloseGossipMenuFor(player);
					break;
				case 4:
					if (player->HasSkill(SKILL_TAILORING))
					{
						CloseGossipMenuFor(player);
						break;
					}
					CompleteLearnProfession(player, creature, SKILL_TAILORING);

					CloseGossipMenuFor(player);
					break;
				case 5:
					if (player->HasSkill(SKILL_ENGINEERING))
					{
						CloseGossipMenuFor(player);
						break;
					}
					CompleteLearnProfession(player, creature, SKILL_ENGINEERING);

					CloseGossipMenuFor(player);
					break;
				case 6:
					if (player->HasSkill(SKILL_ENCHANTING))
					{
						CloseGossipMenuFor(player);
						break;
					}
					CompleteLearnProfession(player, creature, SKILL_ENCHANTING);

					CloseGossipMenuFor(player);
					break;
				case 7:
					if (player->HasSkill(SKILL_JEWELCRAFTING))
					{
						CloseGossipMenuFor(player);
						break;
					}
					CompleteLearnProfession(player, creature, SKILL_JEWELCRAFTING);

					CloseGossipMenuFor(player);
					break;
				case 8:
					if (player->HasSkill(SKILL_INSCRIPTION))
					{
						CloseGossipMenuFor(player);
						break;
					}
					CompleteLearnProfession(player, creature, SKILL_INSCRIPTION);

					CloseGossipMenuFor(player);
					break;
				case 9:
					if (player->HasSkill(SKILL_HERBALISM))
					{
						CloseGossipMenuFor(player);
						break;
					}

					CompleteLearnProfession(player, creature, SKILL_HERBALISM);
					CloseGossipMenuFor(player);
					break;
				case 10:
					if (player->HasSkill(SKILL_SKINNING))
					{
						CloseGossipMenuFor(player);
						break;
					}

					CompleteLearnProfession(player, creature, SKILL_SKINNING);
					CloseGossipMenuFor(player);
					break;
				case 11:
					if (player->HasSkill(SKILL_MINING))
					{
						CloseGossipMenuFor(player);
						break;
					}

					CompleteLearnProfession(player, creature, SKILL_MINING);
					CloseGossipMenuFor(player);
					break;
				case 12:
					CloseGossipMenuFor(player);
					break;
				}


			}
			return true;
		}
	};
	CreatureAI* GetAI(Creature* creature) const override
	{
		return  new ProffAI(creature);
	}
};

void AddSC_Professions_NPC()
{
	new Professions_NPC();
}
