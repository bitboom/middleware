CONFIGURE_FILE(
		${SERVICE_NAME}.pc.in
		${SERVICE_NAME}.pc
		@ONLY
)

INSTALL(
	FILES
		${SERVICE_NAME}.pc
	DESTINATION
		${LIB_INSTALL_DIR}/pkgconfig)
