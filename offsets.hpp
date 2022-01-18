#ifndef OFFSETS_HPP
#define OFFSETS_HPP

namespace Offsets {
	// engine.so
	int Dtor = 0;
	int Create = 23;

	// libvstdlib.so
	int RegisterConCommand = 10;
	int UnregisterConCommand = 11;
	int FindCommandBase = 14;
	int InstallGlobalChangeCallback = 20;
	int RemoveGlobalChangeCallback = 21;
	int m_pConCommandList = 90;
};

#endif // OFFSETS_HPP
