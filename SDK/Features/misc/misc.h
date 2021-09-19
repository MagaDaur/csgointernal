class CBaseEntity;
class CUserCmd;
class CMisc
{
public:
	void OnPrePrediction(CUserCmd*, bool*);
	void OnPrediction();
private:
	void Bhop();
	void FakeLag();
	void AutoStrafe();

	bool bFakelagState = true;
	int iChokedPackets;

	CBaseEntity* pLocalPlayer;
	CUserCmd* pCmd;
};

extern CMisc g_Misc;