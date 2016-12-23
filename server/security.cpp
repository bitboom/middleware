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
#include <vconf.h>
#include <dd-deviced.h>
#include <dd-control.h>
#include <klay/process.h>
#include <klay/filesystem.h>
#include <klay/audit/logger.h>

#include "privilege.h"
#include "policy-builder.h"
#include "launchpad.h"

#include "security.hxx"

namespace DevicePolicyManager {

namespace {

const std::string APPID_DEVICE_ENCRYPTION = "org.tizen.ode";
const std::string APPID_LOCKSCREEN = "org.tizen.lockscreen";

} // namespace

struct SecurityPolicy::Private {
	Private(PolicyControlContext& ctxt) : context(ctxt) {}

	bool checkEncryptionState(const char* key, bool encrypt)
	{
		char *value = ::vconf_get_str(key);
		if (value == NULL) {
			ERROR("Failed to read internal storage encryption state");
			return false;
		}

		std::string state(value);
		if (encrypt) {
			if (state != "unencrypted") {
				ERROR("Storage might be already encrypted or it has error");
				return false;
			}
		} else {
			if (state != "encrypted") {
				ERROR("Storage might be already decrypted or it has error");
				return false;
			}
		}

		return true;
	}

	bool getEncryptionState(const char* key)
	{
		char *state = ::vconf_get_str(key);
		if (state == NULL) {
			throw runtime::Exception("Failed to read internal storage encryption state");
		}

		std::string expected("encrypted");
		if (expected == state) {
			return true;
		}

		return false;
	}

	int launchApplication(const std::string& name, const Bundle& bundle)
	{
		try {
			Launchpad launchpad(context.getPeerUid());
			if (launchpad.isRunning(name)) {
				launchpad.resume(name);
				return 0;
			}

			launchpad.launch(name, bundle);
		} catch (runtime::Exception& e) {
			ERROR("Failed to start application: " << name);
			return -1;
		}

		return 0;
	}

	PolicyControlContext& context;
};

SecurityPolicy::SecurityPolicy(SecurityPolicy&& rhs) = default;
SecurityPolicy& SecurityPolicy::operator=(SecurityPolicy&& rhs) = default;

SecurityPolicy::SecurityPolicy(const SecurityPolicy& rhs)
{
	if (rhs.pimpl) {
		pimpl.reset(new Private(*rhs.pimpl));
	}
}

SecurityPolicy& SecurityPolicy::operator=(const SecurityPolicy& rhs)
{
	if (!rhs.pimpl) {
		pimpl.reset();
	} else {
		pimpl.reset(new Private(*rhs.pimpl));
	}

	return *this;
}

SecurityPolicy::SecurityPolicy(PolicyControlContext& context) :
	pimpl(new Private(context))
{
	context.expose(this, DPM_PRIVILEGE_LOCK, (int)(SecurityPolicy::lockoutScreen)());
	context.expose(this, DPM_PRIVILEGE_SECURITY, (int)(SecurityPolicy::setInternalStorageEncryption)(bool));
	context.expose(this, DPM_PRIVILEGE_SECURITY, (int)(SecurityPolicy::setExternalStorageEncryption)(bool));

	context.expose(this, "", (bool)(SecurityPolicy::isInternalStorageEncrypted)());
	context.expose(this, "", (bool)(SecurityPolicy::isExternalStorageEncrypted)());
}

SecurityPolicy::~SecurityPolicy()
{
}

int SecurityPolicy::lockoutScreen()
{
	return pimpl->launchApplication(APPID_LOCKSCREEN, Bundle());
}

int SecurityPolicy::setInternalStorageEncryption(bool encrypt)
{
	if (!pimpl->checkEncryptionState(VCONFKEY_ODE_CRYPTO_STATE, encrypt)) {
		return -1;
	}

	Bundle bundle;
	bundle.add("viewtype", encrypt ? "ENCRYPT_DEVICE" : "DECRYPT_DEVICE");
	return pimpl->launchApplication(APPID_DEVICE_ENCRYPTION, bundle);
}

bool SecurityPolicy::isInternalStorageEncrypted()
{
	return pimpl->getEncryptionState(VCONFKEY_ODE_CRYPTO_STATE);
}

int SecurityPolicy::setExternalStorageEncryption(bool encrypt)
{
	if (!pimpl->checkEncryptionState(VCONFKEY_SDE_CRYPTO_STATE, encrypt)) {
		return -1;
	}

	Bundle bundle;
	bundle.add("viewtype", encrypt ? "ENCRYPT_SD_CARD" : "DECRYPT_SD_CARD");
	return pimpl->launchApplication(APPID_DEVICE_ENCRYPTION, bundle);
}

bool SecurityPolicy::isExternalStorageEncrypted()
{
	return pimpl->getEncryptionState(VCONFKEY_SDE_CRYPTO_STATE);
}

DEFINE_POLICY(SecurityPolicy);

} // namespace DevicePolicyManager
