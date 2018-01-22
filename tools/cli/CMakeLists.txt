#
# Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
SET(DPM_ADMIN_CLI_TARGET	"dpm-admin-cli")
SET(DPM_SYSPOPUP_TARGET		"dpm-syspopup")
SET(DPM_PREFERENCE_TARGET	"dpm-storage-builder")

SET(DPM_ADMIN_CLI_SOURCES dpm-admin-cli.cpp)

PKG_CHECK_MODULES(DPM_ADMIN_CLI_TARGET_DEP REQUIRED vconf)

ADD_EXECUTABLE(${DPM_ADMIN_CLI_TARGET} ${DPM_ADMIN_CLI_SOURCES})

INCLUDE_DIRECTORIES(SYSTEM ${DPM_LIBS} ${DPM_ADMIN_CLI_TARGET_DEP_INCLUDE_DIRS})

TARGET_LINK_LIBRARIES(${DPM_ADMIN_CLI_TARGET} dpm-pil)

SET_TARGET_PROPERTIES(${DPM_ADMIN_CLI_TARGET} PROPERTIES COMPILE_FLAGS "-fPIE")
SET_TARGET_PROPERTIES(${DPM_ADMIN_CLI_TARGET} PROPERTIES LINK_FLAGS "-pie")

INSTALL(FILES ${DPM_PREFERENCE_TARGET} DESTINATION bin)
INSTALL(FILES ${DPM_SYSPOPUP_TARGET} DESTINATION bin)
INSTALL(TARGETS ${DPM_ADMIN_CLI_TARGET} DESTINATION bin)
