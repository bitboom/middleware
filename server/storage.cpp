/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
#include <thread>
#include <vector>

#include <dd-deviced.h>
#include <dd-control.h>
#include <klay/process.h>
#include <klay/exception.h>
#include <klay/filesystem.h>
#include <klay/dbus/variant.h>
#include <klay/dbus/connection.h>
#include <klay/audit/logger.h>

#include "privilege.h"
#include "policy-builder.h"

#include "storage.hxx"

namespace DevicePolicyManager {

namespace {

const std::string PROG_FACTORY_RESET = "/usr/bin/dbus-send";
const std::vector<std::string> wipeCommand = {
	"/usr/bin/dbus-send",
	"--system",
	"--type=signal",
	"--print-reply",
	"--dest=com.samsung.factoryreset",
	"/com/samsung/factoryreset",
	"com.samsung.factoryreset.start.setting"
};

} // namespace

struct StoragePolicy::Private {
	Private(PolicyControlContext& ctxt) : context(ctxt) {}

	std::vector<std::string> getStorageDeviceList(const std::string& type);
	void requestDeviceFormat(const std::string& devnode, int option);

	PolicyControlContext& context;
};

std::vector<std::string> StoragePolicy::Private::getStorageDeviceList(const std::string& type)
{
	int intparams[6];
	char* strparams[7];
	std::vector<std::string> storages;

	dbus::Connection &systemDBus = dbus::Connection::getSystem();
	const dbus::Variant &var = systemDBus.methodcall("org.tizen.system.storage",
													 "/Org/Tizen/System/Storage/Block/Manager",
													 "org.tizen.system.storage.BlockManager",
													 "GetDeviceList",
													 -1,
													 "(a(issssssisibii))",
													 "(s)",
													 type.c_str());
	dbus::VariantIterator it;
	var.get("(a(issssssisibii))", &it);
	while (it.get("(issssssisibii)",
				  &intparams[0], // block type: 0 - scsi, 1 : mmc
				  &strparams[0], // devnode
				  &strparams[1], // syspath
				  &strparams[2], // usage
				  &strparams[3], // fs type
				  &strparams[4], // fs version
				  &strparams[5], // fs uuid enc
				  &intparams[1], // readonly: 0 - rw, 1 - ro
				  &strparams[6], // mount point
				  &intparams[2], // state: 0 - unmount, 1 - mount
				  &intparams[3], // primary: 0 - flase, 1 - true
				  &intparams[4], // flags: 1 - unmounted
								 //        2 - broken filesystem
								 //        4 - no filesystem
								 //        8 - not supported
								 //       16 - readonly
				  &intparams[5])) { // strage id
		storages.push_back(strrchr(strparams[0], '/') + 1);
		for (int i = 0; i < 7; i++) {
			if (strparams[i]) {
				::free(strparams[i]);
			}
		}
	}

	return storages;
}

void StoragePolicy::Private::requestDeviceFormat(const std::string& devnode, int option)
{
	int ret;
	dbus::Connection &systemDBus = dbus::Connection::getSystem();
	systemDBus.methodcall("org.tizen.system.storage",
						  "/Org/Tizen/System/Storage/Block/Devices/" + devnode,
						  "org.tizen.system.storage.Block",
						  "Format",
						  G_MAXINT,
						  "(i)",
						  "(i)",
						  option).get("(i)", &ret);
	if (ret != 0) {
		throw runtime::Exception("Failed to format " + devnode);
	}
}

StoragePolicy::StoragePolicy(StoragePolicy&& rhs) = default;
StoragePolicy& StoragePolicy::operator=(StoragePolicy&& rhs) = default;

StoragePolicy::StoragePolicy(const StoragePolicy& rhs)
{
	if (rhs.pimpl) {
		pimpl.reset(new Private(*rhs.pimpl));
	}
}

StoragePolicy& StoragePolicy::operator=(const StoragePolicy& rhs)
{
	if (!rhs.pimpl) {
		pimpl.reset();
	} else {
		pimpl.reset(new Private(*rhs.pimpl));
	}

	return *this;
}

StoragePolicy::StoragePolicy(PolicyControlContext& context) :
	pimpl(new Private(context))
{
	context.expose(this, DPM_PRIVILEGE_WIPE, (int)(StoragePolicy::wipeData)(int));
}

StoragePolicy::~StoragePolicy()
{
}

int StoragePolicy::wipeData(int id)
{
	auto worker = [id, this]() {
		if (id & WIPE_EXTERNAL_STORAGE) {
			try {
				std::vector<std::string> devices = pimpl->getStorageDeviceList("mmc");
				for (const std::string& devnode : devices) {
					std::cout << "Erase device: " << devnode << std::endl;
					pimpl->requestDeviceFormat(devnode, 1);
					std::cout << "Erase device: " << devnode << " completed" << std::endl;
				}
			} catch(runtime::Exception& e) {
				ERROR("Failed to enforce external storage policy");
				return -1;
			}
		}

		if (id & WIPE_INTERNAL_STORAGE) {
			runtime::Process proc(PROG_FACTORY_RESET, wipeCommand);
			if (proc.execute() == -1) {
				ERROR("Failed to launch factory-reset");
				return -1;
			}
		}

		return 0;
	};

	std::thread deviceWiper(worker);
	deviceWiper.detach();

	return 0;
}

DEFINE_POLICY(StoragePolicy);

} //namespace DevicePolicyManager
