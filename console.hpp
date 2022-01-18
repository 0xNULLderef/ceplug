#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "sdk.hpp"

#include <cstdio>
#include <string>
#include <memory>

#define TIER0 "libtier0"

#define DEFAULT_COLOR Color(1, 240, 111)

class Console {
public:
	using _LoggingSystem_FindChannel = LoggingChannelID_t(*)(const char *pChannelName);
	using _LoggingSystem_LogDirect = LoggingResponse_t(*)(LoggingChannelID_t channelID, LoggingSeverity_t severity, Color spewColor, const char* pMessage);

	_LoggingSystem_FindChannel LoggingSystem_FindChannel = nullptr;
	_LoggingSystem_LogDirect LoggingSystem_LogDirect = nullptr;

private:
	LoggingChannelID_t console;

public:
	template <typename... T> void Print(const std::string& fmt, T... args) {
		unsigned int stringSize = std::snprintf(nullptr, 0, fmt.c_str(), args...) + 1;
		if(stringSize <= 0) return;
		auto size = static_cast<size_t>(stringSize);
		auto buf = std::make_unique<char[]>(size);
		std::snprintf(buf.get(), size, fmt.c_str(), args...);
		this->LoggingSystem_LogDirect(console, LS_MESSAGE, DEFAULT_COLOR, buf.get());
	}
	void DumpHex(const void* data, size_t size);

	virtual ~Console() = default;
	bool Init();
	void Shutdown();
	const char* Name() { return MODULE(TIER0); }
};

extern Console* console;

#endif // CONSOLE_HPP
