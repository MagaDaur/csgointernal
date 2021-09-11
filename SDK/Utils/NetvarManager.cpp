#include "NetvarManager.h"
#include "..\\SDK\IBaseClientDll.h"
#include "..\\SDK\ClientClass.h"
#include "..\\Utils\Utils.h"
#include <sstream>

#undef GetProp

void NetvarTree::Init()
{
    ClientClass* clientClass = g_pClientDll->GetAllClasses();

    while (clientClass) 
    {
		UnpackTable(clientClass->pRecvTable, clientClass->pNetworkName);
		
        clientClass = clientClass->pNext;
    }

	auto file = fopen("props.txt", "w");
	if(file)
	{
		std::stringstream ss;
		for(const auto &[cclass, map] : props)
		{
			ss << "[" << cclass << "]:" << std::endl;
			for(const auto& [propname, propoffset] : map)
			{
				ss << "\t[" << propname << "] = 0x" << std::hex << propoffset << std::endl;
			}
		}
		fwrite(ss.str().c_str(), 1, ss.str().size(), file);
		fclose(file);
	}

}

void NetvarTree::UnpackTable(RecvTable* table, std::string clientclass)
{
	for(int i = 0; i < table->nProps; i++)
	{
		auto prop = &table->pProps[i];
		
		if(prop->RecvType == DPT_DataTable && prop->pDataTable)
		{
			UnpackTable(prop->pDataTable, clientclass);
		}
		else
		{
			props[clientclass][prop->pVarName] = prop->Offset;
		}
	}
}

uintptr_t NetvarTree::GetProp(std::string table, std::string name)
{
	return props[table][name];
}