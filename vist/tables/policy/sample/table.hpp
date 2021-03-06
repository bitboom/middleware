/*
 *  Copyright (c) 2020-present Samsung Electronics Co., Ltd All Rights Reserved
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
 * Query example
 * - SELECT * FROM sample_policy
 * - UPDATE sample_policy SET sample_int_policy = 99
 * - UPDATE sample_policy SET sample_str_policy = 'TEST_VALUE'
 */

#include <vist/logger.hpp>
#include <vist/table/dynamic-table.hpp>

namespace vist {
namespace table {

class SamplePolicyTable final : public DynamicTable {
public:
	void init();

private:
	TableColumns columns() const override;
	QueryData select(QueryContext&) override;
	QueryData update(QueryContext&, const PluginRequest& request) override;
};

} // namespace table
} // namespace vist
