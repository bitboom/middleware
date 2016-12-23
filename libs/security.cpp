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
#include "security.hxx"

namespace DevicePolicyManager {

struct SecurityPolicy::Private {
	Private(PolicyControlContext& ctxt) : context(ctxt) {}
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

SecurityPolicy::SecurityPolicy(PolicyControlContext& ctxt) :
	pimpl(new Private(ctxt))
{
}

SecurityPolicy::~SecurityPolicy()
{
}

int SecurityPolicy::lockoutScreen()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("SecurityPolicy::lockoutScreen");
}

int SecurityPolicy::setInternalStorageEncryption(bool encrypt)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("SecurityPolicy::setInternalStorageEncryption", encrypt);
}

bool SecurityPolicy::isInternalStorageEncrypted()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("SecurityPolicy::isInternalStorageEncrypted");
}

int SecurityPolicy::setExternalStorageEncryption(bool encrypt)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("SecurityPolicy::setExternalStorageEncryption", encrypt);
}

bool SecurityPolicy::isExternalStorageEncrypted()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("SecurityPolicy::isExternalStorageEncrypted");
}

} // namespace DevicePolicyManager
