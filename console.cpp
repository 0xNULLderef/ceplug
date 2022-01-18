#include "console.hpp"

#include "memory.hpp"

bool Console::Init() {
	auto tier0 = Memory::GetModuleHandleByName(this->Name());
	if(tier0) {
		this->LoggingSystem_FindChannel = Memory::GetSymbolAddress<_LoggingSystem_FindChannel>(tier0, "LoggingSystem_FindChannel");
		this->LoggingSystem_LogDirect = Memory::GetSymbolAddress<_LoggingSystem_LogDirect>(tier0, "LoggingSystem_LogDirect");

		Memory::CloseModuleHandle(tier0);
	}

	if(this->LoggingSystem_FindChannel)
		this->console = this->LoggingSystem_FindChannel("Console");

	return this->LoggingSystem_FindChannel && this->LoggingSystem_LogDirect && this->console;
}

void Console::Shutdown() {}

void Console::DumpHex(const void* data, size_t size) {
	char ascii[17];
	ascii[16] = '\0';
	size_t i, j;
	for (i = 0; i < size; ++i) {
		this->Print("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~')
			ascii[i % 16] = ((unsigned char*)data)[i];
		else
			ascii[i % 16] = '.';
		if ((i + 1) % 8 == 0 || i + 1 == size) {
			this->Print(" ");
			if ((i + 1) % 16 == 0) {
				this->Print("|  %s \n", ascii);
			} else if (i + 1 == size) {
				ascii[(i + 1) % 16] = '\0';
				if ((i + 1) % 16 <= 8) {
					this->Print(" ");
				}
				for (j = (i + 1) % 16; j < 16; ++j) {
					this->Print("   ");
				}
				this->Print("|  %s \n", ascii);
			}
		}
	}
}

Console* console;
