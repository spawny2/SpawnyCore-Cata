/* Copyright (C) 2010 Easy for TrinityCore <http://trinity-core.ru/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* SQL
 REPLACE INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction_A`, `faction_H`, `npcflag`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `Health_mod`, `Mana_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `equipment_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
 (100002, 0, 0, 0, 0, 0, 27153, 0, 0, 0, 'Change', 'Change Service', '', 0, 80, 80, 0, 35, 35, 1, 0.75, 2, 1755, 1755, 0, 1504, 1000, 1500, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 'npc_change');
 */

#include "ScriptPCH.h"
#include "Creature.h"
#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "Bag.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Item.h"
#include "UpdateData.h"
#include "ObjectAccessor.h"

#define JusticeGossip 					2
#define ConquestGossip 					3
#define JCGossip 						4
#define CookGossip 						5
#define MSG_GOSSIP_TEXT_CHANGE          "Exchange 1x Mark of Remulos for 15x Justice Points."
#define MSG_GOSSIP_TEXT_CHANGE_2        "Exchange 1x Mark of Cenarius for 1 Tol Barad Commendation Point."
#define MSG_GOSSIP_TEXT_CHANGE_3		"Exchange 1x Necrotic Rune for 1 Illustrious Jewelcrafter Point."
#define MSG_GOSSIP_TEXT_CHANGE_4		"Exchange 1x Gri'lek's Blood for 1 Chef's Award Point"
#define MSG_LOW_LEVEL                   "Your level is too low!"
#define MSG_NOT_ENOUGH                  "You don't have any items left."
#define CHANGE_ITEM_1             		21515
#define CHANGE_ITEM_2              	 	21508
#define CHANGE_ITEM_3              		22484
#define CHANGE_ITEM_4              		19939
#define JUSTICE							1500
#define TOLBARAD						100
#define JC								100
#define Cook							100

class npc_change: public CreatureScript {
public:
	npc_change() :
		CreatureScript("npc_change") {
	}

	bool OnGossipHello(Player* pPlayer, Creature* pCreature) {
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, MSG_GOSSIP_TEXT_CHANGE, GOSSIP_SENDER_MAIN, JusticeGossip);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, MSG_GOSSIP_TEXT_CHANGE_2, GOSSIP_SENDER_MAIN, ConquestGossip);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, MSG_GOSSIP_TEXT_CHANGE_3, GOSSIP_SENDER_MAIN, JCGossip);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, MSG_GOSSIP_TEXT_CHANGE_4, GOSSIP_SENDER_MAIN, CookGossip);
		pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 uiSender,
			uint32 uiAction) {
		if (uiSender != GOSSIP_SENDER_MAIN)
			return false;

		pPlayer->PlayerTalkClass->ClearMenus();

		switch (uiAction) {
		case 1:
			if (pPlayer->getLevel() < 70) {
				pCreature->MonsterWhisper(MSG_LOW_LEVEL, pPlayer->GetGUID());
				pPlayer->CLOSE_GOSSIP_MENU();
				break;
			}
		case JusticeGossip: {
			if (pPlayer->GetItemByEntry(CHANGE_ITEM_1)) {
				uint32 count = 1;
				pPlayer->DestroyItemCount(CHANGE_ITEM_1, count, true, true);
				pPlayer->ModifyCurrency(395, (JUSTICE));
				pCreature->MonsterWhisper(
						"Congratz, you've been rewarded with 15 Justice Points.",
						pPlayer->GetGUID());
				pPlayer->CLOSE_GOSSIP_MENU();
				break;
			}
			pCreature->MonsterWhisper(MSG_NOT_ENOUGH, pPlayer->GetGUID());
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		case ConquestGossip: {
			if (pPlayer->GetItemByEntry(CHANGE_ITEM_2)) {
				uint32 count = 1;
				pPlayer->DestroyItemCount(CHANGE_ITEM_2, count, true, true);
				pPlayer->ModifyCurrency(391, (TOLBARAD));
				pCreature->MonsterWhisper(
						"Congratz, you've been rewarded with 1 Tol Barad Commendation Point.",
						pPlayer->GetGUID());
				pPlayer->CLOSE_GOSSIP_MENU();
				break;
			}
			pCreature->MonsterWhisper(MSG_NOT_ENOUGH, pPlayer->GetGUID());
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		case JCGossip: {
			if (pPlayer->GetItemByEntry(CHANGE_ITEM_3)) {
				uint32 count = 1;
				pPlayer->DestroyItemCount(CHANGE_ITEM_3, count, true, true);
				pPlayer->ModifyCurrency(361, (JC));
				pCreature->MonsterWhisper(
						"Congratz, you've been rewarded with 1 Illustrious Jewelcrafter's Point.",
						pPlayer->GetGUID());
				pPlayer->CLOSE_GOSSIP_MENU();
				break;
			}
			pCreature->MonsterWhisper(MSG_NOT_ENOUGH, pPlayer->GetGUID());
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		case CookGossip: {
			if (pPlayer->GetItemByEntry(CHANGE_ITEM_4)) {
				uint32 count = 1;
				pPlayer->DestroyItemCount(CHANGE_ITEM_4, count, true, true);
				pPlayer->ModifyCurrency(402, (Cook));
				pCreature->MonsterWhisper(
						"Congratz, you've been rewarded with 1 Chef's Award Point",
						pPlayer->GetGUID());
				pPlayer->CLOSE_GOSSIP_MENU();
				break;
			}
			pCreature->MonsterWhisper(MSG_NOT_ENOUGH, pPlayer->GetGUID());
			pPlayer->CLOSE_GOSSIP_MENU();
		}
			pCreature->MonsterWhisper(MSG_NOT_ENOUGH, pPlayer->GetGUID());
		default:
			pPlayer->CLOSE_GOSSIP_MENU();
		}
		return true;
	}
};

void AddSC_npc_change() {
	new npc_change;
}
