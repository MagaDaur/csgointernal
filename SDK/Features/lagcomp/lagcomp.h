#include "VMatrix.h"
#include "Vector.h"
#include <deque>

class CBaseEntity;

struct LagRecord
{
	CBaseEntity* pBasePlayer;

	float simtime;
	float animtime;

	Vector mins;
	Vector maxs;

	matrix3x4_t matrix[128];
};

class CLagComp
{
public:
	void OnFrameStage(int);

	std::deque<LagRecord>* GetRecords(CBaseEntity*);
private:
	void StoreRecord(CBaseEntity*);
	void ClearRecords(int);

	std::deque<LagRecord> deqRecords[64];
};