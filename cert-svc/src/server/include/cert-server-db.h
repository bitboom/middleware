/**
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/**
 * @file     cert-server-db.h
 * @author   Kyungwook Tak (k.tak@samsung.com)
 * @version  1.0
 * @brief    cert-server db utils.
 */

#ifndef CERT_SERVER_DB_H_
#define CERT_SERVER_DB_H_

#include <sqlite3.h>
#include <cert-svc/cerror.h>

extern sqlite3 *cert_store_db;

typedef enum schema_version_t {
	TIZEN_2_4 =  1,
	TIZEN_3_0 =  2
} schema_version;

int initialize_db(void);
void deinitialize_db(void);
int execute_insert_update_query(const char *query);
int execute_select_query(const char *query, sqlite3_stmt **stmt);
int get_schema_version(schema_version *version);
int set_schema_version(schema_version version);

#endif // CERT_SERVER_DB_H_
