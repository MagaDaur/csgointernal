#include "lagcomp.h"
#include "..\..\SDK\CBaseEntity.h"
#include "..\..\SDK\IClientMode.h"
#include "..\..\SDK\IClientEntityList.h"
#include "..\..\SDK\IVEngineClient.h"
#include "..\..\SDK\Studio.h"

void CLagComp::OnFrameStage(int curStage)
{
    if(curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_END)
    {
        for(int i = 1; i <= 64; i++)
        {
            CBaseEntity* player = g_pEntityList->GetClientEntity(i);
            if(!player)
            {
                ClearRecords(i);
                continue;
            }
			auto records = GetRecords(player);
			if (records->size() == 0 || player->GetSimulationTime() > records->at(0).simtime)
			{
				StoreRecord(player);
			}
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

	std::memcpy(temp.poseparams, player->GetPoseParameters(), sizeof(float) * 24);
	std::memcpy(temp.animlayers, player->GetAnimLayers(), sizeof(AnimationLayer) * 13);

	if (player->EntIndex() != g_pEngine->GetLocalPlayer())
	{
		bShouldAnimate = true;
		player->UpdateClientAnimation();

		std::memcpy(player->GetAnimLayers(), temp.animlayers, sizeof(AnimationLayer) * 13);
		std::memcpy(player->GetPoseParameters(), temp.poseparams, sizeof(float) * 24);
	}

	player->SetupBones(temp.renderable_matrix, 128, BONE_USED_BY_ANYTHING, 0.f);
	player->SetupBones(temp.matrix, 128, BONE_USED_BY_HITBOX, 0.f);

	auto records = GetRecords(player);
	records->push_front(temp);
	if (records->size() > 14)
		records->pop_back();
}

std::deque<LagRecord>* CLagComp::GetRecords(CBaseEntity* player)
{
    return &deqRecords[player->EntIndex() - 1];
}