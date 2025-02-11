/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
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

/* ScriptData
SDName: Instance_Molten_Core
SD%Complete: 100
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "molten_core.h"

static sSpawnLocation m_aBosspawnLocsElite[MAX_MAJORDOMO_ADDS] =
{
    {NPC_FLAMEWAKER_ELITE,  737.945f, -1156.48f, -118.945f, 4.46804f},
    {NPC_FLAMEWAKER_ELITE,  752.520f, -1191.02f, -118.218f, 2.49582f},
    {NPC_FLAMEWAKER_ELITE,  752.953f, -1163.94f, -118.869f, 3.70010f},
    {NPC_FLAMEWAKER_ELITE,  738.814f, -1197.40f, -118.018f, 1.83260f}
};

static sSpawnLocation m_aBosspawnLocsHealer[MAX_MAJORDOMO_ADDS] =
{
    {NPC_FLAMEWAKER_HEALER, 746.939f, -1194.87f, -118.016f, 2.21657f},
    {NPC_FLAMEWAKER_HEALER, 747.132f, -1158.87f, -118.897f, 4.03171f},
    {NPC_FLAMEWAKER_HEALER, 757.116f, -1170.12f, -118.793f, 3.40339f},
    {NPC_FLAMEWAKER_HEALER, 755.910f, -1184.46f, -118.449f, 2.80998f}
};

instance_molten_core::instance_molten_core(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
}

void instance_molten_core::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

bool instance_molten_core::IsEncounterInProgress() const
{
    for (uint32 i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            return true;
    }

    return false;
}

void instance_molten_core::OnPlayerEnter(Player* /*pPlayer*/)
{
    // Summon Majordomo if can
    DoSpawnMajordomoIfCan(true);
}

void instance_molten_core::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        // Bosses
        case NPC_GARR:
        case NPC_SULFURON:
        case NPC_MAJORDOMO:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
    }
}

void instance_molten_core::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        // Runes
        case GO_RUNE_KRESS:
        case GO_RUNE_MOHN:
        case GO_RUNE_BLAZ:
        case GO_RUNE_MAZJ:
        case GO_RUNE_ZETH:
        case GO_RUNE_THERI:
        case GO_RUNE_KORO:
            // Activate the rune if it was previously doused by a player (encounter set to SPECIAL)
            m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            for (auto m_aMoltenCoreRune : m_aMoltenCoreRunes)
            {
                if (m_aMoltenCoreRune.m_uiRuneEntry == pGo->GetEntry() && GetData(m_aMoltenCoreRune.m_uiType) == SPECIAL)
                {
                    pGo->UseDoorOrButton();
                    break;
                }
            }
            break;
        // Runes' Flames Circles
        case GO_CIRCLE_MAGMADAR:
        case GO_CIRCLE_GEHENNAS:
        case GO_CIRCLE_GARR:
        case GO_CIRCLE_SHAZZRAH:
        case GO_CIRCLE_BARON_GEDDON:
        case GO_CIRCLE_SULFURON:
        case GO_CIRCLE_GOLEMAGG:
            // Delete the Flames Circle around the rune if the boss guarding it is killed
            m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            for (auto m_aMoltenCoreRune : m_aMoltenCoreRunes)
            {
                if (m_aMoltenCoreRune.m_uiFlamesCircleEntry == pGo->GetEntry() && (GetData(m_aMoltenCoreRune.m_uiType) == SPECIAL || GetData(m_aMoltenCoreRune.m_uiType) == DONE))
                {
                    pGo->SetLootState(GO_JUST_DEACTIVATED);
                    break;
                }
            }
            break;

        // Majordomo event chest
        case GO_CACHE_OF_THE_FIRE_LORD:
        // Ragnaros GOs
        case GO_LAVA_STEAM:
        case GO_LAVA_SPLASH:
            m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            break;
    }
}

void instance_molten_core::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_LUCIFRON:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_MAGMADAR:
        case TYPE_GEHENNAS:
        case TYPE_GARR:
        case TYPE_SHAZZRAH:
        case TYPE_GEDDON:
        case TYPE_GOLEMAGG:
        case TYPE_SULFURON:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                for (auto m_aMoltenCoreRune : m_aMoltenCoreRunes)
                {
                    if (m_aMoltenCoreRune.m_uiType == uiType)
                    {
                        if (GameObject* pGo = GetSingleGameObjectFromStorage(m_aMoltenCoreRune.m_uiFlamesCircleEntry))
                            pGo->SetLootState(GO_JUST_DEACTIVATED);
                        break;
                    }
                }
            }
            break;
        case TYPE_MAJORDOMO:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoRespawnGameObject(GO_CACHE_OF_THE_FIRE_LORD, HOUR);
                if (GameObject* chest = GetSingleGameObjectFromStorage(GO_CACHE_OF_THE_FIRE_LORD))
                    if (Player* player = GetPlayerInMap(false, false))
                        chest->GenerateLootFor(player);
            }
            break;
        case TYPE_RAGNAROS:
            m_auiEncounter[uiType] = uiData;
            break;
    }

    // Check if Majordomo can be summoned
    if (uiData == SPECIAL)
        DoSpawnMajordomoIfCan(false);

    if (uiData == DONE || uiData == SPECIAL)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                   << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
                   << m_auiEncounter[9];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_molten_core::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

// Handle Majordomo summon here
void instance_molten_core::DoSpawnMajordomoIfCan(bool bByPlayerEnter)
{
    // If both Majordomo and Ragnaros events are finished, return
    if (m_auiEncounter[TYPE_MAJORDOMO] == DONE && m_auiEncounter[TYPE_RAGNAROS] == DONE)
        return;

    // If already spawned return
    if (GetSingleCreatureFromStorage(NPC_MAJORDOMO, true))
        return;

    // Check if all rune bosses are done
    for (uint8 i = TYPE_MAGMADAR; i < TYPE_MAJORDOMO; ++i)
    {
        if (m_auiEncounter[i] != SPECIAL)
            return;
    }

    Player* pPlayer = GetPlayerInMap();
    if (!pPlayer)
        return;

    uint8 uiSummonPos = m_auiEncounter[TYPE_MAJORDOMO] == DONE ? 1 : 0;
    SpawnMajordomo(pPlayer, !bByPlayerEnter, uiSummonPos);
}

void instance_molten_core::SpawnMajordomo(Unit* summoner, bool initialSummon, uint8 summonPos)
{
    // Summon Majordomo
    // If Majordomo encounter isn't done, summon at encounter place, else near Ragnaros
    if (Creature* pMajordomo = summoner->SummonCreature(m_aMajordomoLocations[summonPos].m_uiEntry, m_aMajordomoLocations[summonPos].m_fX, m_aMajordomoLocations[summonPos].m_fY, m_aMajordomoLocations[summonPos].m_fZ, m_aMajordomoLocations[summonPos].m_fO, TEMPSPAWN_MANUAL_DESPAWN, 2 * HOUR * IN_MILLISECONDS))
    {
        if (summonPos)                                      // Majordomo encounter already done, set faction
        {
            pMajordomo->SetFactionTemporary(FACTION_MAJORDOMO_FRIENDLY, TEMPFACTION_RESTORE_RESPAWN);
            pMajordomo->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
            pMajordomo->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
        else                                                // Else yell and summon adds
        {
            if (initialSummon)
                DoScriptText(SAY_MAJORDOMO_SPAWN, pMajordomo);

            for (auto& m_aBosspawnLocElite : m_aBosspawnLocsElite)
                pMajordomo->SummonCreature(m_aBosspawnLocElite.m_uiEntry, m_aBosspawnLocElite.m_fX, m_aBosspawnLocElite.m_fY, m_aBosspawnLocElite.m_fZ, m_aBosspawnLocElite.m_fO, TEMPSPAWN_MANUAL_DESPAWN, DAY * IN_MILLISECONDS);
            for (auto& m_aBosspawnLocHealer : m_aBosspawnLocsHealer)
                pMajordomo->SummonCreature(m_aBosspawnLocHealer.m_uiEntry, m_aBosspawnLocHealer.m_fX, m_aBosspawnLocHealer.m_fY, m_aBosspawnLocHealer.m_fZ, m_aBosspawnLocHealer.m_fO, TEMPSPAWN_MANUAL_DESPAWN, DAY * IN_MILLISECONDS);
        }
    }
}

void instance_molten_core::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);

    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7]
               >> m_auiEncounter[8] >> m_auiEncounter[9];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_molten_core::ShowChatCommands(ChatHandler* handler)
{
    handler->SendSysMessage("This instance supports the following commands:\n spawnmajordomo(0 - initial,1 - ragnaros)");
}

void instance_molten_core::ExecuteChatCommand(ChatHandler* handler, char* args)
{
    char* result = handler->ExtractLiteralArg(&args);
    if (!result)
        return;
    std::string val = result;
    if (val == "spawnmajordomo")
    {
        uint32 summonPos;
        handler->ExtractUInt32(&args, summonPos);

        if (summonPos > 1)
        {
            handler->PSendSysMessage("Could not spawn majordomo because spawn %u was invalid. Please specify a value between 0-1", summonPos);
            return;
        }

        SpawnMajordomo(handler->GetSession()->GetPlayer(), true, summonPos);
    }
}

InstanceData* GetInstance_instance_molten_core(Map* pMap)
{
    return new instance_molten_core(pMap);
}

void AddSC_instance_molten_core()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_molten_core";
    pNewScript->GetInstanceData = &GetInstance_instance_molten_core;
    pNewScript->RegisterSelf();
}
