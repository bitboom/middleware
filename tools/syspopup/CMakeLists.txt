CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(org.tizen.dpm-syspopup C)

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(dpm_syspopup_pkgs REQUIRED
	klay
	dlog
	glib-2.0
	bundle
	elementary
	capi-appfw-application
	capi-system-system-settings
	capi-ui-efl-util
	notification
	vconf
	capi-system-info
)

INCLUDE_DIRECTORIES(${dpm_syspopup_pkgs_INCLUDE_DIRS} ${DPM_LIBS})
LINK_DIRECTORIES(${dpm_syspopup_pkgs_LIBRARY_DIRS})

SET(PKG_NAME "${PROJECT_NAME}")
SET(PKG_SRC	src/main.cpp
		src/application.cpp
		src/interface.cpp)

SET(PKG_POPUP_SRC
		src/popup/bluetooth.cpp
		src/popup/browser.cpp
		src/popup/camera.cpp
		src/popup/clipboard.cpp
		src/popup/krate-create.cpp
		src/popup/krate-remove.cpp
		src/popup/location.cpp
		src/popup/message-retrieving.cpp
		src/popup/message-sending.cpp
		src/popup/microphone.cpp
		src/popup/nfc.cpp
		src/popup/package-installation.cpp
		src/popup/package-uninstallation.cpp
		src/popup/password.cpp
		src/popup/popimap-email.cpp
		src/popup/screen-capture.cpp
		src/popup/settings-changes.cpp
		src/popup/wifi.cpp
		src/popup/wifi-hotspot.cpp
		src/popup/usb-client.cpp
		)

SET(PKG_UI_SRC
		src/ui/widget.cpp
		src/ui/popup.cpp
		src/ui/window.cpp
		src/ui/button.cpp
		src/ui/layout.cpp
		src/ui/image.cpp
		src/ui/notification.cpp
		src/ui/appcontrol.cpp)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")

ADD_EXECUTABLE(${PKG_NAME} ${PKG_SRC} ${PKG_POPUP_SRC} ${PKG_UI_SRC})
INSTALL(TARGETS ${PKG_NAME} DESTINATION ${APP_INSTALL_PREFIX}/${PKG_NAME}/bin)

TARGET_LINK_LIBRARIES(${PKG_NAME} ${dpm_syspopup_pkgs_LIBRARIES})

#Install app data
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PKG_NAME}.xml DESTINATION ${APP_SHARE_PACKAGES_DIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/${PKG_NAME}.png DESTINATION /usr/share/icons/default/small)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res/images/ DESTINATION ${APP_INSTALL_PREFIX}/${PKG_NAME}/res/images FILES_MATCHING PATTERN "*.png")

# i18n
ADD_SUBDIRECTORY(po)
