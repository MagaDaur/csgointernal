#pragma once
#include "..\..\SDK\CInput.h"
#include "..\..\SDK\CBaseEntity.h"
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\CPrediction.h"
#include "..\..\SDK\CGlobalVarsBase.h"
namespace engine_prediction
{
    void RunEnginePred(CBaseEntity* local, CUserCmd* cmd);
    void EndEnginePred(CBaseEntity* local);
}