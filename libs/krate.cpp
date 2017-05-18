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

#include "status.h"
#include "krate.hxx"

namespace DevicePolicyManager {

struct KratePolicy::Private {
	Private(PolicyControlContext& ctxt) : context(ctxt) {}
	PolicyControlContext& context;
};

KratePolicy::KratePolicy(KratePolicy&& rhs) = default;
KratePolicy& KratePolicy::operator=(KratePolicy&& rhs) = default;

KratePolicy::KratePolicy(const KratePolicy& rhs)
{
	if (rhs.pimpl) {
		pimpl.reset(new Private(*rhs.pimpl));
	}
}

KratePolicy& KratePolicy::operator=(const KratePolicy& rhs)
{
	if (!rhs.pimpl) {
		pimpl.reset();
	} else {
		pimpl.reset(new Private(*rhs.pimpl));
	}

	return *this;
}

KratePolicy::KratePolicy(PolicyControlContext& ctx) :
	pimpl(new Private(ctx))
{
}

KratePolicy::~KratePolicy()
{
}

int KratePolicy::createKrate(const std::string& name, const std::string& setupWizAppid)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("KratePolicy::createKrate", name, setupWizAppid);

	return status.get();
}

int KratePolicy::removeKrate(const std::string& name)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("KratePolicy::removeKrate", name);

	return status.get();
}

int KratePolicy::getKrateState(const std::string& name)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { 0 };

	status = context.methodCall<int>("KratePolicy::getKrateState", name);

	return status.get();
}

std::vector<std::string> KratePolicy::getKrateList(int state)
{
	PolicyControlContext& context = pimpl->context;

	Status<std::vector<std::string>> status { std::vector<std::string>() };

	status = context.methodCall<std::vector<std::string>>("KratePolicy::getKrateList", state);

	return status.get();
}

} // namespace DevicePolicyManager
