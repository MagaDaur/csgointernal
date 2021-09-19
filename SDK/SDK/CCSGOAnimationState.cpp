#include "CCSGOAnimationState.h"
#include "CBaseEntity.h"
#include "CBaseCombatWeapon.h"

void CCSGOAnimState::Update(QAngle angle)
{
	static auto UpdateAnimState = Utils::FindSignature("client.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");

	__asm {
		push 0
		mov ecx, this
		movss xmm1, dword ptr[angle + 4]
		movss xmm2, dword ptr[angle]

		call UpdateAnimState
	}
}

void CCSGOAnimState::Reset()
{
	typedef void(__thiscall* ResetAnimState_t)(CCSGOAnimState*);
	static auto ResetAnimState = (ResetAnimState_t)Utils::FindSignature("client.dll", "56 6A 01 68 ? ? ? ? 8B F1");

	ResetAnimState(this);
}