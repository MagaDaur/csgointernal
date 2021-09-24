#pragma once
#include "Definitions.h"
#include "IClientUnknown.h"
#include "IClientNetworkable.h"
#include "IClientEntity.h"

#include "..\Utils\Utils.h"
#include "..\Utils\NetvarManager.h"

class CBaseCombatWeapon;
class CCSGOAnimState;
class AnimationLayer;
class CCSGOAnimState;

#define TIME_TO_TICKS( dt )		( (int)( 0.5 + (float)(dt) / g_pGlobalVars->intervalPerTick ) )
#define TICKS_TO_TIME( t )		( g_pGlobalVars->intervalPerTick *( t ) )

class CBaseEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable
{
public:
	template<typename T> T GetProp(std::string name);
	uintptr_t GetOffset(std::string name);

	bool IsTeamMate();

	float& GetDuckAmount();

	matrix3x4_t*& GetBoneMatrix();

	bool IsLocalPlayer();

	bool& JiggleEnabled();

	AnimationLayer* GetAnimLayers();

	CBaseCombatWeapon* GetActiveWeapon();

	CBaseEntity* GetGroundEntity();

	PVOID UTIL_PlayerByIndex();

	std::string GetName();

	void DrawServerBox(float flDuration, bool bMonoColor);

	float& GetVelocityModifier();

	void SetModelIndex(int model);

	uint32_t& GetReadableBones();

	uint32_t& GetWritableBones();

	matrix3x4_t*& GetBoneArrayForWrite();

	float SpawnTime();

	int& GetEffects();

	bool IsPlayer();

	bool IsScoped();

	bool& ClientAnimations();

	void UpdateClientAnimation();

	float* GetPoseParameters();
private:
	DWORD GetActiveWeaponHandle();
public:
	float& GetLowerBodyYaw();

	int GetTeam();

	bool bHasHeavyArmor();

	int& GetFlags();

	float& GetFlashMaxAlpha();

	int GetMoney();

	QAngle& GetEyeAngles();

	Vector& GetVelocity();

	int HitBoxSet();

	int GetMoveType();

	bool GetLifeState();

	int GetHealth();

	bool IsAlive();

	bool IsImmune();

	int& GetTickBase();

	float GetSimulationTime();

	float GetOldSimulationTime();

	float GetAnimTime();

	Vector& GetOrigin();

	Vector GetViewOffset();

	QAngle GetPunchAngles();

	Vector GetEyePosition();

	int GetArmor();

	int HeavyArmor();

	bool HasHelmet();

	float NextAttack();

	int ChokedTicks();

	CCSGOAnimState* GetAnimState();

	void SetAbsAngles(float yaw);

	void SetAbsOrigin(Vector origin);

	QAngle& GetAbsAngles();

	Vector& GetAbsOrigin();
};