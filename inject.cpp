#include "sdk.hpp"

#include <dlfcn.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <iostream>
#include <memory>

#include "exception.hpp"

#include "console.hpp"
#include "tier1.hpp"
#include "command.hpp"

CON_COMMAND(plugin_load, "plugin_load <filename> : loads a plugin") {
	if(args.ArgC() < 2) {
		console->Print("plugin_load <filename>\n");
	} else {
		void* handle;
		try {
			auto pluginPath = std::make_unique<char[]>(MAX_PATH);
			snprintf(pluginPath.get(), MAX_PATH, "./%s.so", args[1]);

			struct stat st;
			if(stat(pluginPath.get(), &st)) throw fmtEx("file \"%s\" not found", pluginPath.get());

			handle = dlopen(pluginPath.get(), RTLD_NOW);
			if(!handle) throw smpEx("could not dlopen() plugin");

			auto CreateInterface = Memory::GetSymbolAddress<CreateInterfaceFn>(handle, "CreateInterface");

			if(!CreateInterface) throw smpEx("no CreateInterface :(");

			int ret;
			IServerPluginCallbacks* cbs = (IServerPluginCallbacks*)CreateInterface(INTERFACEVERSION_ISERVERPLUGINCALLBACKS, &ret);

			if(!cbs) throw smpEx("no callbacks :P");

			if(!cbs->Load(nullptr, nullptr)) throw smpEx("failed to load ;(");
		} catch(std::exception& ex) {
			if(handle) dlclose(handle);
			console->Print("ERROR: %s\n", ex.what());
		};
	}
}

static void Inject() {
	printf("ceplug loading...\n");

	console = new Console();
	if(!console->Init()) fprintf(stderr, "console init failed!\n");

	tier1 = new Tier1();
	if(!tier1->Init()) fprintf(stderr, "tier1 init failed!\n");

	Command::RegisterAll();
}

static unsigned int count = 0;
using _Connect = int(*)(int sockfd, const struct sockaddr *addrm, socklen_t addrlen);

extern "C" int connect(int sockfd, const struct sockaddr *addrm, socklen_t addrlen) {
	if(count == 12) Inject();
	count++;
	_Connect Connect = (_Connect)dlsym(RTLD_NEXT, "connect");
	return Connect(sockfd, addrm, addrlen);
}