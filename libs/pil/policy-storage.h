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

#ifndef __DPM_POLICY_STORAGE_H__
#define __DPM_POLICY_STORAGE_H__
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "observer.h"

class DataSetInt {
public:
	DataSetInt() : value(0) {}
	DataSetInt(int v) : value(v) {}
	virtual ~DataSetInt() {}

	DataSetInt& operator=(int val) {
		value = val;
		return *this;
	}

	operator int() const {
		return value;
	}

	bool operator==(const DataSetInt& rhs) const {
		return value == rhs.value;
	}

	bool operator!=(const DataSetInt& rhs) const {
		return value != rhs.value;
	}

	virtual bool strictize(const DataSetInt& rhs) {
		if (rhs.value < value) {
			value = rhs.value;
			return true;
		}

		return false;
	}

	int value;
};

class PolicyStorageBackend {
public:
	PolicyStorageBackend() {}
	virtual ~PolicyStorageBackend() {}

	virtual int define(const std::string& name, DataSetInt& value) = 0;
	virtual bool strictize(int id, DataSetInt& value, uid_t domain = 0) = 0;
	virtual void update(int id, const std::string& admin, uid_t domain, const DataSetInt& value) = 0;
	virtual int enroll(const std::string& name, uid_t domain) = 0;
	virtual int unenroll(const std::string& name, uid_t domain) = 0;
	virtual std::vector<uid_t> fetchDomains() = 0;
};

class PolicyStorage {
public:
	template<typename T>
	static int define(const std::string& name, T& value) {
		return backend->define(name, value);
	}

	template<typename T>
	static bool strictize(int id, T& value, uid_t domain = 0) {
		return backend->strictize(id, value, domain);
	}

	template<typename T>
	static void update(int id, const std::string& admin, uid_t domain, const T& value) {
		backend->update(id, admin, domain, value);
	}

	static void setBackend(PolicyStorageBackend *pimpl) {
		backend = pimpl;
	}

	static void subscribeEvent(Observer *observer) {
		observers.attach(observer);
	}

	static int enroll(const std::string& name, uid_t domain) {
		return backend->enroll(name, domain);
	}

	static int unenroll(const std::string& name, uid_t domain) {
		int rc = backend->unenroll(name, domain);
		if (rc == 0) {
			observers.notify({domain});
		}

		return rc;
	}

	static std::vector<uid_t> fetchDomains() {
		return backend->fetchDomains();
	}

private:
	static Observerable observers;
	static PolicyStorageBackend *backend;
};

#endif //__DPM_POLICY_STORAGE_H__
