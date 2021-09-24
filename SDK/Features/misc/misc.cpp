#include "misc.h"
#include "..\..\Utils\Math.h"
#include "..\..\SDK/CBaseEntity.h"
#include "..\..\SDK/CInput.h"
#include "..\..\SDK/ICVar.h"
#include "..\..\Settings.h"
#include "..\..\SDK/IClientEntityList.h"
#include "../ragebot/ragebot.h"
#include <math.h>

void CMisc::OnPrePrediction(CUserCmd* cmd, bool* bSendPackets)
{
	cmd->buttons |= IN_BULLRUSH;
	pLocalPlayer = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
	if (!pLocalPlayer) return;

	pCmd = cmd;

	Bhop();
	AutoStrafe();
	FakeLag();

	*bSendPackets = bFakelagState;
}

void CMisc::OnPrediction()
{
	
}

void CMisc::Bhop()
{
	if((pLocalPlayer->GetFlags() & FL_ONGROUND) == 0) pCmd->buttons &= ~IN_JUMP;
}

void CMisc::FakeLag()
{
	int limit = g_Settings.FakeLag;

	if(iChokedPackets <= limit)
	{
		iChokedPackets++;
		bFakelagState = false;
	}
	else
	{
		iChokedPackets = 0;
		bFakelagState = true;
	}
}

void CMisc::AutoStrafe()
{
	if (pLocalPlayer->GetFlags() & FL_ONGROUND || pLocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || pLocalPlayer->GetMoveType() == MOVETYPE_LADDER) return;

	Vector velocity = pLocalPlayer->GetVelocity();
	float velocity_direction = Math.VectorToAngle(velocity).y;

	Vector move = Vector(pCmd->forwardmove, pCmd->sidemove, 0);
	float move_direction = pCmd->viewangles.y - Math.VectorToAngle(move).y;

	float direction_delta = Math.NormalizeFloat(move_direction - velocity_direction);
	float ideal_strafe = RAD2DEG(asinf(30.f / velocity.Length2D()));
	float new_direction = velocity_direction + Math.Clamp(direction_delta, -ideal_strafe, ideal_strafe);

	if(pCmd->command_number % 2 == 0) new_direction += 88.f; else new_direction -= 88.f;

	new_direction = Math.NormalizeFloat(pCmd->viewangles.y - new_direction);
	Vector new_move = Math.AngleToVector(QAngle(0.f, new_direction, 0.f));

	pCmd->forwardmove = new_move.x * 450.f;
	pCmd->sidemove = new_move.y * 450.f;
}