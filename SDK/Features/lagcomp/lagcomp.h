#include "..\..\SDK\VMatrix.h"
#include "..\..\SDK\Vector.h"
#include "..\..\SDK\CCSGOAnimationState.h"
#include "../../SDK/CBaseEntity.h"
#include "../../SDK/ICVar.h"
#include <deque>

struct LagRecord
{

	float simtime;
	float animtime;

	Vector mins;
	Vector maxs;

	Vector eyeposition;
	Vector position;
	QAngle eyeangles;

	AnimationLayer animlayers[13];
	float poseparams[24];

	matrix3x4_t matrix[128];

	struct backup_t
	{
		Vector position;
		matrix3x4_t matrix[128];

		Vector mins;
		Vector maxs;
	}backup;
	
	void Override(CBaseEntity* pBasePlayer)
	{
		if (!pBasePlayer) return;
		backup.mins = pBasePlayer->GetCollideable()->OBBMins();
		backup.maxs = pBasePlayer->GetCollideable()->OBBMaxs();
		backup.position = pBasePlayer->GetRenderOrigin();
		std::memcpy(backup.matrix, pBasePlayer->GetBoneMatrix(), sizeof(matrix3x4_t) * 128);

		pBasePlayer->GetCollideable()->OBBMins() = mins;
		pBasePlayer->GetCollideable()->OBBMaxs() = maxs;
		pBasePlayer->SetAbsOrigin(position);
		std::memcpy(pBasePlayer->GetBoneMatrix(), matrix, sizeof(matrix3x4_t) * 128);
	}

	void Reset(CBaseEntity* pBasePlayer)
	{
		if (!pBasePlayer) return;
		pBasePlayer->GetCollideable()->OBBMins() = backup.mins;
		pBasePlayer->GetCollideable()->OBBMaxs() = backup.maxs;
		pBasePlayer->SetAbsOrigin(backup.position);
		std::memcpy(pBasePlayer->GetBoneMatrix(), backup.matrix, sizeof(matrix3x4_t) * 128);
	}

	bool IsValid()
	{
		static ConVar* sv_maxunlag = g_pCvar->FindVar("sv_maxunlag");
		return abs(g_pGlobalVars->curtime - simtime) <= sv_maxunlag->GetFloat();
	}
};

class CLagComp
{
public:
	void OnFrameStage(int);
	std::deque<LagRecord>* GetRecords(CBaseEntity*);

	float GetLerpTime();

	bool bShouldAnimate = true;
private:
	void StoreRecord(CBaseEntity*);
	void ClearRecords(int);

	std::deque<LagRecord> deqRecords[64];
};

inline CLagComp g_LagComp;