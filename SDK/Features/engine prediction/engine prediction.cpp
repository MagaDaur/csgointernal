#include "engine prediction.h"
#include "..\..\SDK\CInput.h"
#include "..\..\SDK\CBaseEntity.h"
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\CPrediction.h"
#include "..\..\SDK\CGlobalVarsBase.h"
#include "..\..\SDK\IClientEntityList.h"
#pragma once

float flOldCurtime;
float flOldFrametime;
CBaseEntity* pLocalPlayer;

void engine_prediction::RunEnginePred(CUserCmd* cmd)
{
	pLocalPlayer = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
	if (!pLocalPlayer) return;

	flOldCurtime    = g_pGlobalVars->curtime;
	flOldFrametime  = g_pGlobalVars->frametime;

    g_pGlobalVars->curtime = pLocalPlayer->GetTickBase() * g_pGlobalVars->intervalPerTick;
    g_pGlobalVars->frametime = g_pGlobalVars->intervalPerTick;

    g_pMovement->StartTrackPredictionErrors(pLocalPlayer);

    CMoveData data;
    memset(&data, 0, sizeof(CMoveData));
    g_pMoveHelper->SetHost(pLocalPlayer);
    g_pPrediction->SetupMove(pLocalPlayer, cmd, g_pMoveHelper, &data);
    g_pMovement->ProcessMovement(pLocalPlayer, &data);
    g_pPrediction->FinishMove(pLocalPlayer, cmd, &data);
}

void engine_prediction::EndEnginePred()
{
    g_pMovement->FinishTrackPredictionErrors(pLocalPlayer);
    g_pMoveHelper->SetHost(nullptr);

    g_pGlobalVars->curtime      = flOldCurtime;
    g_pGlobalVars->frametime    = flOldFrametime;
}
