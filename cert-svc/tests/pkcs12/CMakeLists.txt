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
# @author      Jacek Migacz (j.migacz@samsung.com)
# @version     1.0
# @brief       This package provides bacis check of internal OpenSSL's PKCS#12 routines.
#

PKG_CHECK_MODULES(PKCS12_TEST_DEP
    REQUIRED
    )

SET(PKCS12_TEST_SOURCES
    pkcs12_test.cpp
    new_test_cases.cpp
    test-common.cpp
    )

INCLUDE_DIRECTORIES(
    ${PKCS12_TEST_DEP_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/vcore/src
    .
    )

ADD_EXECUTABLE(${TARGET_PKCS12_TEST} ${PKCS12_TEST_SOURCES} ${DPL_TEST_SOURCES})

TARGET_LINK_LIBRARIES(${TARGET_PKCS12_TEST}
    ${TARGET_VCORE_LIB}
    ${TEST_DEP_LIBRARIES}
    ${PKCS12_TEST_DEP_LIBRARIES}
    )

INSTALL(TARGETS ${TARGET_PKCS12_TEST}
    DESTINATION ${BIN_DIR}
    PERMISSIONS OWNER_READ
                OWNER_WRITE
                OWNER_EXECUTE
                GROUP_READ
                GROUP_EXECUTE
                WORLD_READ
                WORLD_EXECUTE
    )

INSTALL(
    FILES
        without_pass.p12
        Invalidcrt.crt
        Testing.crt
        test.pfx
        wifi-server.pem

        # generated using openssl 1.1 in a fresh bash in a fresh dir, then copying wifiserver.ptx over:
        # CONSTANTS="-set_serial 0 -days $((($(date -d 9999/12/31 +%s) - $(date +%s) + 86399) / 86400))"; openssl req -x509 -subj '/CN=WiFi-Root-CA' -addext 'authorityInfoAccess=caIssuers;URI:http://server.wifilabs.local/ca/ca/wifirootca.crt' $CONSTANTS -newkey rsa:1024 -nodes -out root.pem -keyout root.key && openssl req -new -subj '/CN=WiFi-Intermediate-CA-srv' -newkey rsa:1024 -nodes -out intermediate.csr -keyout intermediate.key && openssl x509 -req -extfile <(echo 'authorityInfoAccess=caIssuers;URI:http://server.wifilabs.local/ca/ca/wifirootca.crt'; echo 'basicConstraints=critical,CA:true') $CONSTANTS -in intermediate.csr -CA root.pem -CAkey root.key -out intermediate.pem && openssl req -new -subj '/CN=wifi-server' -newkey rsa:1024 -nodes -out leaf.csr -keyout leaf.key && openssl x509 -req -extfile <(echo 'authorityInfoAccess=caIssuers;URI:http://server.wifilabs.local/ca/ca/wifiimcasrv.crt'; echo 'basicConstraints=CA:false') $CONSTANTS -in leaf.csr -CA intermediate.pem -CAkey intermediate.key -out leaf.pem && openssl pkcs12 -export -in <(cat leaf.pem intermediate.pem root.pem) -password pass:wifi -out wifiserver.pfx -inkey leaf.key
        wifiserver.pfx

        wifiuser.p12
    DESTINATION ${CERT_SVC_TESTS}/p12
)
