#include "CInput.h"
class CBaseEntity;
class CUserCmd;
class CMisc
{
public:
	void OnPrePrediction(CBaseEntity*, CUserCmd*, bool*);
private:
	void Bhop(CBaseEntity*, CUserCmd*);
	void FakeLag(CBaseEntity*, CUserCmd*);
	void AutoStrafe(CBaseEntity*, CUserCmd*);

	bool bFakelagState;
	int iChokedPackets;
};

inline CMisc g_Misc;