# Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
SET(TARGET "dpm-plugin-bluetooth")

SET(PLUGIN_SOURCES "bluetooth.cpp")

SET(DEPENDENCY bluetooth-api
			   capi-network-bluetooth)

PKG_CHECK_MODULES(PLUGIN_DEPS REQUIRED ${DEPENDENCY})

INCLUDE_DIRECTORIES(SYSTEM ${PLUGIN_DEPS_INCLUDE_DIRS})

ADD_LIBRARY(${TARGET} SHARED ${PLUGIN_SOURCES})
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "-fvisibility=default")
TARGET_LINK_LIBRARIES(${TARGET} ${PLUGIN_DEPS_LIBRARIES})

INSTALL(FILES libdpm-plugin-bluetooth.so
		RENAME bluetooth
		DESTINATION ${PLUGIN_INSTALL_DIR})
