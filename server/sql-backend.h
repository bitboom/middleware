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

#ifndef __DPM_SQL_BACKEND_H__
#define __DPM_SQL_BACKEND_H__
#include <memory>
#include <vector>

#include <klay/exception.h>
#include <klay/db/connection.h>
#include <klay/db/statement.h>
#include <klay/db/column.h>

#include "policy-storage.h"

class SQLBackend : public PolicyStorageBackend {
public:
	int open(const std::string& location);
	void close();

	int define(const std::string& name, DataSetInt& value);

	bool strictize(int id, DataSetInt& value, uid_t domain);

	void update(int id, const std::string& admin, uid_t domain, const DataSetInt& value);

	int enroll(const std::string& name, uid_t domain);
	int unenroll(const std::string& name, uid_t domain);

	std::vector<uid_t> fetchDomains();

private:
	std::shared_ptr<database::Connection> database;
};

#endif //__DPM_SQL_BACKEND_H__
