#pragma once
#include "Vector.h"
#include "CBaseEntity.h"

class IPanel
{
public:
	const char* GetName(unsigned int vguiPanel)
	{
		using original_fn = const char* (__thiscall*)(IPanel*, unsigned int);
		return (*(original_fn**)this)[36](this, vguiPanel);
	}
};
extern IPanel* g_pPanel;