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

#ifndef __RUNTIME_NETLINK_AUDIT_RULE_H__
#define __RUNTIME_NETLINK_AUDIT_RULE_H__

#include <vector>

#include <linux/audit.h>

#include <klay/klay.h>

namespace netlink {

class KLAY_EXPORT AuditRule final {
public:
	AuditRule();
	AuditRule(std::vector<char>& buf);
	~AuditRule();

	void setSystemCall(unsigned int syscall);
	void unsetSystemCall(unsigned int syscall);
	void setAllSystemCalls();
	void clearAllSystemCalls();
	std::vector<unsigned int> getSystemCalls() const;

	void setReturn(unsigned int val);
	void unsetReturn();
	unsigned int getReturn() const;
	bool hasReturn() const;

	void setKey(const std::string &name);
	void unsetKey();
	const std::string getkey() const;
	bool hasKey() const;

	const char *data() const
	{
		return buf.data();
	}

	unsigned int size() const
	{
		return buf.size();
	}

private:
	std::vector<char> buf;
};

} // namespace runtime

#endif //!__RUNTIME_NETLINK_AUDIT_RULE_H__
