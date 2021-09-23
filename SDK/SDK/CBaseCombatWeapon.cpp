#include "CBaseCombatWeapon.h"
#include "IWeaponSystem.h"
#include "ClientClass.h"
#include "..\Utils\Utils.h"

unsigned int CBaseCombatWeapon::GetItemDefinitionIndex()
{
	static int m_iItemDefinitionIndex = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_iItemDefinitionIndex");
	return *(unsigned int*)(uintptr_t(this) + 0x2FAA);
}

WeaponInfo_t* CBaseCombatWeapon::GetWeaponInfo()
{
	//return g_pWeaponSys->GetWpnData(GetItemDefinitionIndex());
	return Utils::CallVFunc<461, WeaponInfo_t*>(this);
}

float& CBaseCombatWeapon::GetNextPrimaryAttack()
{
	static int m_flNextPrimaryAttack = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_flNextPrimaryAttack");
	return *(float*)(DWORD(this) + m_flNextPrimaryAttack);
}

float CBaseCombatWeapon::GetLastShotTime()
{
	static int m_fLastShotTime = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_fLastShotTime");
	return *(float*)(uintptr_t(this) + m_fLastShotTime);
}

int CBaseCombatWeapon::GetAmmo()
{
	static int m_iClip1 = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_iClip1");
	return *(int*)(uintptr_t(this) + m_iClip1);
}

bool CBaseCombatWeapon::IsGrenade() {
	auto wpninfo = this->GetWeaponInfo();
	return (wpninfo->weaponType == WEAPONTYPE_GRENADE);
}

bool CBaseCombatWeapon::IsPinPulled() {
	static int m_bIsPinPulled = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_bPinPulled");
	return *(bool*)(uintptr_t(this) + m_bIsPinPulled);
}

float CBaseCombatWeapon::ThrowTime() {
	static int m_fThrowTime = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_fThrowTime");
	return *(float*)(uintptr_t(this) + m_fThrowTime);
}
float CBaseCombatWeapon::GetSpread() {
	return Utils::CallVFunc<453, float>(this);
}

float CBaseCombatWeapon::GetInaccuracy() {
	return Utils::CallVFunc<483, float>(this);
}
float CBaseCombatWeapon::GetAccuracyPenalty() {
	static int m_fAccuracyPenalty = g_Netvars.GetProp(this->GetClientClass()->pNetworkName, "m_fAccuracyPenalty");
	return *(float*)(DWORD(this) + m_fAccuracyPenalty);
}