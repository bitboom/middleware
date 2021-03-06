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
 * @file        certificate.hxx
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <string>

namespace tanchor {

class Certificate {
public:
	explicit Certificate(const std::string &path);
	virtual ~Certificate(void) = default;

	Certificate(const Certificate &) = delete;
	Certificate(Certificate &&) = delete;
	Certificate &operator=(const Certificate &) = delete;
	Certificate &operator=(Certificate &&) = delete;

	std::string getSubjectNameHash() const;
	std::string getCertificateData() const;

private:
	std::string parseData(const std::string &data) const;

	std::string m_path;
};

} // namespace tanchor
