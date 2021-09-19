#pragma once
#include "CGlobalVarsBase.h"
#include "ClientClass.h"
#include "Vector.h"

class bf_write;

class IBaseClientDLL
{
public:
    // Connect appsystem components, get global interfaces, don't run any other init code
    virtual int              Connect(CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals) = 0; // 0
    virtual int              Disconnect(void) = 0;
    virtual int              Init(CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals) = 0;
    virtual void             PostInit() = 0;
    virtual void             Shutdown(void) = 0;
    virtual void             LevelInitPreEntity(char const* pMapName) = 0; // 5
    virtual void             LevelInitPostEntity() = 0;
    virtual void             LevelShutdown(void) = 0;
    virtual ClientClass*     GetAllClasses(void) = 0;
	virtual void				func9() = 0;
	virtual void				func10() = 0; // 10
	virtual void				func11() = 0;
	virtual void				func12() = 0;
	virtual void				func13() = 0;
	virtual void				func14() = 0;
	virtual void				func15() = 0; // 15
	virtual void				func16() = 0;
	virtual void				func17() = 0;
	virtual void				func18() = 0;
	virtual void				func19() = 0;
	virtual void				func20() = 0; // 20
	virtual void				func21() = 0;
	virtual void				func22() = 0;
	virtual void				func23() = 0;
	virtual bool				WriteUsercmdDeltaToBuffer(int, bf_write*, int, int, bool) = 0;
};
extern IBaseClientDLL* g_pClientDll;