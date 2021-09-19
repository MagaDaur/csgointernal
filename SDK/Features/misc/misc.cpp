#include "misc.h"
#include "Utils\Math.h"
#include "CBaseEntity.h"
#include "CInput.h"
#include "ICVar.h"
#include "Settings.h"
#include "IClientEntityList.h"
#include <math.h>

void CMisc::OnPrePrediction(CUserCmd* cmd, bool* bSendPackets)
{
	pLocalPlayer = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
	if (!pLocalPlayer) return;

	pCmd = cmd;

	Bhop();
	FakeLag();

	*bSendPackets = bFakelagState;
}

void CMisc::OnPrediction()
{
	AutoStrafe();
}

void CMisc::Bhop()
{
	if((pLocalPlayer->GetFlags() & FL_ONGROUND) == 0) pCmd->buttons &= ~IN_JUMP;
}

void CMisc::FakeLag()
{
	if(iChokedPackets <= g_Settings.FakeLag)
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
	if (pLocalPlayer->GetFlags() & FL_ONGROUND) return;

	Vector velocity = pLocalPlayer->GetVelocity();
	float velocity_direction = Math.VectorToAngle(velocity).y;

	Vector move = Vector(pCmd->forwardmove, pCmd->sidemove, 0);
	float move_direction = pCmd->viewangles.y - Math.VectorToAngle(move).y;

	float direction_delta = Math.NormalizeFloat(move_direction - velocity_direction);
	float ideal_strafe = RAD2DEG(asinf(30.f / velocity.Length2D()));
	float new_direction = velocity_direction + Math.Clamp(direction_delta, -ideal_strafe, ideal_strafe);

	if(pCmd->command_number % 2 == 0) new_direction += 89.f; else new_direction -= 89.f;

	new_direction = Math.NormalizeFloat(pCmd->viewangles.y - new_direction);
	Vector new_move = Math.AngleToVector(QAngle(0.f, new_direction, 0.f));

	pCmd->forwardmove = new_move.x * 450.f;
	pCmd->sidemove = new_move.y * 450.f;
}