#include "misc.h"
#include "ICVar.h"

#include "CBaseEntity.h"
#include "CInput.h"
#include "Math.h"
#include <math.h>

void CMisc::OnPrePrediction(CBaseEntity* local, CUserCmd* cmd, bool* bSendPackets)
{
	Bhop(local, cmd);
	FakeLag(local, cmd);

	*bSendPackets = bFakelagState;
}

void CMisc::Bhop(CBaseEntity* local, CUserCmd* cmd)
{
	if(cmd->buttons & IN_JUMP && (local->GetFlags() & FL_ONGROUND) == 0)
	{
		cmd->buttons &= ~IN_JUMP;
		
		AutoStrafe(local, cmd);
	}
}

void CMisc::FakeLag(CBaseEntity* local, CUserCmd* cmd)
{
	static ConVar* sv_maxusrcmdprocessticks = g_pCvar->FindVar("sv_maxusrcmdprocessticks");
	if(iChokedPackets < sv_maxusrcmdprocessticks->GetInt() - 2)
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

void CMisc::AutoStrafe(CBaseEntity* local, CUserCmd* cmd)
{
	static ConVar* sv_airaccelerate = g_pCvar->FindVar("sv_airaccelerate");
	float airaccelerate_factor = Math.Clamp(sv_airaccelerate->GetInt() / 100.f, 0.f, 1.f);

	Vector velocity = local->GetVelocity();
	float velocity_direction = Math.VectorToAngle(velocity).y;

	Vector move = Vector(cmd.forwardmove, cmd.sidemove, 0);
	float move_direction = cmd->viewangles.y + Math.VectorToAngle(move).y;

	float direction_delta = Math.NormalizeFloat(move_direction - velocity_direction);
	float ideal_step = Math.Clamp(RAD2DEG(asinf(30.f / velocity.Length2D())) * airaccelerate_factor, 0, 45.f);

	float new_direction = velocity_direction + Math.Clamp(direction_delta, 0.f, ideal_step);
	Vector new_move = Math.AngleToVector(QAngle(0.f, new_direction, 0.f));

	cmd->forwardmove = new_move.x * 450.f;
	cmd->sidemove = new_move.y * 450.f;
}