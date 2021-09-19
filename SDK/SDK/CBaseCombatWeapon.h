#pragma once
#include "Definitions.h"
#include "IClientUnknown.h"
#include "IClientNetworkable.h"
#include "IClientEntity.h"

#include "..\Utils\Utils.h"
#include "..\Utils\NetvarManager.h"

class CBaseCombatWeapon: public IClientEntity
{
public:
	unsigned int GetItemDefinitionIndex();

	WeaponInfo_t* GetWeaponInfo();

	float& GetNextPrimaryAttack();

	float GetLastShotTime();

	int GetAmmo();

	bool IsGrenade();

	bool IsPinPulled();

	float ThrowTime();

	float GetSpread();

	float GetInaccuracy();

	float GetAccuracyPenalty();
};
