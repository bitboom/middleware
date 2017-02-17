#
# Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
SET(API_TEST_TARGET	"dpm-api-tests")

SET(API_TEST_SOURCES main.c
					 testbench.c
					 admin.c
					 application.c
					 bluetooth.c
					 manager.c
					 restriction.c
					 security.c
					 wifi.c
)
SET(API_TEST_SOURCES_1		${API_TEST_SOURCES}
					password.c
)
SET(API_TEST_SOURCES_2		${API_TEST_SOURCES_1}
					zone.c
)
INCLUDE_DIRECTORIES(SYSTEM ${DPM_LIBS} ${DPM_TESTS})


ADD_EXECUTABLE(${API_TEST_TARGET}.minimal ${API_TEST_SOURCES})
TARGET_LINK_LIBRARIES(${API_TEST_TARGET}.minimal dpm glib-2.0)
INSTALL(TARGETS ${API_TEST_TARGET}.minimal DESTINATION bin)

# Add "Password" (no TV). (password, location, bluetooth)
IF(NOT DEFINED NO_EXT_PASSWORD)
  ADD_EXECUTABLE(${API_TEST_TARGET}.ext-pwd ${API_TEST_SOURCES_1})
  TARGET_LINK_LIBRARIES(${API_TEST_TARGET}.ext-pwd dpm dpm-ext-password glib-2.0)
  INSTALL(TARGETS ${API_TEST_TARGET}.ext-pwd DESTINATION bin)
 
# Add "Krate" (no TV, WEARABLE). (krate)
IF (NOT DEFINED NO_EXT_KRATE_ZONE)
  ADD_EXECUTABLE(${API_TEST_TARGET} ${API_TEST_SOURCES_2})
  TARGET_LINK_LIBRARIES(${API_TEST_TARGET} dpm dpm-ext-password dpm-ext-krate_zone glib-2.0)
  INSTALL(TARGETS ${API_TEST_TARGET} DESTINATION bin)

ENDIF(NOT DEFINED NO_EXT_KRATE_ZONE)

ELSE(NOT DEFINED NO_EXT_PASSWORD)
IF (NOT DEFINED NO_EXT_KRATE_ZONE)
# It is error if !NO_EXT_KRATE_ZONE + NO_EXT_PASSWORD
	error("NO_EXT_KRATE_ZONE should be defined if NO_EXT_PASSWORD is defined.")
ENDIF(NOT DEFINED NO_EXT_KRATE_ZONE)
ENDIF(NOT DEFINED NO_EXT_PASSWORD)

