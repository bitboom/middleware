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
#include "storage.hxx"

namespace DevicePolicyManager {

struct StoragePolicy::Private {
	Private(PolicyControlContext& ctxt) : context(ctxt) {}
	PolicyControlContext& context;
};

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

StoragePolicy::StoragePolicy(PolicyControlContext& ctx) :
	pimpl(new Private(ctx))
{
}

StoragePolicy::~StoragePolicy()
{
}

int StoragePolicy::wipeData(int type)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("StoragePolicy::wipeData", type);

	return status.get();
}

} //namespace DevicePolicyManager
