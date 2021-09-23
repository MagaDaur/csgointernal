#include "..\..\SDK\Vector.h"
#include "../ragebot/ragebot.h"

class CLegitBot : CRageBot
{
public:
    void OnPrediction(CUserCmd*);

private:
    void Smooth();
    void TriggerBot();

    QAngle angPrevAngles;
    
    CUserCmd* pCmd;
    CBaseEntity* pLocalPlayer;
	WeaponInfo_t* pWeaponInfo;
	CBaseCombatWeapon* pWeapon;
};

inline CLegitBot g_LegitBot;