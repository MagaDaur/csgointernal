#include <thread>
#include <fstream>
#include <intrin.h>

#include "Hooks.h"
#include "Settings.h"
#include "Utils\Utils.h"

#include "SDK/IPanel.h"
#include "SDK/Studio.h"
#include "SDK/DebugOverlay.h"
#include "SDK/IVEngineClient.h"
#include "SDK/IRenderView.h"
#include "SDK/IVModelRender.h"
#include "SDK/IMaterialSystem.h"
#include "SDK/IVModelInfoClient.h"
#include "SDK/IClientEntityList.h"
#include "SDK/CBaseCombatWeapon.h"

#include "Features/legitbot/legitbot.h"
#include "Features/misc/misc.h"
#include "Features/lagcomp/lagcomp.h"
#include "Features/visuals/visuals.h"
#include "Features/engine prediction/engine prediction.h"
#include "Features/visuals/chams.h"
#include "Features/antiaim/antiaim.h"

#include "Menu.h"
#include "GUI/ImGui/imgui.h"
#include "GUI/ImGui/dx9/imgui_dx9.h"

#include <detours.h>

Settings g_Settings;
Hooks g_Hooks;
NetvarTree g_Netvars;

extern IMGUI_API LRESULT ImGui_ImplDX9_WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam);
void WriteUserCmd(void* buf, CUserCmd* to, CUserCmd* from);

DoExtraBoneProcessingFn oDoExtraBoneProcessing;
BuildTransformationsFn oBuildTransformations;
StandardBlendingRulesFn oStandardBlendingRules;
SetupBonesFn oSetupBones;
CL_MoveFn oCL_Move;
CL_SendMoveFn oCL_SendMove;
CheckForSequenceChangeFn oCheckForSequenceChange;
UpdateClientSideAnimationFn oUpdateClientSideAnimation;

void Hooks::Init()
{
	// Get window handle
	while (!(g_Hooks.hCSGOWindow = FindWindowA( "Valve001", nullptr )))
	{
		using namespace std::literals::chrono_literals;
		std::this_thread::sleep_for( 50ms );
	}

	{
		interfaces::Init();
		g_Netvars.Init();
	}

	Utils::Log( "Hooking in progress..." );

	// D3D Device pointer
	const uintptr_t d3dDevice = **reinterpret_cast<uintptr_t**>(Utils::FindSignature( "shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C" ) + 1);

	if (g_Hooks.hCSGOWindow)        // Hook WNDProc to capture mouse / keyboard input
		g_Hooks.pOriginalWNDProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr( g_Hooks.hCSGOWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Hooks::WndProc) ));

	// VMTHooks
	g_Hooks.pClientHook = std::make_unique<VMTHook>(g_pClientDll);
	g_Hooks.pD3DDevice9Hook = std::make_unique<VMTHook>( reinterpret_cast<void*>(d3dDevice) );
	g_Hooks.pClientModeHook = std::make_unique<VMTHook>( g_pClientMode );
	g_Hooks.pSurfaceHook = std::make_unique<VMTHook>( g_pSurface );
	g_Hooks.pSceneEndHook = std::make_unique<VMTHook>( g_pRenderView );
	g_Hooks.pDMEHook = std::make_unique<VMTHook>( g_MdlRender );
	g_Hooks.pMaterialSystemHook = std::make_unique<VMTHook>( g_MatSystem );
	g_Hooks.pEventHook = std::make_unique<VMTHook>(g_pEventManager);
	g_Hooks.pPanelHook = std::make_unique<VMTHook>(g_pPanel);
	g_Hooks.pBSPHook = std::make_unique<VMTHook>(g_pEngine->GetBSPTreeQuery());
	g_Hooks.pPredictionHook = std::make_unique<VMTHook>(g_pPrediction);

	// Hook the table functions
	g_Hooks.pD3DDevice9Hook->Hook(vtable_indexes::reset, Hooks::Reset); /*/ ImGui /*/
	g_Hooks.pEventHook->Hook(vtable_indexes::firegamevent, Hooks::FireEventClientSide);
	g_Hooks.pClientHook->Hook(vtable_indexes::frameStage, Hooks::FrameStageNotify);
	g_Hooks.pClientModeHook->Hook(vtable_indexes::createMove, Hooks::CreateMove);
	g_Hooks.pClientHook->Hook(24, Hooks::hkdWriteUsercmdDeltaToBuffer);
	g_Hooks.pClientModeHook->Hook(vtable_indexes::overrideview, Hooks::OverrideView);
	g_Hooks.pD3DDevice9Hook->Hook( vtable_indexes::reset, Hooks::Reset );
	g_Hooks.pD3DDevice9Hook->Hook( vtable_indexes::present, Hooks::Present );
	g_Hooks.pSurfaceHook->Hook( vtable_indexes::lockCursor, Hooks::LockCursor );
	g_Hooks.pSceneEndHook->Hook( vtable_indexes::sceneend, Hooks::SceneEnd );
	g_Hooks.pBSPHook->Hook(vtable_indexes::ListLeavesInBox, Hooks::hkListLeavesInBox);
	g_Hooks.pDMEHook->Hook( vtable_indexes::dme, Hooks::DME );
	g_Hooks.pMaterialSystemHook->Hook( vtable_indexes::msh, Hooks::BeginFrame );
	g_Hooks.pPanelHook->Hook(vtable_indexes::paintraverse, Hooks::PaintTraverse);
	g_Hooks.pPredictionHook->Hook(vtable_indexes::runcommand, Hooks::hkRunCommand);

	DWORD* dwDoExtraBoneProcessing = (DWORD*)Utils::FindSignature("client.dll", "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C");
	oDoExtraBoneProcessing = (DoExtraBoneProcessingFn)DetourFunction((PBYTE)dwDoExtraBoneProcessing, (PBYTE)Hooks::hkDoExtraBoneProcessing);

	DWORD* dwBuildTransformations = (DWORD*)Utils::FindSignature("client.dll", "55 8B EC 83 E4 F0 81 EC ? ? ? ? 56 57 8B F9 8B 0D ? ? ? ? 89 7C 24 28");
	oBuildTransformations = (BuildTransformationsFn)DetourFunction((PBYTE)dwBuildTransformations, (PBYTE)Hooks::hkBuildTransformations);

	DWORD* dwStandardBlendingRules = (DWORD*)Utils::FindSignature("client.dll", "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6");
	oStandardBlendingRules = (StandardBlendingRulesFn)DetourFunction((PBYTE)dwStandardBlendingRules, (PBYTE)Hooks::hkStandardBlendingRules);

	DWORD* dwCL_Move = (DWORD*)Utils::FindSignature("engine.dll", "55 8B EC 81 EC ? ? ? ? 53 56 8A F9");
	oCL_Move = (CL_MoveFn)DetourFunction((PBYTE)dwCL_Move, (PBYTE)Hooks::hkCL_Move);

	DWORD* dwCL_SendMove = (DWORD*)Utils::FindSignature("engine.dll", "55 8B EC 8B 4D 04 81");
	oCL_SendMove = (CL_SendMoveFn)DetourFunction((PBYTE)dwCL_SendMove, (PBYTE)Hooks::hkCL_SendMove);

	DWORD* dwCheckForSequenceChange = (DWORD*)Utils::FindSignature("client.dll", "55 8B EC 51 53 8B 5D 08 56 8B F1 57 85");
	oCheckForSequenceChange = (CheckForSequenceChangeFn)DetourFunction((PBYTE)dwCheckForSequenceChange, (PBYTE)Hooks::hkCheckForSequenceChange);

	DWORD* dwUpdateClientSideAnimation = (DWORD*)Utils::FindSignature("client.dll", "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74");
	oUpdateClientSideAnimation = (UpdateClientSideAnimationFn)DetourFunction((PBYTE)dwUpdateClientSideAnimation, (PBYTE)Hooks::hkUpdateClientSideAnimation);
	
	g_pClientState->iDeltaTick = -1;

	Utils::Log( "Hooking completed!" );
}
void Hooks::Restore()
{
	Utils::Log( "Unhooking in progress..." );
	{  

		DetourRemove((PBYTE)oDoExtraBoneProcessing, (PBYTE)Hooks::hkDoExtraBoneProcessing);
		DetourRemove((PBYTE)oBuildTransformations, (PBYTE)Hooks::hkBuildTransformations);
		DetourRemove((PBYTE)oStandardBlendingRules, (PBYTE)Hooks::hkStandardBlendingRules);
		DetourRemove((PBYTE)oCL_Move, (PBYTE)Hooks::hkCL_Move);
		DetourRemove((PBYTE)oCL_SendMove, (PBYTE)Hooks::hkCL_SendMove);
		DetourRemove((PBYTE)oCheckForSequenceChange, (PBYTE)Hooks::hkCheckForSequenceChange);
		DetourRemove((PBYTE)oUpdateClientSideAnimation, (PBYTE)Hooks::hkUpdateClientSideAnimation);

		g_Hooks.pEventHook->Unhook(vtable_indexes::firegamevent);
		g_Hooks.pClientHook->Unhook(24);
		g_Hooks.pClientModeHook->Unhook(vtable_indexes::overrideview);
		g_Hooks.pPanelHook->Unhook(vtable_indexes::paintraverse);
		g_Hooks.pClientModeHook->Unhook( vtable_indexes::createMove );
		g_Hooks.pClientHook->Unhook(vtable_indexes::frameStage);
		g_Hooks.pD3DDevice9Hook->Unhook( vtable_indexes::reset );
		g_Hooks.pD3DDevice9Hook->Unhook( vtable_indexes::present );
		g_Hooks.pSurfaceHook->Unhook( vtable_indexes::lockCursor );
		g_Hooks.pSceneEndHook->Unhook(vtable_indexes::sceneend);
		g_Hooks.pDMEHook->Unhook(vtable_indexes::dme);
		g_Hooks.pMaterialSystemHook->Unhook(vtable_indexes::msh);
		g_Hooks.pBSPHook->Unhook(vtable_indexes::ListLeavesInBox);
		g_Hooks.pPredictionHook->Unhook(vtable_indexes::runcommand);

		SetWindowLongPtr( g_Hooks.hCSGOWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_Hooks.pOriginalWNDProc) );
	}
	Utils::Log( "Unhooking succeded!" );

	g_Render.InvalidateDeviceObjects();
	g_Fonts.DeleteDeviceObjects();
}

int __fastcall Hooks::hkSendDatagram(NetChannel* netchannel, void*, bf_write* datagram)
{
	static auto oSendDatagram = g_Hooks.pNetChannelHook->GetOriginal<SendDatagram_t>(vtable_indexes::senddatagram);
	int ret = oSendDatagram(netchannel, datagram);
	return ret;
}

void __cdecl Hooks::hkCL_Move(float time, bool finaltick)
{
	oCL_Move(time, finaltick);
}

void __cdecl Hooks::hkCL_SendMove()
{
	oCL_SendMove();
}

void __fastcall Hooks::hkUpdateClientSideAnimation(CBaseEntity* player, void* edx)
{
	if (g_LagComp.bShouldAnimate || player->IsLocalPlayer())
	{
		oUpdateClientSideAnimation(player);

		g_LagComp.bShouldAnimate = false;
	}
}

void __fastcall Hooks::hkRunCommand(void* ecx, void* edx, CBaseEntity* player, CUserCmd* cmd, IMoveHelper* moveHelper) {
	static auto oRunCommand = g_Hooks.pPredictionHook->GetOriginal<RunCommand_t>(vtable_indexes::runcommand);

	oRunCommand(ecx, player, cmd, moveHelper);
}

int __fastcall Hooks::hkListLeavesInBox(void* bsp, void* edx, Vector& mins, Vector& maxs, unsigned short* pList, int listMax) {
	typedef int(__thiscall* ListLeavesInBox)(void*, const Vector&, const Vector&, unsigned short*, int);
	static auto ofunc = g_Hooks.pBSPHook->GetOriginal< ListLeavesInBox >(vtable_indexes::ListLeavesInBox);
	CBaseEntity* local = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
	if (*(uint32_t*)_ReturnAddress() != 0x14244489)
		return ofunc(bsp, mins, maxs, pList, listMax);

	static const auto MAX_COORD_FLOAT = (16384.0f);
	static const auto  MIN_COORD_FLOAT = (-MAX_COORD_FLOAT);

	static struct RenderableInfo_t {
		IClientRenderable* m_pRenderable;
		void* m_pAlphaProperty;
		int m_EnumCount;
		int m_nRenderFrame;
		unsigned short m_FirstShadow;
		unsigned short m_LeafList;
		short m_Area;
		uint16_t m_Flags;   // 0x0016
		uint16_t m_Flags2; // 0x0018
		Vector m_vecBloatedAbsMins;
		Vector m_vecBloatedAbsMaxs;
		Vector m_vecAbsMins;
		Vector m_vecAbsMaxs;
		int pad;
	};

	auto addrofretaddr = _AddressOfReturnAddress();
	if (!addrofretaddr)
		return ofunc(bsp, mins, maxs, pList, listMax);

	auto info = *(RenderableInfo_t**)((uintptr_t)addrofretaddr + 0x14);
	if (!info || !info->m_pRenderable)
		return ofunc(bsp, mins, maxs, pList, listMax);

	auto base_entity = info->m_pRenderable->GetIClientUnknown()->GetBaseEntity();
	//if (!base_entity || !base_entity->IsPlayer())
		//return ofunc(bsp, mins, maxs, pList, listMax);

	info->m_Flags &= ~0x100;
	info->m_Flags2 |= 0xC0;

	static const Vector map_min = Vector(MIN_COORD_FLOAT, MIN_COORD_FLOAT, MIN_COORD_FLOAT);
	static const Vector map_max = Vector(MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT);
	auto count = ofunc(bsp, map_min, map_max, pList, listMax);
	return count;
}

bool __fastcall Hooks::OverrideView(void* _this, void* _edx, CViewSetup* setup) {
	static auto oOverrideView = g_Hooks.pClientModeHook->GetOriginal<OverrideView_t>(vtable_indexes::overrideview);

	return oOverrideView(_this, setup);
}

bool __fastcall Hooks::hkSetupBones(IClientRenderable* rplayer, uint32_t edx, matrix3x4_t* matrix, int numbones, int mask, float curtime)
{
	return oSetupBones(rplayer, matrix, numbones, mask, curtime);
}

void __fastcall Hooks::hkStandardBlendingRules(CBaseEntity* player, uint32_t edx, c_studio_hdr* hdr, Vector* pos, Quaternion* q, float curtime, int mask)
{
	player->GetEffects() |= 0x8;

	oStandardBlendingRules(player, hdr, pos, q, curtime, mask);

	player->GetEffects() &= ~0x8;
}

void __fastcall Hooks::hkBuildTransformations(CBaseEntity* player, uint32_t edx, c_studio_hdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& cameraTransform, int boneMask, uint8_t* boneComputed)
{
	player->JiggleEnabled() = false;

	oBuildTransformations(player, hdr, pos, q, cameraTransform, boneMask, boneComputed);
}

void __fastcall Hooks::hkCheckForSequenceChange(void* ecx, void* edx, void* hdr, int nCurSequence, bool bForceNewSequence, bool interpolate)
{
	oCheckForSequenceChange(ecx, hdr, nCurSequence, bForceNewSequence, false);
}

void __fastcall Hooks::hkDoExtraBoneProcessing(CBaseEntity* player, uint32_t edx, c_studio_hdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& mat, uint8_t* bone_computed, void* context)
{
	return;
}

void __fastcall Hooks::PaintTraverse(IPanel* panel, int edx, unsigned int vguiPanel, bool forcerepaint, bool allowforce) {
	static auto oPaintTraverse = g_Hooks.pPanelHook->GetOriginal<PaintTraverse_t>(vtable_indexes::paintraverse);

	oPaintTraverse(panel, vguiPanel, forcerepaint, allowforce);
}

bool __fastcall Hooks::CreateMove(void* ecx, void* edx, float sample_frametime, CUserCmd* cmd)
{
	static auto oCreateMove = g_Hooks.pClientModeHook->GetOriginal<CreateMove_t>(24);

	bool ret = oCreateMove(ecx, sample_frametime, cmd);

	if (!cmd || !cmd->command_number) return ret;

	uintptr_t* fptr; __asm mov fptr, ebp
	bool* bSendPackets = (bool*)(*fptr - 0x1C);

	if (!bSendPackets) return ret;

	*bSendPackets = true;

	g_Misc.OnPrePrediction(cmd, bSendPackets);

	engine_prediction::RunEnginePred(cmd); {

		g_LegitBot.OnPrediction(cmd);

		g_RageBot.OnPrediction(cmd, bSendPackets);

		g_Misc.OnPrediction();

		g_AntiAim.OnPrediction(cmd, bSendPackets);

		g_RageBot.OnPostPrediction(bSendPackets);

	} engine_prediction::EndEnginePred();

	return false;
}

bool __fastcall Hooks::hkdWriteUsercmdDeltaToBuffer(void* ecx, void* edx, int slot, bf_write* buf, int from, int to, bool isnewcommand)
{
	static auto oWriteUserCmdDeltaToBuffer = g_Hooks.pClientHook->GetOriginal <WriteUsercmdDeltaToBufferFn>(24);
	static ConVar* sv_maxusrcmdprocessticks = g_pCvar->FindVar("sv_maxusrcmdprocessticks");


	if(from == -1)
	{

		if (g_RageBot.Exploits.iShiftAmount > 0 && g_RageBot.Exploits.bCharged)
		{
			g_pClientState->nChokedCommands += g_RageBot.Exploits.iShiftAmount;
			
			CUserCmd toCmd = g_RageBot.Exploits.lastcmd;
			CUserCmd fromCmd;

			for (int i = 0; i < g_RageBot.Exploits.iShiftAmount; i++)
			{
				WriteUserCmd(buf, &toCmd, &fromCmd);

				Hooks::CreateMove(g_pClientMode, NULL, 0.f, &fromCmd);

				fromCmd = toCmd;
				toCmd.tick_count++;
				toCmd.command_number++;
			}

			g_RageBot.Exploits.iShiftAmount = 0;
			g_RageBot.Exploits.bCharged = false;
			g_RageBot.Exploits.bShouldCharge = true;
		}

		int* pNumBackupCommands = (int*)(reinterpret_cast<uintptr_t>(buf) - 0x30);
		int* pNumNewCommands = (int*)(reinterpret_cast<uintptr_t>(buf) - 0x2C);

		*pNumNewCommands = std::clamp(g_pClientState->nChokedCommands + 1, 0, sv_maxusrcmdprocessticks->GetInt());
		*pNumBackupCommands = 0;

		int nextcommandnr = g_pClientState->iLastOutgoingCommand + 1 + *pNumNewCommands;

		for(int to = g_pClientState->iLastOutgoingCommand + 1; to <= nextcommandnr; ++to)
		{
			bool isnewcmd = to >= nextcommandnr;

			oWriteUserCmdDeltaToBuffer(ecx, 0, buf, from, to, isnewcmd);
			from = to;
		}
	}

	return true;
}

bool __fastcall Hooks::FireEventClientSide(void* ecx, void* edx, IGameEvent* event) {
	static auto oFunc = g_Hooks.pEventHook->GetOriginal<FireGameEvent_t>(vtable_indexes::firegamevent);

	return oFunc(ecx, event);
}

void __fastcall Hooks::FrameStageNotify(void* ecx, void* edx, int curStage)
{
	static auto oFrameStage = g_Hooks.pClientHook->GetOriginal<FrameStageNotify_t>(vtable_indexes::frameStage);

	g_AntiAim.OnFrameStage(curStage);
	g_LagComp.OnFrameStage(curStage);

	oFrameStage(ecx, curStage);
}

void __fastcall Hooks::DME(void* ecx, void* edx, IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& render_info, matrix3x4_t* matrix)
{
	static auto oDME = g_Hooks.pDMEHook->GetOriginal<DrawModelExecute_t>(vtable_indexes::dme);

	CBaseEntity* player = g_pEntityList->GetClientEntity(render_info.entity_index);
	if (player && player->IsPlayer() && !player->IsLocalPlayer())
	{
		auto records = g_LagComp.GetRecords(player);
		if (records->size() > 1)
		{
			oDME(ecx, context, state, render_info, records->front().matrix);

			oDME(ecx, context, state, render_info, records->back().matrix);
		}
	}

	oDME(ecx, context, state, render_info, matrix);
}

void __fastcall Hooks::SceneEnd( void* thisptr, void* edx )
{
	static auto oSceneEnd = g_Hooks.pSceneEndHook->GetOriginal<SceneEnd_t>( vtable_indexes::sceneend );

	return oSceneEnd(thisptr);
}

void __fastcall Hooks::BeginFrame( void *thisptr, void*, float ft )
{
	static auto oBeginFrame = g_Hooks.pMaterialSystemHook->GetOriginal<BeginFrame_t>( vtable_indexes::msh );

	oBeginFrame( thisptr, ft );
}

void Hooks::InitIMGUI(IDirect3DDevice9* pDevice) 
{
	ImGui_ImplDX9_Init(g_Hooks.hCSGOWindow, pDevice);
	ImGuiStyle& style = ImGui::GetStyle();
	auto& Colors = style.Colors;
	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Calibri.ttf", 14);

	style.WindowRounding = 2.0;
	style.WindowTitleAlign = ImVec2(0.5f, 0.84f);
	style.FrameRounding = 2.0;
	style.ItemSpacing = ImVec2(5.0, 4.0);
	style.ScrollbarSize = 13.0;
	style.ScrollbarRounding = 0;
	style.GrabMinSize = 8.0;
	style.GrabRounding = 1.0;

	style.Colors[ImGuiCol_Text] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.26f, 0.26f, 0.26f, 0.95f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
}

void __fastcall Hooks::LockCursor(ISurface* thisptr, void* edx)
{
	static auto oLockCursor = g_Hooks.pSurfaceHook->GetOriginal<LockCursor_t>(vtable_indexes::lockCursor);

	if (!g_Settings.bMenuOpened)
		return oLockCursor(thisptr, edx);

	g_pSurface->UnlockCursor();
}

HRESULT __stdcall Hooks::Reset(IDirect3DDevice9Ex* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	static auto oReset = g_Hooks.pD3DDevice9Hook->GetOriginal<Reset_t>(vtable_indexes::reset);

	if (g_Hooks.bInitializedDrawManager)
	{
		g_Render.InvalidateDeviceObjects();
		ImGui_ImplDX9_InvalidateDeviceObjects();
		HRESULT hr = oReset(pDevice, pPresentationParameters);
		g_Render.RestoreDeviceObjects(pDevice);
		ImGui_ImplDX9_CreateDeviceObjects();
		return hr;
	}

	return oReset(pDevice, pPresentationParameters);
}

HRESULT __stdcall Hooks::Present( IDirect3DDevice9Ex* pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion )
{
	IDirect3DStateBlock9* stateBlock = nullptr;
	IDirect3DVertexDeclaration9* vertDec = nullptr;

	pDevice->GetVertexDeclaration(&vertDec);
	pDevice->CreateStateBlock(D3DSBT_PIXELSTATE, &stateBlock);

	[pDevice]()
	{
		if (!g_Hooks.bInitializedDrawManager)
		{
			g_Render.InitDeviceObjects( pDevice );
			g_Render.InitDeviceObjects(pDevice);
			g_Hooks.InitIMGUI(pDevice);
			g_Hooks.bInitializedDrawManager = true;
		}
		else
		{
			g_Render.SetupRenderStates(); // Sets up proper render states for our state block

			ImGui_ImplDX9_NewFrame();

			if (g_Settings.bMenuOpened) {
				
				g_Menu.Render();
			}
		}
	}();

	stateBlock->Apply();
	stateBlock->Release();
	pDevice->SetVertexDeclaration(vertDec);

	static auto oPresent = g_Hooks.pD3DDevice9Hook->GetOriginal<Present_t>( 17 );
	return oPresent( pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion );
}

LRESULT Hooks::WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	g_Menu.Menu_Manipulation();
	if (g_Settings.bMenuOpened && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProcA(g_Hooks.pOriginalWNDProc, hWnd, uMsg, wParam, lParam);
}

void WriteUserCmd(void* buf, CUserCmd* to, CUserCmd* from)
{
	using WriteUsercmd_t = void(__fastcall*) (bf_write*, CUserCmd*, CUserCmd*);
	static WriteUsercmd_t WriteUsercmdF = (WriteUsercmd_t)Utils::FindSignature("client.dll", "55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D");
	__asm
	{
		mov ecx, buf;
		mov edx, to;
		push from;
		call WriteUsercmdF;
		add esp, 4;
	}
}

