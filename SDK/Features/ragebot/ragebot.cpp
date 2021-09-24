#include "ragebot.h"
#include "..\..\SDK\CBaseEntity.h"
#include "..\..\SDK\CInput.h"
#include "..\..\SDK\CPrediction.h"
#include "..\..\Settings.h"
#include "..\..\SDK\CCSGOAnimationState.h"
#include "..\..\SDK\CBaseCombatWeapon.h"
#include "..\..\SDK\IClientMode.h"
#include "..\..\SDK\IClientEntityList.h"
#include "../lagcomp/lagcomp.h"

void CRageBot::OnPrediction(CUserCmd* cmd, bool* bSendPackets)
{
	pLocalPlayer = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
	if (!pLocalPlayer) return;
	pWeapon = pLocalPlayer->GetActiveWeapon();
	if (!pWeapon) return;
	pCmd = cmd;

	bFakeLagState = *bSendPackets;

	AutoShot();

	*bSendPackets = bFakeLagState;
}

void CRageBot::OnPostPrediction(bool* bSendPackets)
{
	static ConVar* sv_maxusrcmdprocessticks = g_pCvar->FindVar("sv_maxusrcmdprocessticks");

	if ((pCmd->buttons & IN_ATTACK) && Exploits.bCharged)
	{
		Exploits.iShiftAmount = sv_maxusrcmdprocessticks->GetInt() - 2;
		Exploits.lastcmd = *pCmd;
		Exploits.flLastShiftTime = g_pGlobalVars->curtime;
	}

	if (Exploits.bShouldCharge && g_pGlobalVars->curtime - Exploits.flLastShiftTime > 0.5f)
	{
		pCmd->tick_count = INT_MAX;

		if (Exploits.iChrageAmount++ >= sv_maxusrcmdprocessticks->GetInt() - 2)
		{
			Exploits.bCharged = true;
			Exploits.bShouldCharge = false;
			Exploits.iChrageAmount = 0;
		}
	}
}

void CRageBot::AutoShot()
{
	if (pWeapon->GetNextPrimaryAttack() > g_pGlobalVars->curtime)	pCmd->buttons &= ~IN_ATTACK;
	else if (pCmd->buttons & IN_ATTACK)
	{
		bFakeLagState = true;
	}
}

CBaseEntity* CRageBot::TargetSelection(int type)
{
	CBaseEntity* pRet = NULL;

	float selection = FLT_MAX;
	if (type == TARGET_SELECTION_DAMAGE)
		selection = 0.f;

	for (int i = 1; i < 64; i++)
	{
		CBaseEntity* player = g_pEntityList->GetClientEntity(i);
		if (!player || player->IsLocalPlayer() || !player->IsAlive() || player->IsDormant()) continue;
	
		if (type == TARGET_SELECTION_DISTANCE)
		{
			float distance = (player->GetOrigin() - pLocalPlayer->GetOrigin()).Length();
			if (distance < selection)
			{
				selection = distance;
				pRet = player;
			}
		}
		else if (type == TARGET_SELECTION_FOV)
		{
			for(auto& record : *g_LagComp.GetRecords(player))
			{
				QAngle ang = (Math.CalcAngle(pLocalPlayer->GetEyePosition(), record.eyeposition) - pCmd->viewangles);

				Math.NormalizeAngles(ang);

				float fov = ang.Length();
				if (fov < selection)
				{
					selection = fov;
					pRet = player;
				}
			}
		}
		else if (type == TARGET_SELECTION_DAMAGE)
		{

		}
	}
	return pRet;
}