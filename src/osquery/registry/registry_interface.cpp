/**
 *  Copyright (c) 2018-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include <osquery/registry_factory.h>
#include <osquery/registry_interface.h>
#include <osquery/utils/conversions/split.h>

namespace osquery {
void RegistryInterface::remove(const std::string& item_name)
{
	if (items_.count(item_name) > 0) {
		items_[item_name]->tearDown();
		items_.erase(item_name);
	}

	// Populate list of aliases to remove (those that mask item_name).
	std::vector<std::string> removed_aliases;
	for (const auto& alias : aliases_) {
		if (alias.second == item_name) {
			removed_aliases.push_back(alias.first);
		}
	}

	for (const auto& alias : removed_aliases) {
		aliases_.erase(alias);
	}
}

bool RegistryInterface::isInternal(const std::string& item_name) const
{
	ReadLock lock(mutex_);

	return isInternal_(item_name);
}

std::string RegistryInterface::getActive() const
{
	ReadLock lock(mutex_);

	return active_;
}

std::string RegistryInterface::getName() const
{
	ReadLock lock(mutex_);

	return name_;
}

size_t RegistryInterface::count() const
{
	ReadLock lock(mutex_);

	return items_.size();
}

Status RegistryInterface::setActive(const std::string& item_name)
{
	// FIXME
	//  UpgradeLock lock(mutex_);

	// Default support multiple active plugins.
	for (const auto& item : osquery::split(item_name, ",")) {
		if (items_.count(item) == 0) {
			return Status::failure("Unknown registry plugin: " + item);
		}
	}

	Status status;
	{
		// FIXME
		//    WriteUpgradeLock wlock(lock);
		active_ = item_name;
	}

	// The active plugin is setup when initialized.
	for (const auto& item : osquery::split(item_name, ",")) {
		if (exists_(item, true)) {
			status = RegistryFactory::get().plugin(name_, item)->setUp();
		}

		if (!status.ok()) {
			break;
		}
	}
	return status;
}

Status RegistryInterface::call(const std::string& item_name,
							   const PluginRequest& request,
							   PluginResponse& response)
{
	if (item_name.empty()) {
		return Status::failure("No registry item name specified");
	}
	PluginRef plugin;
	{
		ReadLock lock(mutex_);

		// Search local plugins (items) for the plugin.
		if (items_.count(item_name) > 0) {
			plugin = items_.at(item_name);
		}
	}
	if (plugin) {
		return plugin->call(request, response);
	}

	return Status::failure("Unknown registry name: " + item_name);
}

Status RegistryInterface::addAlias(const std::string& item_name,
								   const std::string& alias)
{
	WriteLock lock(mutex_);

	if (aliases_.count(alias) > 0) {
		return Status::failure("Duplicate alias: " + alias);
	}
	aliases_[alias] = item_name;
	return Status::success();
}

std::string RegistryInterface::getAlias(const std::string& alias) const
{
	ReadLock lock(mutex_);

	if (aliases_.count(alias) == 0) {
		return alias;
	}
	return aliases_.at(alias);
}

Status RegistryInterface::addPlugin(const std::string& plugin_name,
									const PluginRef& plugin_item,
									bool internal)
{
	WriteLock lock(mutex_);

	if (items_.count(plugin_name) > 0) {
		return Status::failure("Duplicate registry item exists: " + plugin_name);
	}

	plugin_item->setName(plugin_name);
	items_.emplace(std::make_pair(plugin_name, plugin_item));

	// The item can be listed as internal, meaning it does not broadcast.
	if (internal) {
		internal_.push_back(plugin_name);
	}

	return Status::success();
}

void RegistryInterface::setUp()
{
	ReadLock lock(mutex_);

	// If this registry does not auto-setup do NOT setup the registry items.
	if (!auto_setup_) {
		return;
	}

	// If the registry is using a single 'active' plugin, setUp that plugin.
	// For config and logger, only setUp the selected plugin.
	if (active_.size() != 0 && exists_(active_, true)) {
		items_.at(active_)->setUp();
		return;
	}

	// Try to set up each of the registry items.
	// If they fail, remove them from the registry.
	std::vector<std::string> failed;
	for (auto& item : items_) {
		if (!item.second->setUp().ok()) {
			failed.push_back(item.first);
		}
	}

	for (const auto& failed_item : failed) {
		remove(failed_item);
	}
}

void RegistryInterface::configure()
{
	ReadLock lock(mutex_);

	if (!active_.empty() && exists_(active_, true)) {
		items_.at(active_)->configure();
	} else {
		for (auto& item : items_) {
			item.second->configure();
		}
	}
}

/// Facility method to check if a registry item exists.
bool RegistryInterface::exists(const std::string& item_name, bool local) const
{
	ReadLock lock(mutex_);

	return exists_(item_name, local);
}

/// Facility method to list the registry item identifiers.
std::vector<std::string> RegistryInterface::names() const
{
	ReadLock lock(mutex_);

	std::vector<std::string> names;
	for (const auto& item : items_) {
		names.push_back(item.first);
	}

	return names;
}

std::map<std::string, PluginRef> RegistryInterface::plugins()
{
	ReadLock lock(mutex_);

	return items_;
}

void RegistryInterface::setname(const std::string& name)
{
	WriteLock lock(mutex_);

	name_ = name;
}

bool RegistryInterface::isInternal_(const std::string& item_name) const
{
	if (std::find(internal_.begin(), internal_.end(), item_name) ==
		internal_.end()) {
		return false;
	}
	return true;
}

bool RegistryInterface::exists_(const std::string& item_name,
								bool local) const
{
	return (items_.count(item_name) > 0);
}

AutoRegisterInterface::AutoRegisterInterface(const char* _type,
											 const char* _name,
											 bool optional)
	: type_(_type), name_(_name), optional_(optional) {}

AutoRegisterSet& AutoRegisterInterface::registries()
{
	static AutoRegisterSet registries_;
	return registries_;
}

AutoRegisterSet& AutoRegisterInterface::plugins()
{
	static AutoRegisterSet plugins_;
	return plugins_;
}

void AutoRegisterInterface::autoloadRegistry(
	std::unique_ptr<AutoRegisterInterface> ar_)
{
	registries().push_back(std::move(ar_));
}

void AutoRegisterInterface::autoloadPlugin(
	std::unique_ptr<AutoRegisterInterface> ar_)
{
	plugins().push_back(std::move(ar_));
}

void registryAndPluginInit()
{
	for (const auto& it : AutoRegisterInterface::registries()) {
		it->run();
	}

	for (const auto& it : AutoRegisterInterface::plugins()) {
		it->run();
	}

	AutoRegisterSet().swap(AutoRegisterInterface::registries());
	AutoRegisterSet().swap(AutoRegisterInterface::plugins());
}

} // namespace osquery
