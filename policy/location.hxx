/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __LOCATION_POLICY__
#define __LOCATION_POLICY__

#include "policy-context.hxx"

namespace DevicePolicyManager {

class LocationPolicy {
public:
	LocationPolicy(PolicyControlContext& ctxt);
	~LocationPolicy();

	int setLocationState(bool enable);
	bool getLocationState();

private:
	PolicyControlContext& context;
};

} // namespace DevicePolicyManager

#endif /* __LOCATION_POLICY__ */
