#include "lagcomp.h"
#include "CBaseEntity.h"
#include "IClientMode.h"
#include "IClientEntityList.h"
#include "IVEngineClient.h"
#include "Studio.h"

void CLagComp::OnFrameStage(int curStage)
{
    if(curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_END)
    {
        for(int i = 1; i <= 64; i++)
        {
            CBaseEntity* player = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
            if(!player)
            {
                ClearRecords(i);
                continue;
            }
            StoreRecord(player);
        }
    }
}

void CLagComp::ClearRecords(int idx)
{
    deqRecords[idx - 1].clear();
}

void CLagComp::StoreRecord(CBaseEntity* player)
{
    LagRecord temp;

    temp.pBasePlayer = player;
    temp.simtime = player->GetSimulationTime();
    temp.animtime = player->GetAnimTime();
    temp.mins = player->GetMins();
    temp.maxs = player->GetMaxs();
    player->SetupBones(temp.matrix, 128, BONE_USED_BY_HITBOX, 0.f);

    GetRecords(player)->push_front(temp);
}

std::deque<LagRecord>* CLagComp::GetRecords(CBaseEntity* player)
{
    return &deqRecords[player->EntIndex() - 1];
}