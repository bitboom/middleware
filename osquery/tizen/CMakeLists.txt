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

ADD_OSQUERY_LIBRARY(TRUE osquery_property property/property.cpp)

ADD_OSQUERY_LIBRARY(TRUE osquery_manager manager/manager.cpp
										 manager/manager_impl.cpp)

ADD_OSQUERY_LIBRARY(TRUE osquery_notification notification/notification.cpp)

FILE(GLOB OSQUERY_TIZEN_TESTS "*/tests/*.cpp")
ADD_OSQUERY_TEST(TRUE ${OSQUERY_TIZEN_TESTS})

IF(DEFINED GBS_BUILD)
	SET(GBS_ONLY_PACKAGES klay
						  dpm-pil
						  capi-base-common
						  capi-system-info
						  capi-network-wifi-manager)

	INCLUDE(FindPkgConfig)
	PKG_CHECK_MODULES(GBS_DEPS REQUIRED ${GBS_ONLY_PACKAGES})
	INCLUDE_DIRECTORIES(SYSTEM ${GBS_DEPS_INCLUDE_DIRS})

	ADD_OSQUERY_LINK(FALSE ${GBS_DEPS_LIBRARIES})
	ADD_OSQUERY_LIBRARY(FALSE tizen-dpm-wifi device-policy/wifi/wifi.cpp
											 device-policy/wifi/wifi-impl.cpp)
ENDIF(DEFINED GBS_BUILD)
