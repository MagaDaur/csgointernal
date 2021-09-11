#pragma once
#include "Definitions.h"
#include "IClientUnknown.h"
#include "IClientNetworkable.h"
#include "IClientEntityList.h"
#include "ClientClass.h"
#include "IMaterialSystem.h"
#include "IVModelInfoClient.h"
#include "CGlobalVarsBase.h"
#include "CInput.h"
#include "IWeaponSystem.h"
#include "CBaseCombatWeapon.h"
#include "CCSGOAnimationState.h"
#include "..\Utils\Math.h"
#include "..\Utils\Utils.h"
#include "..\Utils\NetvarManager.h"
#define TIME_TO_TICKS( dt )		( (int)( 0.5 + (float)(dt) / g_pGlobalVars->intervalPerTick ) )
#define TICKS_TO_TIME( t )		( g_pGlobalVars->intervalPerTick *( t ) )

class CBaseEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable
{
public:
	template<typename T> T GetProp(std::string name)
	{
		ClientClass* clientclass = this->GetClientClass();

		uintptr_t offset = g_Netvars.GetProp(clientclass->pNetworkName, name);

		return *(T*)(uintptr_t(this) + offset);
	}
	AnimationLayer* GetAnimLayers() {
		return *(AnimationLayer**)(DWORD(this) + 0x2990);
	}

	CBaseCombatWeapon* GetActiveWeapon() {
		return reinterpret_cast<CBaseCombatWeapon*>(g_pEntityList->GetClientEntityFromHandle(GetActiveWeaponHandle()));
	}

	CBaseEntity* GetGroundEntity()
	{
		static auto m_hGroundEntity = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_hGroundEntity");
		CBaseHandle hGroundEntity = *(CBaseHandle*)(m_hGroundEntity);
		return reinterpret_cast<CBaseEntity*>(g_pEntityList->GetClientEntityFromHandle(hGroundEntity));
	}

	PVOID UTIL_PlayerByIndex()
	{
		typedef void* (__fastcall* PlayerByIndex)(int);
		static PlayerByIndex UTIL_PlayerByIndex = (PlayerByIndex)Utils::FindSignature("server.dll", "85 C9 7E 2A A1");
		if (!UTIL_PlayerByIndex)
			return nullptr;
		return UTIL_PlayerByIndex(this->EntIndex());
	}

	std::string GetName()
	{
		PlayerInfo_t pinfo;
		g_pEngine->GetPlayerInfo(this->EntIndex(), &pinfo);
		return pinfo.szName;
	}

	void DrawServerBox(float flDuration, bool bMonoColor)
	{
		PVOID pEntity = UTIL_PlayerByIndex();
		if (!pEntity)
			return;
		static DWORD Draw = Utils::FindSignature("server.dll", "55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE");
		__asm
		{
			pushad
			movss xmm1, flDuration
			push bMonoColor
			mov ecx, pEntity
			call Draw
			popad
		}
	}

	float& GetVelocityModifier()
	{
		static auto m_flVelocityModifier = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_flVelocityModifier");
		return *(float*)(DWORD(this) + m_flVelocityModifier);
	}

	void SetModelIndex(int model)
	{
		return Utils::CallVFunc<75, void>(this, model);
	}

	uint32_t& GetReadableBones()
	{
		return *(uint32_t*)((DWORD)this + 0x26AC);
	}

	uint32_t& GetWritableBones()
	{
		return *(uint32_t*)((DWORD)this + 0x26B0);
	}

	matrix3x4_t*& GetBoneArrayForWrite()
	{
		return *(matrix3x4_t**)((DWORD)this + 0x26A8);
	}

	float SpawnTime() {
		static auto m_flSpawnTime = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_flSpawnTime");
		return *(float*)(DWORD(this) + m_flSpawnTime);
	}

	int& GetEffects()
	{
		static auto offset = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_fEffects");
		return *(int*)(DWORD(this) + offset);
	}

	bool IsPlayer() {
		typedef bool(__thiscall* IsPlayerfn)(void*);
		return Utils::GetVFunc<IsPlayerfn>(this, 157)(this);
	}

	bool isScoped() {
		static auto m_bIsScoped = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_bIsScoped");
		return *(bool*)(uintptr_t(this) + m_bIsScoped);
	}

	void ClientAnimations(bool toggle) {
		static auto m_bClientAnimation = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_bClientSideAnimation");
		*(bool*)((DWORD)this + m_bClientAnimation) = toggle;
	}

	void UpdateClientAnimation()
	{
		Utils::GetVFunc<void(__thiscall*)(void*)>(this, 223)(this);
	}

	float* GetPoseParameters() {
		static auto m_flPoseParameter = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_flPoseParameter");
		return (float*)((DWORD)this + m_flPoseParameter);
	}
private:
	DWORD GetActiveWeaponHandle() {
		static auto m_hActiveWeapon = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_hActiveWeapon");
		return *(DWORD*)(uintptr_t(this) + m_hActiveWeapon);
	}
public:
	float& GetLowerBodyYaw() {
		return *(float*)(DWORD(this) + 0x3A90);
	}

	int GetTeam() {
		static auto m_iTeamNum = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_iTeamNum");
		return *(int*)(uintptr_t(this) + m_iTeamNum);
	}
	bool bHasHeavyArmor() {
		static auto m_bHasHeavyArmor = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_bHasHeavyArmor");
		return *(bool*)(uintptr_t(this) + m_bHasHeavyArmor);
	}

	int& GetFlags() {
		static auto m_fFlags = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_fFlags");
		return *(int*)((DWORD)this + m_fFlags);
	}

	float& GetFlashMaxAlpha() {
		static auto m_flFlashMaxAlpha = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_flFlashMaxAlpha");
		return *(float*)((DWORD)this + m_flFlashMaxAlpha);
	}

	int GetMoney() {
		static auto m_iAccount = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_iAccount");
		return *(int*)(uintptr_t(this) + m_iAccount);
	}

	QAngle& GetAngEyeAngles() {
		static auto m_angEyeAngles = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_angEyeAngles");
		return *(QAngle*)(DWORD(this) + m_angEyeAngles);
	}

	Vector& GetVelocity()
	{
		static auto m_vecVelocity = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_vecVelocity[0]");
		return *(Vector*)((DWORD)this + 0x114);
	}

	int HitBoxSet() {
		static auto m_nHitBoxSet = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_nHitboxSet");
		return *(int*)(uintptr_t(this) + m_nHitBoxSet);
	}

	int GetMoveType()
	{
		static auto m_Movetype = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_nRenderMode") + 1;
		return *(int*)(uintptr_t(this) + m_Movetype);
	}

	bool GetLifeState()
	{
		static auto m_lifeState = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_lifeState");
		return *(bool*)(uintptr_t(this) + m_lifeState);
	}

	int GetHealth()
	{
		static auto m_iHealth = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_iHealth");
		return *(int*)(uintptr_t(this) + m_iHealth);
	}

	bool IsAlive() 
	{ 
		return (this->GetHealth() > 0 && this->GetLifeState() == 0); 
	}

	bool IsImmune()
	{
		static auto m_bGunGameImmunity = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_bGunGameImmunity");
		return *(bool*)(uintptr_t(this) + m_bGunGameImmunity);
	}

	int& GetTickBase()
	{
		static auto m_nTickBase = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_nTickBase");
		return *(int*)((DWORD)(this) + m_nTickBase);
	}

	float GetSimTime() {
		static auto m_flSimulationtime = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_flSimulationTime");
		return *(float*)(uintptr_t(this) + m_flSimulationtime);
	}

	float GetOldSimTime() {
		static auto m_flSimulationtime = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_flSimulationTime") + 0x4;
		return *(float*)(uintptr_t(this) + m_flSimulationtime);
	}

	Vector& GetOrigin()
	{
		static auto m_vecOrigin = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_vecOrigin");
		return *(Vector*)(uintptr_t(this) + m_vecOrigin);
	}

	Vector GetViewOffset()
	{
		static auto m_vecViewOffset = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_vecViewOffset[0]");
		return *(Vector*)(uintptr_t(this) + m_vecViewOffset);
	}

	QAngle GetPunchAngles()
	{ 
		static auto m_aimPunchAngle = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_aimPunchAngle");
		return *(QAngle*)(uintptr_t(this) + m_aimPunchAngle);
	}

	Vector GetEyePosition()
	{ 
		return this->GetOrigin() + this->GetViewOffset(); 
	}

	int GetArmor()
	{
		static auto m_ArmorValue = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_ArmorValue");
		return *(int*)(uintptr_t(this) + m_ArmorValue);
	}

	int HeavyArmor() {
		static auto m_bHasHeavyArmor = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_bHasHeavyArmor");
		return *(int*)(uintptr_t(this) + m_bHasHeavyArmor);
	}

	bool HasHelmet()
	{
		static auto m_bHasHelmet = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_bHasHelmet");
		return *(bool*)(uintptr_t(this) + m_bHasHelmet);
	}

	float NextAttack() {
		static auto m_flNextAttack = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_flNextAttack");
		return *(float*)((uintptr_t)this + m_flNextAttack);
	}

	int ChokedTicks() {
		return TIME_TO_TICKS(this->GetSimTime() - this->GetOldSimTime());
	}

	CCSGOAnimState* GetAnimState() {
		static auto offset = *(uintptr_t*)(Utils::FindSignature("client.dll", "8B 8E ? ? ? ? EB 39") + 0x2);
		return *(CCSGOAnimState**)(uintptr_t(this) + offset);
	}
	void SetAbsAngles(QAngle angles)
	{
		typedef void(__thiscall* SetAbsAnglesFn)(CBaseEntity*, const QAngle & angles);
		static auto SetAbsAngles = (SetAbsAnglesFn)(Utils::FindSignature("client.dll", "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"));

		SetAbsAngles(this, angles);
	}
	void SetAbsOrigin(Vector origin)
	{
		typedef void(__thiscall* SetAbsOriginFn)(void*, const Vector & origin);
		static auto SetAbsOrigin = (SetAbsOriginFn)Utils::FindSignature("client.dll", "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8 ? ? ? ? 8B 7D");

		SetAbsOrigin(this, origin);
	}
	QAngle& GetAbsAngles() {
		return Utils::CallVFunc<11, QAngle&>(this);
	}
	Vector& GetAbsOrigin() {
		return Utils::CallVFunc<10, Vector&>(this);
	}
};