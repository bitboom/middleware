# Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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
# @author      Sangwan Kwon (sangwan.kwon@samsung.com)
# @breif       Make trasnport security library
#
PKG_CHECK_MODULES(${TARGET_TRANSEC_LIB}_DEP
	REQUIRED
		klay
)

SET(${TARGET_TRANSEC_LIB}_SRCS
	InitLib.cpp
	AppCustomTrustAnchor.cpp
)

INCLUDE_DIRECTORIES(
	SYSTEM
	.
	${${TARGET_TRANSEC_LIB}_DEP_INCLUDE_DIRS}
)

ADD_LIBRARY(${TARGET_TRANSEC_LIB} SHARED ${${TARGET_TRANSEC_LIB}_SRCS})

# TODO(sangwan.kwon) visibility needed to be hidden
SET_TARGET_PROPERTIES(${TARGET_TRANSEC_LIB}
	PROPERTIES
		COMPILE_FLAGS "-D_GNU_SOURCE -fPIC -fvisibility=default"
		SOVERSION ${SO_VERSION}
		VERSION ${VERSION}
)

TARGET_LINK_LIBRARIES(${TARGET_TRANSEC_LIB}
	${${TARGET_TRANSEC_LIB}_DEP_LIBRARIES}
)
INSTALL(TARGETS ${TARGET_TRANSEC_LIB} DESTINATION ${LIB_INSTALL_DIR})
INSTALL(FILES
			AppCustomTrustAnchor.h
		DESTINATION
			${INCLUDEDIR}/cert-svc/transec
)
