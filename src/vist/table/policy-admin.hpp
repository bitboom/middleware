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

#include <osquery/tables.h>

namespace vist {
namespace table {

using namespace osquery;

class PolicyAdminTable final : public TablePlugin {
public:
	static void Init();

private:
	TableColumns columns() const override;
	TableRows generate(QueryContext&) override;
	QueryData delete_(QueryContext&, const PluginRequest& request) override;
	QueryData insert(QueryContext&, const PluginRequest& request) override;
	QueryData update(QueryContext&, const PluginRequest& request) override;
};

} // namespace table
} // namespace vist
