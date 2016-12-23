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

#ifndef __ADMINISTRATION_POLICY__
#define __ADMINISTRATION_POLICY__

#include "policy-context.hxx"

namespace DevicePolicyManager {

class AdministrationPolicy {
public:
	AdministrationPolicy(PolicyControlContext& ctxt);
	~AdministrationPolicy();

	AdministrationPolicy(const AdministrationPolicy& rhs);
	AdministrationPolicy(AdministrationPolicy&& rhs);

	AdministrationPolicy& operator=(const AdministrationPolicy& rhs);
	AdministrationPolicy& operator=(AdministrationPolicy&& rhs);

	int registerPolicyClient(const std::string& name, uid_t uid);
	int deregisterPolicyClient(const std::string& name, uid_t uid);

private:
	struct Private;
	std::unique_ptr<Private> pimpl;
};

} // namespace DevicePolicyManager
#endif // __ADMINISTRATION_POLICY__
