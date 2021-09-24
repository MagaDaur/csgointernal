#include "antiaim.h"
#include "..\..\SDK\CBaseEntity.h"
#include "..\..\SDK\CInput.h"
#include "..\..\SDK\CPrediction.h"
#include "..\..\Settings.h"
#include "..\..\SDK\IClientMode.h"
#include "..\..\SDK\IClientEntityList.h"
#include "../../SDK/Studio.h"

void CAntiAim::OnPrediction(CUserCmd* cmd, bool* bSendPackets)
{
	pLocalPlayer = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
	if (!pLocalPlayer) return;
	pCmd = cmd;

	bFakeLagState = *bSendPackets;
	angOriginal = cmd->viewangles;
	vecMove = Vector(cmd->forwardmove, cmd->sidemove, 0.0f);

	if(g_Settings.AntiAimEnable)
	{
		auto movetype = pLocalPlayer->GetMoveType();
		if(movetype != MOVETYPE_LADDER && 
			movetype != MOVETYPE_NOCLIP &&
			(cmd->buttons & IN_ATTACK) == 0 &&
			(cmd->buttons & IN_USE) == 0)
		{
			Pitch();
			Yaw();
		}
	}

	Math.NormalizeAngles(pCmd->viewangles);
	angCurrent = pCmd->viewangles;

	*bSendPackets = bFakeLagState;

	CorrectMovement();
}

void CAntiAim::OnFrameStage(int curStage)
{
	pLocalPlayer = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
	if (!pLocalPlayer) return;
	auto animstate = pLocalPlayer->GetAnimState();
	if (!animstate) return;
	if (curStage == FRAME_RENDER_START)
	{
		if (animstate->m_iLastClientSideAnimationUpdateFramecount)
			animstate->m_iLastClientSideAnimationUpdateFramecount--;
		animstate->Update(angCurrent);
		if (bFakeLagState)
		{
			memcpy(poseparameters, pLocalPlayer->GetPoseParameters(), sizeof(float) * 24);
			memcpy(animlayers, pLocalPlayer->GetAnimLayers(), sizeof(AnimationLayer) * 13);
			memcpy(&goalfeetyaw, &animstate->m_flGoalFeetYaw, sizeof(float));

			bFakeLagState = false;
		}
		memcpy(pLocalPlayer->GetPoseParameters(), poseparameters, sizeof(float) * 24);
		memcpy(pLocalPlayer->GetAnimLayers(), animlayers, sizeof(AnimationLayer) * 13);
		pLocalPlayer->SetAbsAngles(goalfeetyaw);
	}
}

void CAntiAim::Pitch()
{
	pCmd->viewangles.x = 89.f;
}

void CAntiAim::Yaw()
{

	pCmd->viewangles.y += 180.f;

	if(g_Settings.DesyncEnable)
	{
		if(LowerBodyYawUpdate())
		{
			pCmd->viewangles.y += 120.f;
			bFakeLagState = false;
		}
		else if(bFakeLagState == false)
		{
			pCmd->viewangles.y += g_Settings.FakeAmount;
		}
	}
}

bool CAntiAim::LowerBodyYawUpdate()
{
	Vector velocity = pLocalPlayer->GetVelocity();
	if(velocity.Length2D() == 0.f || velocity.z == 0.f)
	{
		if(flLowerbodyTimer <= g_pGlobalVars->curtime)
		{
			flLowerbodyTimer = g_pGlobalVars->curtime + 1.1f;

			return true;
		}
	}
	else
	{
		flLowerbodyTimer = g_pGlobalVars->curtime + 0.22f;
	}
	return false;
}

void CAntiAim::CorrectMovement()
{
	float deltaView = Math.NormalizeFloat(pCmd->viewangles.y - angOriginal.y);
	pCmd->forwardmove = std::clamp(cos(DEG2RAD(deltaView)) * vecMove.x + cos(DEG2RAD(deltaView + 90.f)) * vecMove.y, -450.f, 450.f);
	pCmd->sidemove = std::clamp(sin(DEG2RAD(deltaView)) * vecMove.x + sin(DEG2RAD(deltaView + 90.f)) * vecMove.y, -450.f, 450.f);
	pCmd->buttons &= ~IN_FORWARD;
	pCmd->buttons &= ~IN_BACK;
	pCmd->buttons &= ~IN_MOVELEFT;
	pCmd->buttons &= ~IN_MOVERIGHT;
}