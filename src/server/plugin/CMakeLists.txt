### SW-BACKEND ############################################################################
PKG_CHECK_MODULES(SW_BACKEND_DEP
    REQUIRED
    openssl
    )

INCLUDE_DIRECTORIES(
    SYSTEM
    ${SW_BACKEND_DEP_INCLUDE_DIRS}
    ${COMMON_PATH}/include
    ${SERVER_PATH}/service/include
    ${DPL_PATH}/core/include
    ${DPL_PATH}/log/include
    ${PLUGIN_PATH}
    )

SET(SW_BACKEND_SOURCES
    generic-backend/ipassword.cpp
    generic-backend/password-file-buffer.cpp
    sw-backend/password-file.cpp
    )

ADD_LIBRARY(${TARGET_SW_BACKEND} SHARED ${SW_BACKEND_SOURCES})

SET_TARGET_PROPERTIES(
    ${TARGET_SW_BACKEND}
    PROPERTIES
        COMPILE_FLAGS "-D_GNU_SOURCE -fPIC -fvisibility=default"
        SOVERSION ${API_VERSION}
        VERSION ${VERSION}
    )

TARGET_LINK_LIBRARIES(${TARGET_SW_BACKEND} ${SW_BACKEND_DEP_LIBRARIES} ${TARGET_COMMON})

INSTALL(TARGETS ${TARGET_SW_BACKEND} DESTINATION ${PLUGIN_DIR})

### TZ-BACKEND ############################################################################
PKG_CHECK_MODULES(TZ_BACKEND_DEP
    REQUIRED
    )

INCLUDE_DIRECTORIES(
    SYSTEM
    ${TZ_BACKEND_DEP_INCLUDE_DIRS}
    ${COMMON_PATH}/include
    ${SERVER_PATH}/service/include
    ${DPL_PATH}/core/include
    ${DPL_PATH}/log/include
    ${PLUGIN_PATH}
    )

SET(TZ_BACKEND_SOURCES
    generic-backend/ipassword.cpp
    generic-backend/password-file-buffer.cpp
    tz-backend/password-file.cpp
    )

ADD_LIBRARY(${TARGET_TZ_BACKEND} SHARED ${TZ_BACKEND_SOURCES})

SET_TARGET_PROPERTIES(
    ${TARGET_TZ_BACKEND}
    PROPERTIES
        COMPILE_FLAGS "-D_GNU_SOURCE -fPIC -fvisibility=default"
        SOVERSION ${API_VERSION}
        VERSION ${VERSION}
    )

TARGET_LINK_LIBRARIES(${TARGET_TZ_BACKEND} ${TZ_BACKEND_DEP_LIBRARIES} ${TARGET_COMMON})

#INSTALL(TARGETS ${TARGET_TZ_BACKEND} DESTINATION ${PLUGIN_DIR})
