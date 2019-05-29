#  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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
#
# @file        CMakeLists.txt
# @author      Sangwan Kwon (sangwan.kwon@samsung.com)
# @breif       Make trust anchor library
#
PKG_CHECK_MODULES(${TARGET_TANCHOR_LIB}_DEP REQUIRED klay
													 openssl1.1)

SET(${TARGET_TANCHOR_LIB}_SRCS ${TANCHOR_SRC}/init-lib.cpp
							   ${TANCHOR_SRC}/exception.cpp
							   ${TANCHOR_SRC}/api.cpp
							   ${TANCHOR_SRC}/certificate.cpp
							   ${TANCHOR_SRC}/file-system.cpp
							   ${TANCHOR_SRC}/logic.cpp
							   ${TANCHOR_SRC}/trust-anchor.cpp)

INCLUDE_DIRECTORIES(SYSTEM ${TANCHOR_INCLUDE}
						   ${TANCHOR_SRC}
						   ${${TARGET_TANCHOR_LIB}_DEP_INCLUDE_DIRS})

ADD_LIBRARY(${TARGET_TANCHOR_LIB} SHARED ${${TARGET_TANCHOR_LIB}_SRCS})

SET_TARGET_PROPERTIES(${TARGET_TANCHOR_LIB}
	PROPERTIES COMPILE_FLAGS "-D_GNU_SOURCE -fPIC -fvisibility=hidden"
			   SOVERSION ${API_VERSION}
			   VERSION ${LIB_VERSION})

TARGET_LINK_LIBRARIES(${TARGET_TANCHOR_LIB}
	${${TARGET_TANCHOR_LIB}_DEP_LIBRARIES})

INSTALL(TARGETS ${TARGET_TANCHOR_LIB}
		DESTINATION ${LIB_INSTALL_DIR})
