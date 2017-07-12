/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        logic.hxx
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <string>
#include <set>
#include <vector>

namespace tanchor {

class Logic {
public:
	explicit Logic(const std::string &basePath);
	virtual ~Logic(void) = default;

	Logic(const Logic &) = delete;
	Logic(Logic &&) = delete;
	Logic &operator=(const Logic &) = delete;
	Logic &operator=(Logic &&) = delete;

	void init(void) const;
	void deinit(bool isRollback) const;

	void makeCustomCerts(void);
	void makeCustomBundle(void);

	void setPkgCertsPath(const std::string &path) const;
	std::string getPkgCertsPath(void) const;

	bool isSystemCertsUsed(void) const;
	bool isSystemCertsModified(void) const;
	void setSystemCertsUsed(void) const;

	void disassociateNS(void) const;
	void mountCustomCerts(void) const;
	void mountCustomBundle(void) const;

private:
	std::string getUniqueCertName(const std::string &name) const;
	bool isPkgCertsValid(const std::string &path) const;

	std::string m_customBasePath;
	std::string m_customCertsPath;
	std::string m_customBundlePath;

	std::set<std::string> m_customCertNameSet;
	std::vector<std::string> m_customCertsData;
};

} // namespace tanchor
