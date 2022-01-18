#ifndef SDK_HPP
#define SDK_HPP

// -- PLATFORM --

#define MODULE(name) name MODULE_EXTENSION
#define MODULE_EXTENSION ".so"

// -- INTERFACE --

// highly temporary, probably will make my own version since it's incompatible with anything source :PPP
#define INTERFACEVERSION_ISERVERPLUGINCALLBACKS "ISERVERPLUGINCALLBACKS002"

typedef void*(*CreateInterfaceFn)(const char* pName, int* pReturnCode);
typedef void*(*InstantiateInterfaceFn)();

class IServerPluginCallbacks {
public:
	virtual bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory) = 0;
	virtual void Unload() = 0;
	virtual void Pause() = 0;
	virtual void UnPause() = 0;
	virtual const char* GetPluginDescription() = 0;
	virtual void LevelInit(char const* pMapName) = 0;
	virtual void ServerActivate(void* pEdictList, int edictCount, int clientMax) = 0;
	virtual void GameFrame(bool simulating) = 0;
	virtual void LevelShutdown() = 0;
	virtual void ClientFullyConnect(void* pEdict) = 0;
	virtual void ClientActive(void* pEntity) = 0;
	virtual void ClientDisconnect(void* pEntity) = 0;
	virtual void ClientPutInServer(void* pEntity, char const* playername) = 0;
	virtual void SetCommandClient(int index) = 0;
	virtual void ClientSettingsChanged(void* pEdict) = 0;
	virtual int ClientConnect(bool* bAllowConnect, void* pEntity, const char* pszName, const char* pszAddress, char* reject, int maxrejectlen) = 0;
	virtual int ClientCommand(void* pEntity, const void*& args) = 0;
	virtual int NetworkIDValidated(const char* pszUserName, const char* pszNetworkID) = 0;
	virtual void OnQueryCvarValueFinished(int iCookie, void* pPlayerEntity, int eStatus, const char* pCvarName, const char* pCvarValue) = 0;
	virtual void OnEdictAllocated(void* edict) = 0;
	virtual void OnEdictFreed(const void* edict) = 0;
};

// -- GENERIC --

struct Color {
	Color() { *((int*)this) = 255; }
	Color(int _r, int _g, int _b) { SetColor(_r, _g, _b, 255); }
	Color(int _r, int _g, int _b, int _a) { SetColor(_r, _g, _b, _a); }
	void SetColor(int _r, int _g, int _b, int _a = 255) {
		_color[0] =(unsigned char)_r;
		_color[1] =(unsigned char)_g;
		_color[2] =(unsigned char)_b;
		_color[3] =(unsigned char)_a;
	}
	unsigned char _color[4] = { 0, 0, 0, 0 };
};

template <class T, class I = int> struct CUtlMemory {
	T* m_pMemory;
	int m_nAllocationCount;
	int m_nGrowSize;
};

template <class T, class A = CUtlMemory<T>> struct CUtlVector {
	A m_Memory;
	int m_Size;
	T* m_pElements;

	void Append(const T& val) {
		if(this->m_Size == this->m_Memory.m_nAllocationCount) {
			int grow = this->m_Memory.m_nGrowSize;
			if(grow == 0)
				grow = 1;
			this->m_Memory.m_nAllocationCount += grow;
			this->m_Memory.m_pMemory = static_cast<T*>(realloc(this->m_Memory.m_pMemory, sizeof(T) * this->m_Memory.m_nAllocationCount));
			this->m_pElements = this->m_Memory.m_pMemory;
		}
		this->m_Memory.m_pMemory[this->m_Size] = val;
		this->m_Size++;
	}

	void Clear() {
		if(this->m_Memory.m_pMemory) {
			free(this->m_Memory.m_pMemory);
			this->m_Memory.m_pMemory = 0;
		}
		this->m_Size = 0;
		this->m_Memory.m_nAllocationCount = 0;
	}
};

// -- CONSOLE --

typedef int LoggingChannelID_t;

enum LoggingResponse_t {
	LR_CONTINUE,
	LR_DEBUGGER,
	LR_ABORT,
};

enum LoggingSeverity_t {
	LS_MESSAGE = 0,
	LS_WARNING = 1,
	LS_ASSERT = 2,
	LS_ERROR = 3,
	LS_HIGHEST_SEVERITY = 4,
};

// -- CONCOMMAND --

struct ConCommandBase;
struct CCommand;
typedef void (*FnChangeCallback_t)(void* var, const char* pOldValue, float flOldValue);

#define COMMAND_COMPLETION_MAXITEMS 64
#define COMMAND_COMPLETION_ITEM_LENGTH 64

using _CommandCallback = void (*)(const CCommand& args);
using _CommandCompletionCallback = int (*)(const char* partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH]);
using _InternalSetValue = void(*)(void* thisptr, const char* value);
using _InternalSetFloatValue = void(*)(void* thisptr, float value);
using _InternalSetIntValue = void(*)(void* thisptr, int value);
using _RegisterConCommand = void(*)(void* thisptr, ConCommandBase* pCommandBase);
using _UnregisterConCommand = void(*)(void* thisptr, ConCommandBase* pCommandBase);
using _FindCommandBase = void*(*)(void* thisptr, const char* name);
using _InstallGlobalChangeCallback = void(*)(void* thisptr, FnChangeCallback_t callback);
using _RemoveGlobalChangeCallback = void(*)(void* thisptr, FnChangeCallback_t callback);
using _AutoCompletionFunc = int(*)(void* thisptr, char const* partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH]);

struct ConCommandBase {
	void* ConCommandBase_VTable;
	ConCommandBase* m_pNext;
	bool m_bRegistered;
	const char* m_pszName;
	const char* m_pszHelpString;
	int m_nFlags;

	ConCommandBase(const char* name, int flags, const char* helpstr) :
		ConCommandBase_VTable(nullptr),
		m_pNext(nullptr),
		m_bRegistered(nullptr),
		m_pszName(name),
		m_pszHelpString(helpstr),
		m_nFlags(flags)
	{}
};

struct CCommand {
	enum {
		COMMAND_MAX_ARGC = 64,
		COMMAND_MAX_LENGTH = 512
	};
	int m_nArgc;
	int m_nArgv0Size;
	char m_pArgSBuffer[COMMAND_MAX_LENGTH];
	char m_pArgvBuffer[COMMAND_MAX_LENGTH];
	const char* m_ppArgv[COMMAND_MAX_ARGC];

	int ArgC() const {
		return this->m_nArgc;
	}
	const char* Arg(int nIndex) const {
		return this->m_ppArgv[nIndex];
	}
	const char* operator[](int nIndex) const {
		return Arg(nIndex);
	}
};

struct ConCommand : ConCommandBase {
	union {
		void* m_fnCommandCallbackV1;
		_CommandCallback m_fnCommandCallback;
		void* m_pCommandCallback;
	};

	union {
		_CommandCompletionCallback m_fnCompletionCallback;
		void* m_pCommandCompletionCallback;
	};

	bool m_bHasCompletionCallback : 1;
	bool m_bUsingNewCommandCallback : 1;
	bool m_bUsingCommandCallbackInterface : 1;

	ConCommand(const char* pName, _CommandCallback callback, const char* pHelpString, int flags, _CommandCompletionCallback completionFunc) :
		ConCommandBase(pName, flags, pHelpString),
		m_fnCommandCallback(callback),
		m_fnCompletionCallback(completionFunc),
		m_bHasCompletionCallback(completionFunc != nullptr),
		m_bUsingNewCommandCallback(true),
		m_bUsingCommandCallbackInterface(false)
	{}
};

struct ConVar : ConCommandBase {
	void* ConVar_VTable;
	ConVar* m_pParent;
	const char* m_pszDefaultValue;
	char* m_pszString;
	int m_StringLength;
	float m_fValue;
	int m_nValue;
	bool m_bHasMin;
	float m_fMinVal;
	bool m_bHasMax;
	float m_fMaxVal;
	CUtlVector<FnChangeCallback_t> m_fnChangeCallback;

	ConVar(const char* name, const char* value, int flags, const char* helpstr, bool hasmin, float min, bool hasmax, float max) :
		ConCommandBase(name, flags, helpstr),
		ConVar_VTable(nullptr),
		m_pParent(nullptr),
		m_pszDefaultValue(value),
		m_pszString(nullptr),
		m_fValue(0.0f),
		m_nValue(0),
		m_bHasMin(hasmin),
		m_fMinVal(min),
		m_bHasMax(hasmax),
		m_fMaxVal(max),
		m_fnChangeCallback()
	{}
};

#endif // SDK_HPP
