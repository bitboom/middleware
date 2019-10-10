/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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
/*
 * @file bluetooth_policy.cpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief Implementation of bluetooth_policy table
 */

#include <string>
#include <memory>
#include <stdexcept>

#include <osquery/sql.h>
#include <osquery/logger.h>
#include <osquery/tables.h>

#include <policyd/core/policy-manager.h>

using namespace policyd;

namespace osquery {
namespace tables {

QueryData genPolicy(QueryContext& context) try {
	auto& manager = PolicyManager::Instance();

	QueryData results;
	if (context.constraints["name"].exists(EQUALS)) { /// where clause
		auto names = context.constraints["name"].getAll(EQUALS);
		for (const auto& name : names) {
			auto ret = manager.get(name);

			Row r;
			r["name"] = TEXT(name);
			r["value"] = TEXT(ret.value);

			results.emplace_back(std::move(r));
		}
	} else { /// select *;
		auto policies = manager.getAll();
		for (auto& policy : policies) {
			Row r;
			r["name"] = TEXT(policy.first);
			r["value"] = TEXT(policy.second);

			results.emplace_back(std::move(r));
		}
	}

	return results;
} catch (...) {
// TODO(Sangwan): Resolve duplicated "ERROR" macro with DPM
//    LOG(ERROR) << "Exception occured";
	Row r;
	return { r };
}

} // namespace tables
} // namespace osquery
