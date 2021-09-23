#include "legitbot.h"
#include "../../SDK/IClientEntityList.h"
#include "../../SDK/CBaseEntity.h"
#include "../../SDK/CInput.h"
#include "../../SDK/CBaseCombatWeapon.h"
#include "../../SDK/IEngineTrace.h"
#include "../lagcomp/lagcomp.h"

void CLegitBot::OnPrediction(CUserCmd* cmd)
{
	pLocalPlayer = g_pEntityList->GetLocalPlayer();
	if (!pLocalPlayer) return;
	pWeapon = pLocalPlayer->GetActiveWeapon();
	if (!pWeapon) return;
	pWeaponInfo = pWeapon->GetWeaponInfo();
	if (!pWeaponInfo) return;
	pCmd = cmd;

	TriggerBot();
}

void CLegitBot::TriggerBot()
{
	float flRange = (pWeaponInfo->weaponType == WEAPONTYPE_KNIFE) ? 32.f : pWeaponInfo->flRange;
	for (int i = 1; i <= 64; i++)
	{
		CBaseEntity* player = g_pEntityList->GetClientEntity(i);
		if (!player || player->IsLocalPlayer() || player->IsTeamMate() || !player->IsAlive() || player->IsDormant()) continue;
		for (auto& record : *g_LagComp.GetRecords(player))
		{
			if (record.IsValid())
			{
				Vector end = pLocalPlayer->GetEyePosition() + Math.AngleToVector(pCmd->viewangles) * flRange;
				Ray_t ray;
				ray.Init(pLocalPlayer->GetEyePosition(), end);
				CTraceFilter filter;
				filter.pSkip = pLocalPlayer;
				C_Trace trace;

				record.Override(player);
				g_pEngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);
				record.Reset(player);

				if (trace.m_pEnt && trace.m_pEnt->IsPlayer() && !trace.m_pEnt->IsTeamMate())
				{
					pCmd->buttons |= IN_ATTACK;
					pCmd->tick_count = TIME_TO_TICKS(record.simtime + g_LagComp.GetLerpTime());
					return;
				}
			}
		}
	}
}