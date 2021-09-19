#include "..\\..\SDK\Vector.h"
#include "..\..\SDK\CCSGOAnimationState.h"

class CBaseEntity;
class CUserCmd;

class CAntiAim
{
public:

	void OnPrediction(CUserCmd*, bool*);
	void OnFrameStage(int);
private:

	void Yaw();
	void Pitch();
	bool LowerBodyYawUpdate();

	void CorrectMovement();

	QAngle angCurrent;
	QAngle angOriginal;
	Vector vecMove;

	float flLowerbodyTimer;
	float flDesyncDelta;
	bool bDesyncDirection;
	bool bFakeLagState = true;

	AnimationLayer animlayers[13];
	float poseparameters[24];
	float goalfeetyaw;

	CUserCmd* pCmd;
	CBaseEntity* pLocalPlayer;
};

extern CAntiAim g_AntiAim;