#include "..\..\SDK\CInput.h"

class CBaseEntity;
class CBaseCombatWeapon;

class CRageBot
{
public:
	void OnPrediction(CUserCmd*, bool*);

protected:
	void AutoShot();
	
private:
	bool bFakeLagState = true;

	struct exploit_t
	{
		bool bCharged;
		bool bShouldCharge;
		float flChrageAmount;

		CUserCmd lastcmd;

		int iShiftAmount;
	}Exploits;

	CUserCmd* pCmd;
	CBaseEntity* pLocalPlayer;
	CBaseCombatWeapon* pWeapon;
};

extern CRageBot g_RageBot;