#include "CBaseEntity.h"
#include "ClientClass.h"
#include "IMaterialSystem.h"
#include "IVModelInfoClient.h"
#include "CGlobalVarsBase.h"
#include "CInput.h"
#include "IWeaponSystem.h"
#include "CBaseCombatWeapon.h"
#include "CCSGOAnimationState.h"
#include "IClientEntityList.h"

template<typename T> T CBaseEntity::GetProp(std::string name)
{
	ClientClass* clientclass = this->GetClientClass();

	uintptr_t offset = g_Netvars.GetProp(clientclass->pNetworkName, name);

	return *(T*)(uintptr_t(this) + offset);
}

uintptr_t CBaseEntity::GetOffset(std::string name)
{
	ClientClass* clientclass = GetClientClass();

	return g_Netvars.GetProp(clientclass->pNetworkName, name);
}

float& CBaseEntity::GetDuckAmount()
{
	static uintptr_t m_flDuckAmount = GetOffset("m_flDuckAmount");

	return *(float*)(uintptr_t(this) + m_flDuckAmount);
}

bool& CBaseEntity::JiggleEnabled()
{
	static auto m_bJiggleBones = 0x2930; // find sig!!!
	return *(bool*)(uintptr_t(this) + m_bJiggleBones);
}

AnimationLayer* CBaseEntity::GetAnimLayers() {
	return *(AnimationLayer**)(uintptr_t(this) + 0x2990); // find sig!!!
}

CBaseCombatWeapon* CBaseEntity::GetActiveWeapon() {
	return Utils::CallVFunc<268, CBaseCombatWeapon*>(this);
}

CBaseEntity* CBaseEntity::GetGroundEntity()
{
	static auto m_hGroundEntity = GetOffset("m_hGroundEntity");
	CBaseHandle hGroundEntity = *(CBaseHandle*)(uintptr_t(this) + m_hGroundEntity);
	return reinterpret_cast<CBaseEntity*>(g_pEntityList->GetClientEntityFromHandle(hGroundEntity));
}

void* CBaseEntity::UTIL_PlayerByIndex()
{
	typedef void* (__fastcall* PlayerByIndex)(int);
	static PlayerByIndex UTIL_PlayerByIndex = (PlayerByIndex)Utils::FindSignature("server.dll", "85 C9 7E 32 A1 ? ? ? ?");

	return UTIL_PlayerByIndex(EntIndex());
}

std::string CBaseEntity::GetName()
{
	PlayerInfo_t pinfo;
	g_pEngine->GetPlayerInfo(EntIndex(), &pinfo);
	return pinfo.szName;
}

void CBaseEntity::DrawServerBox(float flDuration, bool bMonoColor)
{
	static uintptr_t Draw = Utils::FindSignature("server.dll", "55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE");
	void* pEntity = UTIL_PlayerByIndex();
	if(pEntity)
	{
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
}

float& CBaseEntity::GetVelocityModifier()
{
	static auto m_flVelocityModifier = GetOffset("m_flVelocityModifier");
	return *(float*)(uintptr_t(this) + m_flVelocityModifier);
}

void CBaseEntity::SetModelIndex(int model)
{
	return Utils::CallVFunc<75, void>(this, model);
}

matrix3x4_t*& CBaseEntity::GetBoneMatrix()
{
	return *(matrix3x4_t**)(uintptr_t(this) + 0x26A8); // should find sig!!
}

float CBaseEntity::SpawnTime() {
	static auto m_flSpawnTime = GetOffset("m_flSpawnTime");
	return *(float*)(uintptr_t(this) + m_flSpawnTime);
}

int& CBaseEntity::GetEffects()
{
	static auto m_fEffects = GetOffset("m_fEffects");
	return *(int*)(uintptr_t(this) + m_fEffects);
}

bool CBaseEntity::IsPlayer() {
	return Utils::CallVFunc<158, bool>(this);
}

bool CBaseEntity::IsScoped() {
	static auto m_bIsScoped = GetOffset("m_bIsScoped");
	return *(bool*)(uintptr_t(this) + m_bIsScoped);
}

bool& CBaseEntity::ClientAnimations() {
	static auto m_bClientAnimation = GetOffset("m_bClientSideAnimation");
	return *(bool*)((uintptr_t)this + m_bClientAnimation);
}

void CBaseEntity::UpdateClientAnimation()
{
	Utils::CallVFunc<224>(this);
}

float* CBaseEntity::GetPoseParameters() {
	static auto m_flPoseParameter = GetOffset("m_flPoseParameter");
	return (float*)((uintptr_t)this + 10100); // propal iz propov, hz, nado sig!
}

float& CBaseEntity::GetLowerBodyYaw() {
	static auto m_flLowerBodyYawTarget = GetOffset("m_flLowerBodyYawTarget")
	return *(float*)(uintptr_t(this) + m_flLowerBodyYawTarget);
}

bool CBaseEntity::IsLocalPlayer()
{
	CBaseEntity* localplayer = g_pEntityList->GetLocalPlayer();
	if (!localplayer) return false;
	return this == localplayer;
}

int CBaseEntity::GetTeam() {
	static auto m_iTeamNum = GetOffset("m_iTeamNum");
	return *(int*)(uintptr_t(this) + m_iTeamNum);
}

bool CBaseEntity::bHasHeavyArmor() {
	static auto m_bHasHeavyArmor = GetOffset("m_bHasHeavyArmor");
	return *(bool*)(uintptr_t(this) + m_bHasHeavyArmor);
}

int& CBaseEntity::GetFlags() {
	static auto m_fFlags = GetOffset("m_fFlags");
	return *(int*)((uintptr_t)this + m_fFlags);
}

float& CBaseEntity::GetFlashMaxAlpha() {
	static auto m_flFlashMaxAlpha = GetOffset("m_flFlashMaxAlpha");
	return *(float*)((uintptr_t)this + m_flFlashMaxAlpha);
}

int CBaseEntity::GetMoney() {
	static auto m_iAccount = GetOffset("m_iAccount");
	return *(int*)(uintptr_t(this) + m_iAccount);
}

QAngle& CBaseEntity::GetEyeAngles() {
	static auto m_angEyeAngles = GetOffset("m_angEyeAngles");
	return *(QAngle*)(uintptr_t(this) + m_angEyeAngles);
}

Vector& CBaseEntity::GetVelocity()
{
	static auto m_vecVelocity = GetOffset("m_vecVelocity[0]");
	return *(Vector*)((uintptr_t)this + m_vecVelocity);
}

int CBaseEntity::HitBoxSet() {
	static auto m_nHitBoxSet = GetOffset("m_nHitboxSet");
	return *(int*)(uintptr_t(this) + m_nHitBoxSet);
}

int CBaseEntity::GetMoveType()
{
	static auto m_Movetype = GetOffset("m_nRenderMode") + 1;
	return *(int*)(uintptr_t(this) + m_Movetype);
}

bool CBaseEntity::GetLifeState()
{
	static auto m_lifeState = GetOffset("m_lifeState");
	return *(bool*)(uintptr_t(this) + m_lifeState);
}

int CBaseEntity::GetHealth()
{
	static auto m_iHealth = GetOffset("m_iHealth");
	return *(int*)(uintptr_t(this) + m_iHealth);
}

bool CBaseEntity::IsAlive()
{
	return (GetHealth() > 0 && GetLifeState() == 0);
}

bool CBaseEntity::IsImmune()
{
	static auto m_bGunGameImmunity = GetOffset("m_bGunGameImmunity");
	return *(bool*)(uintptr_t(this) + m_bGunGameImmunity);
}

int& CBaseEntity::GetTickBase()
{
	static auto m_nTickBase = GetOffset("m_nTickBase");
	return *(int*)((uintptr_t)(this) + m_nTickBase);
}

float CBaseEntity::GetSimulationTime() {
	static auto m_flSimulationtime = GetOffset("m_flSimulationTime");
	return *(float*)(uintptr_t(this) + m_flSimulationtime);
}

float CBaseEntity::GetOldSimulationTime() {
	static auto m_flSimulationtime = GetOffset("m_flSimulationTime") + 0x4;
	return *(float*)(uintptr_t(this) + m_flSimulationtime);
}

float CBaseEntity::GetAnimTime()
{
	static auto m_flAnimTime = GetOffset("m_flAnimTime");
	return *(float*)(uintptr_t(this) + m_flAnimTime);
}

Vector& CBaseEntity::GetOrigin()
{
	static auto m_vecOrigin = GetOffset("m_vecOrigin");
	return *(Vector*)(uintptr_t(this) + m_vecOrigin);
}

Vector CBaseEntity::GetViewOffset()
{
	static auto m_vecViewOffset = GetOffset("m_vecViewOffset[0]");
	return *(Vector*)(uintptr_t(this) + m_vecViewOffset);
}

QAngle CBaseEntity::GetPunchAngles()
{
	static auto m_aimPunchAngle = GetOffset("m_aimPunchAngle");
	return *(QAngle*)(uintptr_t(this) + m_aimPunchAngle);
}

Vector CBaseEntity::GetEyePosition()
{
	Vector ret;
	Utils::CallVFunc<169, void, Vector&>(this, ret);
	return ret;
}

int CBaseEntity::GetArmor()
{
	static auto m_ArmorValue = GetOffset("m_ArmorValue");
	return *(int*)(uintptr_t(this) + m_ArmorValue);
}

int CBaseEntity::HeavyArmor() {
	static auto m_bHasHeavyArmor = GetOffset("m_bHasHeavyArmor");
	return *(int*)(uintptr_t(this) + m_bHasHeavyArmor);
}

bool CBaseEntity::HasHelmet()
{
	static auto m_bHasHelmet = GetOffset("m_bHasHelmet");
	return *(bool*)(uintptr_t(this) + m_bHasHelmet);
}

float CBaseEntity::NextAttack() {
	static auto m_flNextAttack = GetOffset("m_flNextAttack");
	return *(float*)((uintptr_t)this + m_flNextAttack);
}

int CBaseEntity::ChokedTicks() {
	return TIME_TO_TICKS(this->GetSimulationTime() - this->GetOldSimulationTime());
}

CCSGOAnimState* CBaseEntity::GetAnimState() {
	static auto offset = *(uintptr_t*)(Utils::FindSignature("client.dll", "8B 8E ? ? ? ? EB 39") + 0x2);
	return *(CCSGOAnimState**)(uintptr_t(this) + offset);
}
void CBaseEntity::SetAbsAngles(float yaw)
{
	typedef void(__thiscall* SetAbsAnglesFn)(CBaseEntity*, const QAngle& angles);
	static auto SetAbsAngles = (SetAbsAnglesFn)(Utils::FindSignature("client.dll", "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"));

	SetAbsAngles(this, QAngle(0, yaw, 0));
}
void CBaseEntity::SetAbsOrigin(Vector origin)
{
	typedef void(__thiscall* SetAbsOriginFn)(void*, const Vector& origin);
	static auto SetAbsOrigin = (SetAbsOriginFn)Utils::FindSignature("client.dll", "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8 ? ? ? ? 8B 7D");

	SetAbsOrigin(this, origin);
}
QAngle& CBaseEntity::GetAbsAngles() {
	return Utils::CallVFunc<11, QAngle&>(this);
}
Vector& CBaseEntity::GetAbsOrigin() {
	return Utils::CallVFunc<10, Vector&>(this);
}

bool CBaseEntity::IsTeamMate()
{
	CBaseEntity* localplayer = g_pEntityList->GetLocalPlayer();
	if (!localplayer) return false;
	return localplayer->GetTeam() == GetTeam();
}