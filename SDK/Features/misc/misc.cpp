#include "misc.h"

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
	}
}

void CMisc::FakeLag(CBaseEntity* local, CUserCmd* cmd)
{
	if(iChokedPackets < 14)
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