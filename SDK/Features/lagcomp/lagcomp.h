#include "..\..\SDK\VMatrix.h"
#include "..\..\SDK\Vector.h"
#include "..\..\SDK\CCSGOAnimationState.h"
#include <deque>

class CBaseEntity;

struct LagRecord
{
	CBaseEntity* pBasePlayer;

	float simtime;
	float animtime;

	Vector mins;
	Vector maxs;

	AnimationLayer animlayers[13];
	float poseparams[24];

	matrix3x4_t matrix[128];
	matrix3x4_t renderable_matrix[128];
};

class CLagComp
{
public:
	void OnFrameStage(int);
	std::deque<LagRecord>* GetRecords(CBaseEntity*);

	bool bShouldAnimate = true;
private:
	void StoreRecord(CBaseEntity*);
	void ClearRecords(int);

	std::deque<LagRecord> deqRecords[64];
};

extern CLagComp g_LagComp;