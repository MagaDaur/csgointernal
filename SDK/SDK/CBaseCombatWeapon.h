#pragma once
#include "CBaseEntity.h"
#include "..\Utils\NetvarManager.h"

class CBaseCombatWeapon: public IClientEntity
{
public:
	unsigned int GetItemDefinitionIndex()
	{
		static int m_iItemDefinitionIndex = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_iItemDefinitionIndex");
		return *(unsigned int*)(uintptr_t(this) + 0x2FAA);
	}

	const WeaponInfo_t* GetWeaponInfo()
	{
		return g_pWeaponSys->GetWpnData(GetItemDefinitionIndex());
	}

	float& GetNextPrimaryAttack()
	{
		static int m_flNextPrimaryAttack = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_flNextPrimaryAttack");
		return *(float*)(DWORD(this) + m_flNextPrimaryAttack);
	}

	float GetLastShotTime()
	{
		static int m_fLastShotTime = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_fLastShotTime");
		return *(float*)(uintptr_t(this) + m_fLastShotTime);
	}

	int GetAmmo()
	{
		static int m_iClip1 = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_iClip1");
		return *(int*)(uintptr_t(this) + m_iClip1);
	}

	bool IsGrenade() {
		auto wpninfo = this->GetWeaponInfo();
		return (wpninfo->weapon_type == WEAPONTYPE_GRENADE);
	}

	bool isPinPulled() {
		static int m_bIsPinPulled = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_bPinPulled");
		return *(bool*)(uintptr_t(this) + m_bIsPinPulled);
	}

	float ThrowTime() {
		static int m_fThrowTime = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_fThrowTime");
		return *(float*)(uintptr_t(this) + m_fThrowTime);
	}
	float GetSpread() {
		return Utils::CallVFunc<452, float>(this);
	}
	WeaponInfo_t* GetCSWpnData()
	{
		return Utils::CallVFunc<460, WeaponInfo_t*>(this);
	}
	float GetInaccuracy() {
		return Utils::CallVFunc<482, float>(this);
	}
	float GetAccuracyPenalty() {
		static int m_fAccuracyPenalty = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_fAccuracyPenalty");
		return *(float*)(DWORD(this) + m_fAccuracyPenalty);
	}
};
