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
#include <klay/error.h>
#include <klay/exception.h>

#include <klay/netlink/audit-rule.h>

namespace netlink {

AuditRule::AuditRule() :
	buf(sizeof(struct audit_rule_data))
{
	auto r = (struct audit_rule_data *)buf.data();
	r->flags = AUDIT_FILTER_EXIT;
	r->action = AUDIT_ALWAYS;
}

AuditRule::AuditRule(std::vector<char> &buf) :
	buf(buf)
{

}

AuditRule::~AuditRule()
{

}

void AuditRule::setSystemCall(unsigned int syscall)
{
	auto r = (struct audit_rule_data *)buf.data();
	r->mask[syscall / 32] |= 1 << (syscall % 32);
}

void AuditRule::unsetSystemCall(unsigned int syscall)
{
	auto r = (struct audit_rule_data *)buf.data();
	r->mask[syscall / 32] &= ~(1 << (syscall % 32));
}

void AuditRule::setAllSystemCalls()
{
	auto r = (struct audit_rule_data *)buf.data();
	for (auto &m : r->mask) {
		m = 0xffffffff;
	}
}

void AuditRule::clearAllSystemCalls()
{
	auto r = (struct audit_rule_data *)buf.data();
	for (auto &m : r->mask) {
		m = 0;
	}
}

std::vector<unsigned int> AuditRule::getSystemCalls() const
{
	auto r = (struct audit_rule_data *)buf.data();
	std::vector<unsigned int> ret;

	for (int i = 0; i < AUDIT_BITMASK_SIZE; i++) {
		for (int j = 0, k = 1; j < 32; j++, k <<= 1) {
			if (r->mask[i] & k) {
				ret.push_back(i << 5 | j);
			}
		}
	}
	return ret;
}

void AuditRule::setReturn(unsigned int val)
{
	auto r = (struct audit_rule_data *)buf.data();
	unsigned int i;

	for (i = 0; i < r->field_count; i++) {
		if (r->fields[i] == AUDIT_EXIT) {
			break;
		}
	}
	if (i == r->field_count) {
		r->fields[i] = AUDIT_EXIT;
		r->fieldflags[i] = AUDIT_EQUAL;
		r->field_count++;
	}
	r->values[i] = val;
}

void AuditRule::unsetReturn()
{
	auto r = (struct audit_rule_data *)buf.data();
	unsigned int i;

	for (i = 0; i < r->field_count; i++) {
		if (r->fields[i] == AUDIT_EXIT) {
			break;
		}
	}

	if (i == r->field_count) {
		return;
	}

	std::move(&r->values[i + 1], &r->values[r->field_count], &r->values[i]);
	std::move(&r->fields[i + 1], &r->fields[r->field_count], &r->fields[i]);
	std::move(&r->fieldflags[i + 1], &r->fieldflags[r->field_count], &r->fieldflags[i]);

	r->field_count--;

	r->values[r->field_count] = 0;
	r->fields[r->field_count] = 0;
	r->fieldflags[r->field_count] = 0;
}

unsigned int AuditRule::getReturn() const
{
	auto r = (struct audit_rule_data *)buf.data();
	for (unsigned int i = 0; i < r->field_count; i++) {
		if (r->fields[i] == AUDIT_EXIT) {
			return r->values[i];
		}
	}
	throw runtime::Exception("No rule for return");
}

bool AuditRule::hasReturn() const
{
	auto r = (struct audit_rule_data *)buf.data();
	for (unsigned int i = 0; i < r->field_count; i++) {
		if (r->fields[i] == AUDIT_EXIT) {
			return true;
		}
	}
	return false;
}

void AuditRule::setKey(const std::string &name)
{
	auto r = (struct audit_rule_data *)buf.data();
	unsigned int i;

	for (i = 0; i < r->field_count; i++) {
		if (r->fields[i] == AUDIT_FILTERKEY) {
			break;
		}
	}

	if (i == r->field_count) {
		r->fields[i] = AUDIT_FILTERKEY;
		r->fieldflags[i] = AUDIT_EQUAL;
		r->field_count++;
	}

	// TODO : Calculate the offset. It assummed that there is no field to use buffer

	r->values[i] = name.size();
	r->buflen = name.size();

	buf.resize(sizeof(struct audit_rule_data) + name.size());
	r = (struct audit_rule_data *)buf.data();
	std::copy(name.begin(), name.end(), r->buf);
}

void AuditRule::unsetKey()
{
	auto r = (struct audit_rule_data *)buf.data();
	unsigned int i;

	for (i = 0; i < r->field_count; i++) {
		if (r->fields[i] == AUDIT_FILTERKEY) {
			break;
		}
	}

	if (i == r->field_count) {
		return;
	}

	std::move(&r->values[i + 1], &r->values[r->field_count], &r->values[i]);
	std::move(&r->fields[i + 1], &r->fields[r->field_count], &r->fields[i]);
	std::move(&r->fieldflags[i + 1], &r->fieldflags[r->field_count], &r->fieldflags[i]);

	r->field_count--;

	r->values[r->field_count] = 0;
	r->fields[r->field_count] = 0;
	r->fieldflags[r->field_count] = 0;
}

const std::string AuditRule::getkey() const
{
	auto r = (struct audit_rule_data *)buf.data();
	for (unsigned int i = 0; i < r->field_count; i++) {
		if (r->fields[i] == AUDIT_FILTERKEY) {
			// TODO : Calculate the offset. It assummed that there is no field to use buffer
			return r->buf;
		}
	}

	throw runtime::Exception("No rule for key");
}

bool AuditRule::hasKey() const
{
	auto r = (struct audit_rule_data *)buf.data();
	for (unsigned int i = 0; i < r->field_count; i++) {
		if (r->fields[i] == AUDIT_FILTERKEY) {
			return true;
		}
	}
	return false;
}

} // namespace runtime
