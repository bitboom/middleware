/**
 *  Copyright (c) 2018-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include <osquery/core.h>
#include <osquery/registry_factory.h>

#include <cstdlib>
#include <sstream>

#include <vist/logger.hpp>
#include <osquery/registry.h>
#include <osquery/utils/conversions/split.h>

namespace osquery {

RegistryFactory& RegistryFactory::get()
{
	static RegistryFactory instance;
	return instance;
}

void RegistryFactory::add(const std::string& name, RegistryInterfaceRef reg)
{
	if (exists(name)) {
		throw std::runtime_error("Cannot add duplicate registry: " + name);
	}
	registries_[name] = std::move(reg);
}

RegistryInterfaceRef RegistryFactory::registry(const std::string& t) const
{
	if (!exists(t)) {
		throw std::runtime_error("Unknown registry requested: " + t);
	}
	return registries_.at(t);
}

std::map<std::string, RegistryInterfaceRef> RegistryFactory::all() const
{
	return registries_;
}

std::map<std::string, PluginRef> RegistryFactory::plugins(
	const std::string& registry_name) const
{
	return registry(registry_name)->plugins();
}

PluginRef RegistryFactory::plugin(const std::string& registry_name,
								  const std::string& item_name) const
{
	return registry(registry_name)->plugin(item_name);
}

/// Adds an alias for an internal registry item. This registry will only
/// broadcast the alias name.
Status RegistryFactory::addAlias(const std::string& registry_name,
								 const std::string& item_name,
								 const std::string& alias)
{
	if (!exists(registry_name)) {
		return Status(1, "Unknown registry: " + registry_name);
	}
	return registries_.at(registry_name)->addAlias(item_name, alias);
}

/// Returns the item_name or the item alias if an alias exists.
std::string RegistryFactory::getAlias(const std::string& registry_name,
									  const std::string& alias) const
{
	if (!exists(registry_name)) {
		return alias;
	}
	return registries_.at(registry_name)->getAlias(alias);
}

Status RegistryFactory::call(const std::string& registry_name,
							 const std::string& item_name,
							 const PluginRequest& request,
							 PluginResponse& response)
{
	// Forward factory call to the registry.
	try {
		if (item_name.find(',') != std::string::npos) {
			// Call is multiplexing plugins (usually for multiple loggers).
			for (const auto& item : osquery::split(item_name, ",")) {
				get().registry(registry_name)->call(item, request, response);
			}
			// All multiplexed items are called without regard for statuses.
			return Status(0);
		}
		return get().registry(registry_name)->call(item_name, request, response);
	} catch (const std::exception& e) {
		ERROR(OSQUERY) << registry_name << " registry " << item_name
					   << " plugin caused exception: " << e.what();
		return Status(1, e.what());
	} catch (...) {
		ERROR(OSQUERY) << registry_name << " registry " << item_name
					   << " plugin caused unknown exception";
		return Status(2, "Unknown exception");
	}
}

Status RegistryFactory::call(const std::string& registry_name,
							 const std::string& item_name,
							 const PluginRequest& request)
{
	PluginResponse response;
	// Wrapper around a call expecting a response.
	return call(registry_name, item_name, request, response);
}

Status RegistryFactory::call(const std::string& registry_name,
							 const PluginRequest& request,
							 PluginResponse& response)
{
	auto plugin = get().registry(registry_name)->getActive();
	return call(registry_name, plugin, request, response);
}

Status RegistryFactory::call(const std::string& registry_name,
							 const PluginRequest& request)
{
	PluginResponse response;
	return call(registry_name, request, response);
}

Status RegistryFactory::setActive(const std::string& registry_name,
								  const std::string& item_name)
{
	WriteLock lock(mutex_);
	return registry(registry_name)->setActive(item_name);
}

std::string RegistryFactory::getActive(const std::string& registry_name) const
{
	return registry(registry_name)->getActive();
}

void RegistryFactory::setUp()
{
	for (const auto& registry : get().all()) {
		registry.second->setUp();
	}
}

bool RegistryFactory::exists(const std::string& registry_name,
							 const std::string& item_name,
							 bool local) const
{
	if (!exists(registry_name)) {
		return false;
	}

	// Check the registry.
	return registry(registry_name)->exists(item_name, local);
}

std::vector<std::string> RegistryFactory::names() const
{
	std::vector<std::string> names;
	for (const auto& registry : all()) {
		names.push_back(registry.second->getName());
	}
	return names;
}

std::vector<std::string> RegistryFactory::names(
	const std::string& registry_name) const
{
	if (registries_.at(registry_name) == nullptr) {
		std::vector<std::string> names;
		return names;
	}
	return registry(registry_name)->names();
}

size_t RegistryFactory::count(const std::string& registry_name) const
{
	if (!exists(registry_name)) {
		return 0;
	}
	return registry(registry_name)->count();
}

} // namespace osquery
