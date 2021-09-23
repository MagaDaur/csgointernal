#include "lagcomp.h"
#include "..\..\SDK\CBaseEntity.h"
#include "..\..\SDK\IClientMode.h"
#include "..\..\SDK\IClientEntityList.h"
#include "..\..\SDK\IVEngineClient.h"
#include "..\..\SDK\Studio.h"
#include "../../SDK/ICVar.h"
#include <algorithm>

void CLagComp::OnFrameStage(int curStage)
{
    if(curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_END)
    {
        for(int i = 1; i <= 64; i++)
        {
            CBaseEntity* player = g_pEntityList->GetClientEntity(i);
            if(!player || !player->IsAlive() || player->IsDormant())
            {
                ClearRecords(i);
                continue;
            }
			auto records = GetRecords(player);
			if (records->size() == 0 || player->GetSimulationTime() > records->front().simtime)
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

    temp.simtime = player->GetSimulationTime();
    temp.animtime = player->GetAnimTime();

    temp.mins = player->GetCollideable()->OBBMins();
    temp.maxs = player->GetCollideable()->OBBMaxs();

	temp.eyeposition = player->GetEyePosition();
	temp.position = player->GetOrigin();
	temp.eyeangles = player->GetEyeAngles();

	std::memcpy(temp.poseparams, player->GetPoseParameters(), sizeof(float) * 24);
	std::memcpy(temp.animlayers, player->GetAnimLayers(), sizeof(AnimationLayer) * 13);

	if (player->IsLocalPlayer() == false)
	{
		bShouldAnimate = true;
		player->UpdateClientAnimation();

		player->SetAbsOrigin(temp.position);
		player->SetAbsAngles(temp.eyeangles.y);
		std::memcpy(player->GetAnimLayers(), temp.animlayers, sizeof(AnimationLayer) * 13);
		std::memcpy(player->GetPoseParameters(), temp.poseparams, sizeof(float) * 24);

		player->SetupBones(temp.matrix, 128, BONE_USED_BY_HITBOX, 0.f);
	}

	auto records = GetRecords(player);
	records->push_front(temp);
	if (records->size() > 13)
		records->pop_back();
}

std::deque<LagRecord>* CLagComp::GetRecords(CBaseEntity* player)
{
    return &deqRecords[player->EntIndex() - 1];
}

float CLagComp::GetLerpTime()
{
	static auto cl_interp_ratio = g_pCvar->FindVar(("cl_interp_ratio"));
	static auto cl_interp = g_pCvar->FindVar(("cl_interp"));
	static auto sv_client_min_interp_ratio = g_pCvar->FindVar(("sv_client_min_interp_ratio"));
	static auto sv_client_max_interp_ratio = g_pCvar->FindVar(("sv_client_max_interp_ratio"));

	static auto cl_updaterate = g_pCvar->FindVar(("cl_updaterate"));
	static auto sv_minupdaterate = g_pCvar->FindVar(("sv_minupdaterate"));
	static auto sv_maxupdaterate = g_pCvar->FindVar(("sv_maxupdaterate"));

	auto update_rate = cl_updaterate->GetInt();
	if (sv_minupdaterate && sv_maxupdaterate)
		update_rate = std::clamp(update_rate, (sv_minupdaterate->GetInt()), (sv_maxupdaterate->GetInt()));

	auto lerp_amount = (cl_interp->GetFloat());
	auto lerp_ratio = (cl_interp_ratio->GetFloat());
	if (lerp_ratio == 0.f)
		lerp_ratio = 1.f;

	if (sv_client_min_interp_ratio && sv_client_max_interp_ratio && (sv_client_min_interp_ratio->GetFloat()) != -1.f)
		lerp_ratio = (float)std::clamp(lerp_ratio, (sv_client_min_interp_ratio->GetFloat()), (sv_client_max_interp_ratio->GetFloat()));

	return max(lerp_amount, lerp_ratio / (float)update_rate);

}