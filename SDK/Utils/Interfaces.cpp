#include "Interfaces.h"
#include "Utils.h"

#include "..\SDK\IClientMode.h"
#include "..\SDK\IBaseClientDll.h"
#include "..\SDK\IClientEntityList.h"
#include "..\SDK\IVEngineClient.h"
#include "..\SDK\CPrediction.h"
#include "..\SDK\IGameEvent.h"
#include "..\SDK\ISurface.h"
#include "..\SDK\IRenderView.h"
#include "..\SDK\IVModelRender.h"
#include "..\SDK\IVModelInfoClient.h"
#include "..\SDK\IEngineTrace.h"
#include "..\SDK\IPhysics.h"
#include "..\\SDK\ICVar.h"
#include "..\SDK\CInput.h"
#include "..\SDK\IPanel.h"
#include "..\SDK\DebugOverlay.h"
#include "..\SDK\IWeaponSystem.h"

#define enc_str(s) std::string(s)
#define enc_char(s) enc_str(s).c_str()
#define enc_wstr(s) std::wstring(enc_str(s).begin(), enc_str(s).end())
#define enc_wchar(s) enc_wstr(s).c_str()

// Initializing global interfaces

IBaseClientDLL*     g_pClientDll    = nullptr;
IClientMode*        g_pClientMode   = nullptr;
IClientEntityList*  g_pEntityList   = nullptr;
IVEngineClient*     g_pEngine       = nullptr;
CPrediction*        g_pPrediction   = nullptr;
IGameMovement*      g_pMovement     = nullptr;
IMoveHelper*        g_pMoveHelper   = nullptr;
CGlobalVarsBase*    g_pGlobalVars   = nullptr;
IGameEventManager2* g_pEventManager = nullptr;
ISurface*           g_pSurface      = nullptr;
IVRenderView*       g_pRenderView   = nullptr;
IVModelRender*		g_MdlRender     = nullptr;
IVModelInfo*  g_MdlInfo       = nullptr;
IMaterialSystem*    g_MatSystem     = nullptr;
IEngineTrace*       g_pEngineTrace  = nullptr;
IPhysicsSurfaceProps* g_PhysSurface = nullptr;
ICVar* g_pCvar = nullptr;
CInput* g_pGameInput = nullptr;
IPanel* g_pPanel = nullptr;
CClientState* g_pClientState = nullptr;
IVDebugOverlay* g_pDebugOverlay = nullptr;
IWeaponSystem* g_pWeaponSys = nullptr;
CInputSystem* g_pInputSystem = nullptr;


namespace interfaces
{
	void* FindInterface(const char* Module, const char* InterfaceName)
	{
		void* Interface = nullptr;
		auto CreateInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(
			GetModuleHandleA(Module), enc_char("CreateInterface")));

		char PossibleInterfaceName[1024];
		for (int i = 1; i < 100; i++)
		{
			sprintf(PossibleInterfaceName, "%s0%i", InterfaceName, i);
			Interface = CreateInterface(PossibleInterfaceName, 0);
			if (Interface)
				break;

			sprintf(PossibleInterfaceName, "%s00%i", InterfaceName, i);
			Interface = CreateInterface(PossibleInterfaceName, 0);
			if (Interface)
				break;
		}

		//		if (!Interface)
		//			std::cout << (enc_str("Failed to find interface - ") + PossibleInterfaceName) << std::endl;
		//		else
		//			std::cout <<  (enc_str("Successfully found interface - ") + PossibleInterfaceName) << std::endl;

		return Interface;
	}
	template<typename T>
	T* CaptureInterface(const char* szModuleName, const char* szInterfaceVersion)
	{
		HMODULE moduleHandle = GetModuleHandleA(szModuleName);
		if (moduleHandle)   /* In case of not finding module handle, throw an error. */
		{
			CreateInterfaceFn pfnFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(moduleHandle, "CreateInterface"));
			return reinterpret_cast<T*>(pfnFactory(szInterfaceVersion, nullptr));
		}
		Utils::Log("Error getting interface %", szInterfaceVersion);
		return nullptr;
	}
    void Init()
    {
        g_pClientDll     = CaptureInterface<IBaseClientDLL>("client.dll", "VClient018");					// Get IBaseClientDLL
        g_pClientMode    = **reinterpret_cast<IClientMode***>    ((*reinterpret_cast<uintptr_t**>(g_pClientDll))[10] + 0x5u);  // Get IClientMode
        g_pGlobalVars    = **reinterpret_cast<CGlobalVarsBase***>((*reinterpret_cast<uintptr_t**>(g_pClientDll))[0]  + 0x1F); // Get CGlobalVarsBase
        g_pEntityList    = CaptureInterface<IClientEntityList>("client.dll", "VClientEntityList003");    // Get IClientEntityList
        g_pEngine        = CaptureInterface<IVEngineClient>("engine.dll", "VEngineClient014");						// Get IVEngineClient
        g_pPrediction    = CaptureInterface<CPrediction>("client.dll", "VClientPrediction001");          // Get CPrediction
        g_pMovement      = CaptureInterface<IGameMovement>("client.dll", "GameMovement001");             // Get IGameMovement
        g_pMoveHelper    = **reinterpret_cast<IMoveHelper***>((Utils::FindSignature("client.dll", "8B 0D ? ? ? ? 8B 46 08 68") + 0x2));  // Get IMoveHelper
        g_pEventManager  = CaptureInterface<IGameEventManager2>("engine.dll", "GAMEEVENTSMANAGER002");				// Get IGameEventManager2
        g_pSurface       = CaptureInterface<ISurface>("vguimatsurface.dll", "VGUI_Surface031");						// Get ISurface
		g_pRenderView    = CaptureInterface<IVRenderView>( "engine.dll", "VEngineRenderView014" );					// Get IVRenderView
		g_MdlRender      = CaptureInterface<IVModelRender>( "engine.dll", "VEngineModel016" );						// Get IVMofelRender
		g_MdlInfo        = reinterpret_cast<IVModelInfo*>(FindInterface("engine.dll", "VModelInfoClient"));			// Get IVModelInfoClient
		g_MatSystem      = CaptureInterface<IMaterialSystem>( "materialsystem.dll", "VMaterialSystem080" );          // Get IMaterialSystem
		g_pEngineTrace   = CaptureInterface<IEngineTrace>("engine.dll", "EngineTraceClient004");                  //EngineTraceClient004
		g_PhysSurface    = CaptureInterface<IPhysicsSurfaceProps>("vphysics.dll","VPhysicsSurfaceProps001");      //VPhysicsSurfaceProps001
		g_pCvar = CaptureInterface<ICVar>("vstdlib.dll", "VEngineCvar007");
		g_pGameInput = *(CInput * *)(Utils::FindSignature("client.dll", "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10") + 1);
		g_pPanel = reinterpret_cast<IPanel*>(FindInterface("vgui2.dll", "VGUI_Panel"));
		g_pClientState = **reinterpret_cast<CClientState***>(Utils::FindSignature("engine.dll", "A1 ? ? ? ? 8B 88 ? ? ? ? 85 C9 75 07") + 0x1);
		g_pDebugOverlay = CaptureInterface<IVDebugOverlay>("engine.dll", "VDebugOverlay004");
		g_pWeaponSys = *reinterpret_cast<IWeaponSystem**>(Utils::FindSignature("client.dll", "8B 35 ? ? ? ? FF 10 0F B7 C0") + 0x2);
		g_pInputSystem = CaptureInterface<CInputSystem>("vstdlib.dll'", "InputSystemVersion001");
	}
}
