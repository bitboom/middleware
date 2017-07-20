#include <dlfcn.h>

#include <string>
#include <iostream>
#include <mutex>
#include <thread>
#include <exception>
#include <memory>
#include <unordered_map>

#include "plugin.h"

Plugin::Plugin() :
	handle(nullptr)
{
}

Plugin::~Plugin()
{
}

int Plugin::load(const std::string& path, int flags)
{
	handle = ::dlopen(path.c_str(), flags);
	if (!handle) {
		return -1;
	}

	return 0;
}

void Plugin::unload()
{
	if (handle) {
		dlclose(handle);
		handle = nullptr;
	}
}

void* Plugin::lookupSymbol(const std::string& name)
{
	void *result = nullptr;
	if (handle) {
		result = ::dlsym(handle, name.c_str());
	}
	return result;
}

PolicyLoader::PolicyLoader(const std::string& base) :
	basename(base + "/")
{
}

AbstractPolicyProvider* PolicyLoader::instantiate(const std::string& name, PolicyControlContext& context)
{
	PluginMap::iterator iter = pluginMap.find(name);
	if (iter == pluginMap.end()) {
		Plugin plguin;
		if (plguin.load(basename + name, RTLD_LAZY | RTLD_LOCAL) != 0) {
			std::cout << "Failed to load \"" << name << "\"" << std::endl;
			return nullptr;
		}

		std::cout << "Loading plugin: " << name << " ... ";
		PolicyFactory factory = (PolicyFactory)(plguin.lookupSymbol("PolicyFactory"));
		if (factory == nullptr) {
			std::cout << dlerror() << std::endl;
			return NULL;
		}

		std::cout << "OK" << std::endl;
		pluginMap[name] = std::move(plguin);

		return (*factory)(context);
	}

	return nullptr;
}
