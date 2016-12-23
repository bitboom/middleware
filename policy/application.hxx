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

#ifndef __APPLICATION_POLICY__
#define __APPLICATION_POLICY__

#include "policy-context.hxx"

namespace DevicePolicyManager {

class ApplicationPolicy {
public:
	enum PrivilegeType {
		PRIVILEGE_TYPE_WRT = 0,
		PRIVILEGE_TYPE_CORE = 1
	};

	enum RestrictionMode {
		RESTRICTION_MODE_ALL = 0x0F,
		RESTRICTION_MODE_INSTALL = 0x01,
		RESTRICTION_MODE_UNINSTALL = 0x02,
		RESTRICTION_MODE_REINSTALL = 0x04,
		RESTRICTION_MODE_MVOE = 0x08
	};

	ApplicationPolicy(PolicyControlContext& ctxt);
	~ApplicationPolicy();

	ApplicationPolicy(const ApplicationPolicy& rhs);
	ApplicationPolicy(ApplicationPolicy&& rhs);

	ApplicationPolicy& operator=(const ApplicationPolicy& rhs);
	ApplicationPolicy& operator=(ApplicationPolicy&& rhs);


	int installPackage(const std::string& pkgpath);
	int uninstallPackage(const std::string& pkgid);

	int setModeRestriction(int mode);
	int unsetModeRestriction(int mode);
	int getModeRestriction();

	int addPrivilegeToBlacklist(int type, const std::string& privilege);
	int removePrivilegeFromBlacklist(int type, const std::string& privilege);
	int checkPrivilegeIsBlacklisted(int type, const std::string& privilege);

private:
	struct Private;
	std::unique_ptr<Private> pimpl;
};

} // namespace DevicePolicyManager
#endif // __APPLICATION_POLICY__
