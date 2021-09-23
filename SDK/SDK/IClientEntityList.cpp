#include "IClientEntityList.h"
#include  "IVEngineClient.h"

CBaseEntity* IClientEntityList::GetLocalPlayer()
{
	return GetClientEntity(g_pEngine->GetLocalPlayer());
}