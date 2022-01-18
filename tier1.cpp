#include "tier1.hpp"

#include "offsets.hpp"

#include <stdio.h>

void DumpHex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}

bool Tier1::Init() {
	this->g_pCVar = Interface::Create(this->Name(), "VEngineCvar007", false);
	if(this->g_pCVar) {
		this->RegisterConCommand = this->g_pCVar->Original<_RegisterConCommand>(Offsets::RegisterConCommand);
		this->UnregisterConCommand = this->g_pCVar->Original<_UnregisterConCommand>(Offsets::UnregisterConCommand);
		this->FindCommandBase = this->g_pCVar->Original<_FindCommandBase>(Offsets::FindCommandBase);

		this->m_pConCommandList = reinterpret_cast<ConCommandBase*>((uintptr_t)this->g_pCVar->ThisPtr() + Offsets::m_pConCommandList);

		auto listdemo = reinterpret_cast<ConCommand*>(this->FindCommandBase(this->g_pCVar->ThisPtr(), "listdemo"));
		if(listdemo) {
			this->ConCommand_VTable = listdemo->ConCommandBase_VTable;
		}

		auto sv_lan = reinterpret_cast<ConVar*>(this->FindCommandBase(this->g_pCVar->ThisPtr(), "sv_lan"));
		if(sv_lan) {
			this->ConVar_VTable = sv_lan->ConCommandBase_VTable;
			this->ConVar_VTable2 = sv_lan->ConVar_VTable;

			auto vtable = &ConVar_VTable;
			this->Dtor = Memory::VMT<_Dtor>(vtable, Offsets::Dtor);
			this->Create = Memory::VMT<_Create>(vtable, Offsets::Create);
		}

		this->InstallGlobalChangeCallback = this->g_pCVar->Original<_InstallGlobalChangeCallback>(Offsets::InstallGlobalChangeCallback);
		this->RemoveGlobalChangeCallback = this->g_pCVar->Original<_RemoveGlobalChangeCallback>(Offsets::RemoveGlobalChangeCallback);
	}

	// return this->g_pCVar && this->ConCommand_VTable && this->ConVar_VTable && this->ConVar_VTable2 && this->AutoCompletionFunc;
	return this->g_pCVar && this->ConCommand_VTable;
}

void Tier1::Shutdown() {
	Interface::Delete(g_pCVar);
}

Tier1* tier1;
