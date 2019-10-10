#  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License

SET(OSQUERY_CODEGEN_PATH "${CMAKE_SOURCE_DIR}/tools/codegen")
SET(OSQUERY_TABLES_PATH "${CMAKE_SOURCE_DIR}")
SET(OSQUERY_GENERATED_PATH "${CMAKE_BINARY_DIR}/generated")

SET(GENERATED_TABLES "")

FILE(GLOB TABLE_FILES "${CMAKE_SOURCE_DIR}/specs/*.table")
FILE(GLOB TABLE_FILES_LINUX "${CMAKE_SOURCE_DIR}/specs/linux/*.table")
FILE(GLOB TABLE_FILES_UTILITY "${CMAKE_SOURCE_DIR}/specs/utility/*.table")
LIST(APPEND TABLE_FILES ${TABLE_FILES_LINUX})
LIST(APPEND TABLE_FILES ${TABLE_FILES_UTILITY})

IF(DEFINED GBS_BUILD)
	FILE(GLOB TABLE_FILES_TIZEN "${CMAKE_SOURCE_DIR}/specs/tizen/*.table")
	LIST(APPEND TABLE_FILES ${TABLE_FILES_TIZEN})
ENDIF(DEFINED GBS_BUILD)

FILE(GLOB TABLE_FILES_TEMPLATES "${CMAKE_SOURCE_DIR}/tools/codegen/templates/*.in")
SET(GENERATION_DEPENDENCIES "${OSQUERY_CODEGEN_PATH}/gentable.py"
							"${OSQUERY_CODEGEN_PATH}/amalgamate.py"
							"${OSQUERY_TABLES_PATH}/specs/blacklist")

LIST(APPEND GENERATION_DEPENDENCIES ${TABLE_FILES_TEMPLATES})

FOREACH(TABLE_FILE ${TABLE_FILES})
	SET(TABLE_FILE_GEN ${TABLE_FILE})
	STRING(REPLACE "${OSQUERY_TABLES_PATH}/specs"
				   "${OSQUERY_GENERATED_PATH}/tables"
				   TABLE_FILE_GEN
				   ${TABLE_FILE_GEN})
	STRING(REPLACE "linux/" "" TABLE_FILE_GEN ${TABLE_FILE_GEN})
	STRING(REPLACE "" "" TABLE_FILE_GEN ${TABLE_FILE_GEN})
	STRING(REPLACE ".table" ".cpp" TABLE_FILE_GEN ${TABLE_FILE_GEN})
	ADD_CUSTOM_COMMAND(
		OUTPUT ${TABLE_FILE_GEN}
		COMMAND
			python "${OSQUERY_CODEGEN_PATH}/gentable.py" "${TABLE_FILE}" "${TABLE_FILE_GEN}" "$ENV{DISABLE_BLACKLIST}"
		DEPENDS
			${TABLE_FILE} ${GENERATION_DEPENDENCIES}

		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
	LIST(APPEND GENERATED_TABLES ${TABLE_FILE_GEN})
ENDFOREACH()

SET(AMALGAMATION_FILE_GEN "${OSQUERY_GENERATED_PATH}/amalgamation.cpp")
ADD_CUSTOM_COMMAND(
	OUTPUT ${AMALGAMATION_FILE_GEN}
	COMMAND
		python "${OSQUERY_CODEGEN_PATH}/amalgamate.py"
			--templates "${OSQUERY_CODEGEN_PATH}/templates"
			--sources "${OSQUERY_GENERATED_PATH}"
			--output "${AMALGAMATION_FILE_GEN}"
	DEPENDS
		${GENERATED_TABLES}
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

ADD_LIBRARY(osquery_generated_tables OBJECT "${AMALGAMATION_FILE_GEN}")
