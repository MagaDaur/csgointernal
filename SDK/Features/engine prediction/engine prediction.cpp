#include "engine prediction.h"
#pragma once

float flOldCurtime;
float flOldFrametime;

void engine_prediction::RunEnginePred(CBaseEntity* local, CUserCmd* cmd)
{
    flOldCurtime    = g_pGlobalVars->curtime;
    flOldFrametime  = g_pGlobalVars->frametime;

    g_pGlobalVars->curtime = local->GetTickBase() * g_pGlobalVars->intervalPerTick;
    g_pGlobalVars->frametime = g_pGlobalVars->intervalPerTick;

    g_pMovement->StartTrackPredictionErrors(local);

    CMoveData data;
    memset(&data, 0, sizeof(CMoveData));
    g_pMoveHelper->SetHost(local);
    g_pPrediction->SetupMove(local, cmd, g_pMoveHelper, &data);
    g_pMovement->ProcessMovement(local, &data);
    g_pPrediction->FinishMove(local, cmd, &data);
}

void engine_prediction::EndEnginePred(CBaseEntity* local)
{
    g_pMovement->FinishTrackPredictionErrors(local);
    g_pMoveHelper->SetHost(nullptr);

    g_pGlobalVars->curtime      = flOldCurtime;
    g_pGlobalVars->frametime    = flOldFrametime;
}
