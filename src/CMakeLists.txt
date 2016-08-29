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
# @author      Kyungwook Tak (k.tak@samsung.com)
# @author      Sangwan Kwon (sangwan.kwon@samsung.com)
#

######################### VCORE #########################

IF(TIZEN_PROFILE_MOBILE)
    ADD_DEFINITIONS("-DTIZEN_PROFILE_MOBILE")
    SET(LIB_CERT_CHECKER cert-checker)
ENDIF(TIZEN_PROFILE_MOBILE)

PKG_CHECK_MODULES(VCORE_DEPS
    REQUIRED
    libxml-2.0
    libpcrecpp
    openssl
    xmlsec1
    dlog
    libsystemd-journal
    ${LIB_CERT_CHECKER}
    )

ADD_DEFINITIONS(${VCORE_DEPS_CFLAGS})
ADD_DEFINITIONS(${VCORE_DEPS_CFLAGS_OTHER})

SET(VCORE_SOURCES
    dpl/core/src/assert.cpp
    dpl/core/src/exception.cpp
    dpl/core/src/singleton.cpp
    dpl/core/src/colors.cpp

    dpl/log/src/abstract_log_provider.cpp
    dpl/log/src/old_style_log_provider.cpp
    dpl/log/src/dlog_log_provider.cpp
    dpl/log/src/journal_log_provider.cpp
    dpl/log/src/log.cpp

    vcore/api.cpp
    vcore/Base64.cpp
    vcore/Certificate.cpp
    vcore/CertificateCollection.cpp
    vcore/CertificateConfigReader.cpp
    vcore/CertificateLoader.cpp
    vcore/CertStoreType.cpp
    vcore/ReferenceValidator.cpp
    vcore/SaxReader.cpp
    vcore/SignatureData.cpp
    vcore/SignatureFinder.cpp
    vcore/SignatureReader.cpp
    vcore/TimeConversion.cpp
    vcore/ValidatorFactories.cpp
    vcore/SignatureValidator.cpp
    vcore/XmlsecAdapter.cpp
    vcore/pkcs12.cpp
    vcore/exception.cpp
    vcore/Client.cpp
    vcore/Ocsp.cpp
    vcore/CryptoInit.cpp
    vcore/PluginHandler.cpp
    vcore/init-lib.cpp
    )

SET(VCORE_INCLUDES
    ./
    dpl/core/include
    dpl/log/include
    )

INCLUDE_DIRECTORIES(
    SYSTEM
    ${VCORE_DEPS_INCLUDE_DIRS}
    ${VCORE_INCLUDES}
    )

ADD_LIBRARY(${TARGET_VCORE_LIB} SHARED ${VCORE_SOURCES})

# TODO: visibility needed to be hidden
SET_TARGET_PROPERTIES(${TARGET_VCORE_LIB}
    PROPERTIES
        COMPILE_FLAGS "-D_GNU_SOURCE -fPIC -fvisibility=default"
        SOVERSION ${SO_VERSION}
        VERSION ${VERSION})

TARGET_LINK_LIBRARIES(${TARGET_VCORE_LIB}
    ${VCORE_DEPS_LIBRARIES}
    ${TARGET_CERT_SVC_LIB}
    -ldl
    )

INSTALL(TARGETS ${TARGET_VCORE_LIB}
    DESTINATION ${LIB_INSTALL_DIR}
    )

######################### CERT_SERVER #########################

PKG_CHECK_MODULES(CERT_SERVER_DEP
    REQUIRED
    dlog
    sqlite3
    db-util
    libsystemd-daemon
    key-manager
    )

SET(CERT_SERVER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/server)

SET(CERT_SERVER_SRC
    ${CERT_SERVER_DIR}/src/cert-server-main.c
    ${CERT_SERVER_DIR}/src/cert-server-logic.c
    ${CERT_SERVER_DIR}/src/cert-server-db.c
    )

INCLUDE_DIRECTORIES(
    SYSTEM
    ./
    ${CERT_SERVER_DEP_INCLUDE_DIRS}
    ${CERT_SERVER_DIR}/include
    )

SET_SOURCE_FILES_PROPERTIES(
    ${CERT_SERVER_SRC}
    PROPERTIES
        COMPILE_FLAGS "-D_GNU_SOURCE -fvisibility=hidden -fPIE"
    )

ADD_EXECUTABLE(${TARGET_CERT_SERVER} ${CERT_SERVER_SRC})

TARGET_LINK_LIBRARIES(${TARGET_CERT_SERVER}
    ${CERT_SERVER_DEP_LIBRARIES}
    -pie
    )

INSTALL(TARGETS ${TARGET_CERT_SERVER} DESTINATION ${BIN_DIR})

######################### API HEADERS #########################

INSTALL(FILES
    vcore/SignatureValidator.h
    vcore/SignatureFinder.h
    vcore/Certificate.h
    vcore/SignatureData.h
    vcore/CertStoreType.h
    vcore/exception.h
    vcore/ValidatorPluginApi.h
    vcore/Error.h
    DESTINATION ${INCLUDEDIR}/cert-svc/vcore
    )

INSTALL(FILES
    cert-svc/ccert.h
    cert-svc/cinstance.h
    cert-svc/cerror.h
    cert-svc/cpkcs12.h
    cert-svc/cprimitives.h
    cert-svc/cstring.h
    DESTINATION ${INCLUDEDIR}/cert-svc/cert-svc
    )
