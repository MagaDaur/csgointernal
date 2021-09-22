#include "..\ragebot\ragebot.h"
#include "..\..\SDK\Vector.h"
class CLegitBot : protected CRageBot
{
public:
    void OnPrediction(CUserCmd*);

private:
    void Smooth();
    void TriggerBot();

    QAngle angPrevAngles;
    
    CUserCmd* pCmd;
    CBaseEntity* pLocalPlayer;
};

extern CLegitBot g_LegitBot;