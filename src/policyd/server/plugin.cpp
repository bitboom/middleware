#include <dlfcn.h>

#include <string>
#include <iostream>
#include <mutex>
#include <thread>
#include <exception>
#include <memory>
#include <unordered_map>

#include "pil/logger.h"

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

std::shared_ptr<AbstractPolicyProvider> PolicyLoader::instantiate(const std::string& name)
{
	PluginMap::iterator iter = pluginMap.find(name);
	if (iter == pluginMap.end()) {
		Plugin plguin;
		if (plguin.load(basename + name, RTLD_LAZY | RTLD_LOCAL) != 0) {
			ERROR(DPM, "Failed to load \"" << name << "\"");
			return nullptr;
		}

		DEBUG(DPM, "Loading plugin: " << name << " ... ");
		PolicyFactory factory = (PolicyFactory)(plguin.lookupSymbol("PolicyFactory"));
		if (factory == nullptr) {
			ERROR(DPM, dlerror());
			return NULL;
		}

		pluginMap[name] = std::move(plguin);

		auto provider = (*factory)();
		return std::shared_ptr<AbstractPolicyProvider>(provider);
	}

	return nullptr;
}
