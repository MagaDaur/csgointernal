#pragma once
#include "IClientEntity.h"

class IClientEntityList
{
public:
    virtual IClientNetworkable*   GetClientNetworkable(int entnum) = 0;
    virtual void*                 vtablepad0x1(void) = 0;
    virtual void*                 vtablepad0x2(void) = 0;
    virtual CBaseEntity*         GetClientEntity(int entNum) = 0;
    virtual IClientEntity*        GetClientEntityFromHandle(CBaseHandle hEnt) = 0;
    virtual int                   NumberOfEntities(bool bIncludeNonNetworkable) = 0;
    virtual int                   GetHighestEntityIndex(void) = 0;
    virtual void                  SetMaxEntities(int maxEnts) = 0;
    virtual int                   GetMaxEntities() = 0;
	CBaseEntity* GetLocalPlayer();
};
extern IClientEntityList*  g_pEntityList;