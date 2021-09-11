#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "..\SDK\Recv.h"

#undef GetProp

class NetvarTree 
{
public:
	void Init();
	uintptr_t GetProp(std::string, std::string);
private:

	void UnpackTable(RecvTable*, std::string);

	std::map<std::string, std::map<std::string, uintptr_t>> props;
};

extern NetvarTree g_Netvars;