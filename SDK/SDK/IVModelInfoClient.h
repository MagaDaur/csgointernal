#pragma once

#include "Studio.h"
#include "IVModelRender.h"

class CPhysCollide;
class CUtlBuffer;
class IClientRenderable;
struct virtualmodel_t;

enum RenderableTranslucencyType_t
{
	RENDERABLE_IS_OPAQUE = 0,
	RENDERABLE_IS_TRANSLUCENT,
	RENDERABLE_IS_TWO_PASS,    // has both translucent and opaque sub-partsa
};

class IVModelInfo // gucci
{
public:
	inline void* GetModel(int Index) {
		using Fn = void* (__thiscall*)(IVModelInfo*, int);
		return (*(Fn * *)this)[1](this, Index);
	}
	int	GetModelIndex(const char* name)
	{
		using Fn = int(__thiscall*)(IVModelInfo*, const char*);
		return (*(Fn * *)this)[2](this, name);
	}
	inline const char* GetModelNamevoid(const void* Model) {
		using Fn = char* (__thiscall*)(void*, const void*);
		return (*(Fn * *)this)[3](this, Model);
	}
	const char* GetModelName(const model_t* mod)
	{
		using Fn = char* (__thiscall*)(IVModelInfo*, const model_t*);
		return (*(Fn * *)this)[3](this, mod);
	}
	studiohdr_t* GetStudiomodel(const model_t* mod)
	{
		using Fn = studiohdr_t * (__thiscall*)(IVModelInfo*, const model_t*);
		return (*(Fn * *)this)[32](this, mod);
	}


	void GetModelMaterials(const model_t* model, int count, IMaterial** ppMaterial)
	{
		typedef void* (__thiscall * oGetModelMaterials)(PVOID, const model_t*, int, IMaterial * *);
		Utils::GetVFunc<oGetModelMaterials>(this, 19)(this, model, count, ppMaterial);
	}
};
extern IVModelInfo* g_MdlInfo;