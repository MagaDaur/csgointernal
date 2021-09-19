#include "ragebot.h"
#include "..\..\SDK\CBaseEntity.h"
#include "..\..\SDK\CInput.h"
#include "..\..\SDK\CPrediction.h"
#include "..\..\Settings.h"
#include "..\..\SDK\CCSGOAnimationState.h"
#include "..\..\SDK\CBaseCombatWeapon.h"
#include "..\..\SDK\IClientMode.h"
#include "..\..\SDK\IClientEntityList.h"

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

void CRageBot::AutoShot()
{
	if (pWeapon->GetNextPrimaryAttack() > g_pGlobalVars->curtime)	pCmd->buttons &= ~IN_ATTACK;
	else if (pCmd->buttons & IN_ATTACK) bFakeLagState = true;
}