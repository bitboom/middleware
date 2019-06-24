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
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or ManagerImplied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/*
 * @file manager_impl.h
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief Implementation interface of osquery manager
 */

#include <osquery_manager.h>

#include <string>
#include <vector>

namespace osquery {

/// Singleton class
class ManagerImpl final {
public:
	ManagerImpl(const ManagerImpl&) = delete;
	ManagerImpl& operator=(const ManagerImpl&) = delete;

	ManagerImpl(ManagerImpl&&) noexcept = default;
	ManagerImpl& operator=(ManagerImpl&&) noexcept = default;

	static ManagerImpl& instance();

	Rows execute(const std::string& query);
	void subscribe(const std::string& table, const Callback& callback);

	std::vector<std::string> tables(void) noexcept;
	std::vector<std::string> columns(const std::string& table) noexcept;

private:
	ManagerImpl();
	~ManagerImpl() noexcept;
};

} // namespace osquery
