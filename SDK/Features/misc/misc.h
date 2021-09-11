#include "..\\..\\SDK\CBaseEntity.h"
#include "..\\..\\SDK\CInput.h"

class CMisc
{
public:
	void OnPrePrediction(CBaseEntity*, CUserCmd*, bool*);
private:
	void Bhop(CBaseEntity*, CUserCmd*);
	void FakeLag(CBaseEntity*, CUserCmd*);

	bool bFakelagState;
	int iChokedPackets;
};

inline CMisc g_Misc;