/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @file        CertStoreType.h
 * @version     1.0
 * @brief       Identification of certificate domain. Certificate domains
 *              were defined in WAC 1.0 documentation.
 */
#ifndef _VALIDATION_CORE_CERTSTORETYPE_H_
#define _VALIDATION_CORE_CERTSTORETYPE_H_

#include <string>

#include <cert-svc/ccert.h>

namespace ValidationCore {
namespace CertStoreId {
typedef unsigned int Type;

// RootCA certificates for developer mode.
const Type TIZEN_DEVELOPER = CERTSVC_VISIBILITY_DEVELOPER;
// RootCA certificates for author signatures.
const Type TIZEN_TEST = 1 << 1;
const Type TIZEN_VERIFY = 1 << 2;
const Type TIZEN_STORE = 1 << 3;
const Type TIZEN_REVOKED = 1 << 4;
// RootCA's visibility level : public
const Type VIS_PUBLIC = CERTSVC_VISIBILITY_PUBLIC;
// RootCA's visibility level : partner
const Type VIS_PARTNER = CERTSVC_VISIBILITY_PARTNER;
// RootCA's visibility level : platform
const Type VIS_PLATFORM = CERTSVC_VISIBILITY_PLATFORM;

class Set {
public:
	Set();
	virtual ~Set();

	void add(Type second);
	bool contains(Type second) const;
	bool isContainsVis() const;
	bool isEmpty() const;

	std::string typeToString() const;

private:
	Type m_certificateStorage;
};

} // namespace CertStoreId
} // namespace ValidationCore

#endif //  _VALIDATION_CORE_CERTSTORETYPE_H_
