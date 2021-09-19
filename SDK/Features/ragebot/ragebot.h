class CBaseEntity;
class CUserCmd;
class CBaseCombatWeapon;

class CRageBot
{
public:
	void OnPrediction(CUserCmd*, bool*);

protected:
	void AutoShot();
	
private:
	bool bFakeLagState = true;

	CUserCmd* pCmd;
	CBaseEntity* pLocalPlayer;
	CBaseCombatWeapon* pWeapon;
};

extern CRageBot g_RageBot;