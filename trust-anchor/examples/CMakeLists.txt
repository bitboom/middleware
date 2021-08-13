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
# @file      CMakeLists.txt
# @author    Sangwan kwon (sangwan.kwon@samsung.com)
#

INCLUDE_DIRECTORIES(SYSTEM ${TANCHOR_INCLUDE})

FUNCTION(BUILD_EXAMPLE EXAMPLE_NAME EXAMPLE_SRCS)
	# just for build test
	ADD_EXECUTABLE(${EXAMPLE_NAME} ${EXAMPLE_SRCS})
	TARGET_LINK_LIBRARIES(${EXAMPLE_NAME} ${TARGET_TANCHOR_LIB})
	INSTALL(FILES ${EXAMPLE_SRCS}
			DESTINATION  ${TANCHOR_EXAMPLE})
ENDFUNCTION(BUILD_EXAMPLE)

BUILD_EXAMPLE("tanchor-example-installer" installer.c)
BUILD_EXAMPLE("tanchor-example-launcher" launcher.c)
