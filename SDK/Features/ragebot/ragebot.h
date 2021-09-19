class CBaseEntity;
class CUserCmd;
class CBaseCombatWeapon;

class CRageBot
{
public:
	void OnPrediction(CUserCmd*, bool*);
private:
	void AutoShot();

	bool bFakeLagState = true;

	CUserCmd* pCmd;
	CBaseEntity* pLocalPlayer;
	CBaseCombatWeapon* pWeapon;
};

extern CRageBot g_RageBot;