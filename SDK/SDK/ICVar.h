#include "..\\Utils\Color.h"
#include "..\\Utils\Utils.h"
#pragma once
class ConVar
{
public:

	void SetValue(float value)
	{
		typedef void(__thiscall * OriginalFn)(void*, float);
		return Utils::GetVFunc<OriginalFn>(this, 15)(this, value);
	}
	void SetValue(int value)
	{
		typedef void(__thiscall * OriginalFn)(void*, int);
		return Utils::GetVFunc<OriginalFn>(this, 16)(this, value);
	}
	void SetValueChar(const char* name)
	{
		typedef void(__thiscall * SetNameFN)(void*, const char*);
		return Utils::GetVFunc<SetNameFN>(this, 14)(this, name);
	}
	void InternalSetString(const char* str)
	{
		typedef void(__thiscall * SetStringFn)(void*, const char*);
		return Utils::GetVFunc<SetStringFn>(this, 17)(this, str);
	}
	char* GetBaseName()
	{
		typedef char* (__thiscall * SetStringFn)(void*);
		return Utils::GetVFunc<SetStringFn>(this, 6)(this);
	}
	float GetFloat()
	{
		typedef float(__thiscall * SetValueFn)(void*);
		return Utils::GetVFunc< SetValueFn >(this, 12)(this);
	}
	int GetInt(void) {
		return Utils::CallVFunc<13, int>(this);
	}
	const char* GetString(void) const {
		return pParent->pszDefaultValue;
	}

	char pad_0x0000[0x4]; //0x0000
	ConVar* pNext; //0x0004 
	__int32 bRegistered; //0x0008 
	char* pszName; //0x000C 
	char* pszHelpString; //0x0010 
	__int32 nFlags; //0x0014 
	char pad_0x0018[0x4]; //0x0018
	ConVar* pParent; //0x001C 
	char* pszDefaultValue; //0x0020 
	char* strString; //0x0024 
	__int32 StringLength; //0x0028 
	float fValue; //0x002C 
	__int32 nValue; //0x0030 
	__int32 bHasMin; //0x0034 
	float fMinVal; //0x0038 
	__int32 bHasMax; //0x003C 
	float fMaxVal; //0x0040 
	void* fnChangeCallback; //0x0044 


};//Size=0x0048

class ConCommandBase;
class ConCommand;
struct CVarDLLIdentifier_t;
class ICVar
{
public:
	template <typename... Values>
	void ConsoleColorPrintf(const Color& Msgcolor_t, const char* szMsgFormat, Values... Parameters)
	{
		typedef void(__cdecl*oConsolecolor_tPrintf)(void*, const Color&, const char*, ...);
		return Utils::GetVFunc<oConsolecolor_tPrintf>(this, 25)(this, Msgcolor_t, szMsgFormat, Parameters...);
	}
	ConVar* FindVar(const char* var_name)
	{
		return Utils::CallVFunc<17, ConVar*>(this, var_name);
	}
};

extern ICVar* g_pCvar;