# Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
# @file        CMakeLists.txt
# @author      Kyungwook tak (k.tak@samsung.com)
# @author      Sangwan Kwon (sangwan.kwon@samsung.com)
#

SET(PWD ${CMAKE_CURRENT_SOURCE_DIR})
SET(CA_BUNDLE_DIGEST "c0c04242cc04632a3227622cdcc22e3d")

EXECUTE_PROCESS(
	COMMAND
		${PWD}/check_ca_bundle_digest.sh
		${TZ_SYS_CA_BUNDLE}
	OUTPUT_VARIABLE CURRENT_DIGEST
	)

IF (${CURRENT_DIGEST} STREQUAL ${CA_BUNDLE_DIGEST})
	MESSAGE("ca-certificate digest is match with static db.")

	INSTALL(FILES
		resource/certs-meta-0.0.4.db
		DESTINATION ${CERT_SVC_DB_PATH}
		RENAME certs-meta.db
		)

ELSE (${CURRENT_DIGEST} STREQUAL ${CA_BUNDLE_DIGEST})
	MESSAGE("${CURRENT_DIGEST} digest is not match with static db.")
	MESSAGE("Add ssl table to certs-meta.db")

	EXECUTE_PROCESS(
		COMMAND
			${PWD}/cert_svc_create_clean_store_db.sh
			${PWD}/cert_svc_store_db.sql
			${PWD}/certs-meta.db
		RESULT_VARIABLE ERROR_CODE
		)

	IF (ERROR_CODE)
		MESSAGE(FATAL_ERROR "[${ERROR_CODE}] Failed to create clean store db")
	ENDIF (ERROR_CODE)

	EXECUTE_PROCESS(
		COMMAND
			${PWD}/initialize_store_db.sh
			${PWD}/certs-meta.db
			${TZ_SYS_CA_CERTS}
		RESULT_VARIABLE ERROR_CODE
		)

	IF (ERROR_CODE)
		MESSAGE(FATAL_ERROR "[${ERROR_CODE}] Failed to initialize store db")
	ENDIF (ERROR_CODE)

	INSTALL(FILES
		${PWD}/certs-meta.db
		DESTINATION ${CERT_SVC_DB_PATH}
		)

ENDIF (${CURRENT_DIGEST} STREQUAL ${CA_BUNDLE_DIGEST})
