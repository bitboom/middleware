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

#ifndef __KRATE_POLICY__
#define __KRATE_POLICY__

#include "policy-context.hxx"

namespace DevicePolicyManager {

class KratePolicy {
public:
	enum State {
		Locked      = 0x01,
		Running     = 0x02,
	};

	KratePolicy(PolicyControlContext& ctxt);
	~KratePolicy();

	KratePolicy(const KratePolicy& rhs);
	KratePolicy(KratePolicy&& rhs);

	KratePolicy& operator=(const KratePolicy& rhs);
	KratePolicy& operator=(KratePolicy&& rhs);

	int createKrate(const std::string& name, const std::string& setupWizardAppid);
	int removeKrate(const std::string& name);

	int getKrateState(const std::string& name);

	std::vector<std::string> getKrateList(int state);

private:
	struct Private;
	std::unique_ptr<Private> pimpl;
};

} // namespace DevicePolicyManager
#endif // __KRATE_POLICY__
