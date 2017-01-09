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
#include <memory>
#include <vector>
#include <unordered_map>

#include <klay/exception.h>
#include <klay/db/connection.h>
#include <klay/db/statement.h>
#include <klay/db/column.h>
#include <klay/audit/logger.h>

#include "observer.h"

typedef database::Statement DataSet;

class PolicyStorage {
public:
	static void open(const std::string& location);
	static DataSet prepare(const std::string& query);
	static void close();
	static void apply();

	static void addStorageEventListenr(Observer* listener);
	static void removeStorageEventListener(Observer* listener);

	static int prepareStorage(const std::string& admin, uid_t domain);
	static int removeStorage(const std::string& admin, uid_t domain);

private:
	static std::vector<uid_t> getManagedDomainList();

private:
	static Observerable listeners;
	static std::shared_ptr<database::Connection> database;
};

#endif //__DPM_POLICY_STORAGE_H__
