#include "..\..\SDK\CInput.h"

enum nTargetSelectionType : int
{
	TARGET_SELECTION_FOV,
	TARGET_SELECTION_DISTANCE,
	TARGET_SELECTION_DAMAGE,
};

class CBaseEntity;
class WeaponInfo_t;
class CBaseCombatWeapon;

class CRageBot
{
public:
	void OnPrediction(CUserCmd*, bool*);
	void OnPostPrediction(bool*);

	struct exploit_t
	{
		bool bCharged = true;
		bool bShouldCharge;
		int iChrageAmount;
		float flLastShiftTime;

		CUserCmd lastcmd;

		int iShiftAmount;
	}Exploits;
protected:
	CBaseEntity* TargetSelection(int);

private:
	void AutoShot();

	CUserCmd* pCmd;
	CBaseEntity* pLocalPlayer;
	WeaponInfo_t* pWeaponInfo;
	CBaseCombatWeapon* pWeapon;

	bool bFakeLagState = true;
};

inline CRageBot g_RageBot;