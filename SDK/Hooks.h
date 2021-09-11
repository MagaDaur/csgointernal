#pragma once

#include "Utils\DrawManager.h"
#include "Utils\Interfaces.h"
#include "SDK\IClientMode.h"
#include "SDK\ISurface.h"
#include "SDK\CInput.h"
#include "SDK/IVModelRender.h"
#include "SDK/IBaseClientDll.h"
#include "SDK/CPrediction.h"
#include "SDK/IGameEvent.h"
namespace vtable_indexes
{
	constexpr auto endscene = 42;
	constexpr auto reset = 16;
	constexpr auto present = 17;
	constexpr auto createMove = 24;
	constexpr auto frameStage = 37;
	constexpr auto lockCursor = 67;
	constexpr auto sceneend = 9;
	constexpr auto dme = 21;
	constexpr auto msh = 42;
	constexpr auto firegamevent = 9;
	constexpr auto paintraverse = 41;
	constexpr auto overrideview = 18;
	constexpr auto ListLeavesInBox = 6;
	constexpr auto runcommand = 19;
	constexpr auto senddatagram = 46;
}

typedef void(__thiscall* DoExtraBoneProcessingFn)(CBaseEntity*, c_studio_hdr*, Vector*, Quaternion*, const matrix3x4_t&, uint8_t*, void*);
typedef void(__thiscall* StandardBlendingRulesFn)(CBaseEntity*, c_studio_hdr*, Vector*, Quaternion*, float, int);
typedef void(__thiscall* UpdateClientSideAnimationFn)(CBaseEntity*);
typedef bool(__fastcall* ShouldSkipAnimFrameFn)();
typedef void(__thiscall* BuildTransformationsFn)(CBaseEntity*, c_studio_hdr*, Vector*, Quaternion*, const matrix3x4_t&, int, uint8_t*);
typedef bool (__thiscall* SetupBonesFn)(IClientRenderable*, matrix3x4_t*, int, int, float);
typedef void (__cdecl* CL_MoveFn)(float, bool);
typedef void(__cdecl* CL_SendMoveFn)();

extern DoExtraBoneProcessingFn oDoExtraBoneProcessing;
extern BuildTransformationsFn oBuildTransformations;
extern StandardBlendingRulesFn oStandardBlendingRules;
extern SetupBonesFn oSetupBones;
extern CL_MoveFn oCL_Move;
extern CL_SendMoveFn oCL_SendMove;


class VMTHook;
class Hooks
{
public:
	// Initialization setup, called on injection
	static void Init();
	static void Restore();

	/*---------------------------------------------*/
	/*-------------Hooked functions----------------*/
	/*---------------------------------------------*/
	//static long     __stdcall   EndScene(IDirect3DDevice9* device);
	static void     __fastcall  SceneEnd(void* thisptr, void* edx);
	static HRESULT  __stdcall   Present(IDirect3DDevice9Ex* pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion);
	static LRESULT  __stdcall   WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void InitIMGUI(IDirect3DDevice9 * pDevice);


	static bool     __fastcall  CreateMove( IClientMode*, void*, float, CUserCmd* );
	static void     __fastcall  LockCursor( ISurface*, void* );
	static HRESULT  __stdcall   Reset( IDirect3DDevice9Ex* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters );
	static void     __fastcall  DME( void*, void*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t* );
	static void     __fastcall  BeginFrame( void *thisptr, void*, float ft );
	static void     __fastcall   FrameStageNotify(void*, void*, int curStage);
	static bool	  __fastcall FireEventClientSide(void*, void*, IGameEvent* event);
	static void	  __fastcall  PaintTraverse(IPanel* pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
	static bool     __fastcall hkdWriteUsercmdDeltaToBuffer(void* ecx, void*, int slot, bf_write* buf, int from, int to, bool isnewcommand);
	static bool	  __fastcall OverrideView(void* _this, void* _edx, CViewSetup* setup);
	static int __fastcall hkListLeavesInBox(void* bsp, void* edx, Vector& mins, Vector& maxs, unsigned short* pList, int listMax);
	static void __fastcall hkRunCommand(void* ecx, void* edx, CBaseEntity* player, CUserCmd* cmd, IMoveHelper* moveHelper);
	static void __fastcall hkDoExtraBoneProcessing(CBaseEntity* player, uint32_t, c_studio_hdr* hdr, Vector* pos, Quaternion* q,const matrix3x4_t& mat, uint8_t* bone_computed, void* context);
	static void __fastcall hkBuildTransformations(CBaseEntity* player, uint32_t, c_studio_hdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& cameraTransform, int boneMask, uint8_t* boneComputed);
	static void __fastcall hkStandardBlendingRules(CBaseEntity*, uint32_t, c_studio_hdr*, Vector*, Quaternion*, float, int);
	static bool __fastcall hkSetupBones(IClientRenderable*, uint32_t, matrix3x4_t*, int, int, float);
	static void __cdecl hkCL_Move(float, bool);
	static void __cdecl hkCL_SendMove();
	static int __fastcall hkSendDatagram(NetChannel*, void*, bf_write*);


private:
	/*---------------------------------------------*/
	/*-------------VMT Hook pointers---------------*/
	/*---------------------------------------------*/
	std::unique_ptr<VMTHook> pD3DDevice9Hook;
	std::unique_ptr<VMTHook> pClientHook;
	std::unique_ptr<VMTHook> pClientModeHook;
	std::unique_ptr<VMTHook> pSurfaceHook;
	std::unique_ptr<VMTHook> pSceneEndHook;
	std::unique_ptr<VMTHook> pDMEHook;
	std::unique_ptr<VMTHook> pMaterialSystemHook;
	std::unique_ptr<VMTHook> pEventHook;
	std::unique_ptr<VMTHook> pPanelHook;
	std::unique_ptr<VMTHook> pBSPHook;
	std::unique_ptr<VMTHook> pPredictionHook;
	std::unique_ptr<VMTHook> pNetChannelHook;

	/*---------------------------------------------*/
	/*-------------Hook prototypes-----------------*/
	/*---------------------------------------------*/
	typedef long(__stdcall* EndScene_t)   (IDirect3DDevice9*);
	typedef long(__stdcall  *Reset_t)(IDirect3DDevice9Ex*, D3DPRESENT_PARAMETERS*);
	typedef long(__stdcall  *Present_t)(IDirect3DDevice9Ex*, const RECT*, const RECT*, HWND, const RGNDATA*);
	typedef void(__thiscall *SceneEnd_t)(void*);

	typedef bool( __fastcall *CreateMove_t )(IClientMode*, void*, float, CUserCmd*);
	typedef void( __fastcall *LockCursor_t )(ISurface*, void*);
	typedef void( __thiscall *DrawModelExecute_t )(void*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
	typedef void( __thiscall *BeginFrame_t )(void*, float);
	typedef void(__thiscall* FrameStageNotify_t) (void*, int);
	typedef bool(__thiscall * FireGameEvent_t)(void*, IGameEvent*);
	typedef void(__thiscall* PaintTraverse_t) (IPanel*, unsigned int, bool, bool);
	typedef bool(__thiscall* WriteUsercmdDeltaToBufferFn) (void*, int, void*, int, int, bool);
	typedef bool (__thiscall* OverrideView_t) (void*, CViewSetup*);
	typedef void(__thiscall* RunCommand_t)(void*, CBaseEntity*, CUserCmd*, IMoveHelper*);
	typedef int(__thiscall* SendDatagram_t)(NetChannel*, bf_write*);

private:
	HWND                           hCSGOWindow = nullptr; // CSGO window handle
	bool                           bInitializedDrawManager = false;   // Check if we initialized our draw manager
	WNDPROC                        pOriginalWNDProc = nullptr; // Original CSGO window proc
};

extern Hooks g_Hooks;


class VMTHook
{
public:
	VMTHook( void* ppClass )
	{
		this->ppBaseClass = static_cast<std::uintptr_t**>(ppClass);

		// loop through all valid class indexes. When it will hit invalid (not existing) it will end the loop
		while (static_cast<std::uintptr_t*>(*this->ppBaseClass)[this->indexCount])
			++this->indexCount;

		const std::size_t kSizeTable = this->indexCount * sizeof( std::uintptr_t );


		this->pOriginalVMT = *this->ppBaseClass;
		this->pNewVMT = std::make_unique<std::uintptr_t[]>( this->indexCount );

		// copy original vtable to our local copy of it
		std::memcpy( this->pNewVMT.get(), this->pOriginalVMT, kSizeTable );

		// replace original class with our local copy
		*this->ppBaseClass = this->pNewVMT.get();
	};
	~VMTHook() { *this->ppBaseClass = this->pOriginalVMT; };

	template<class Type>
	Type GetOriginal( const std::size_t index )
	{
		return reinterpret_cast<Type>(this->pOriginalVMT[index]);
	};

	HRESULT Hook( const std::size_t index, void* fnNew )
	{
		if (index > this->indexCount)   // check if given index is valid
			return E_INVALIDARG;

		this->pNewVMT[index] = reinterpret_cast<std::uintptr_t>(fnNew);
		return S_OK;
	};

	HRESULT Unhook( const std::size_t index )
	{
		if (index > this->indexCount)
			return E_INVALIDARG;

		this->pNewVMT[index] = this->pOriginalVMT[index];
		return S_OK;
	};

private:
	std::unique_ptr<std::uintptr_t[]> pNewVMT = nullptr;    // Actual used vtable
	std::uintptr_t**                  ppBaseClass = nullptr;             // Saved pointer to original class
	std::uintptr_t*                   pOriginalVMT = nullptr;             // Saved original pointer to the VMT
	std::size_t                       indexCount = 0;                     // Count of indexes inside out f-ction
};
