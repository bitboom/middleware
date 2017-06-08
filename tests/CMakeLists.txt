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
					 password.c
					 zone.c
)

PKG_CHECK_MODULES(API_TEST_DEPS REQUIRED dpm glib-2.0)

INCLUDE_DIRECTORIES(SYSTEM ${API_TEST_DEPS_INCLUDE_DIRS} ${DPM_TESTS})

ADD_EXECUTABLE(${API_TEST_TARGET} ${API_TEST_SOURCES})

TARGET_LINK_LIBRARIES(${API_TEST_TARGET} ${API_TEST_DEPS_LIBRARIES} pthread)

INSTALL(TARGETS ${API_TEST_TARGET} DESTINATION bin)
