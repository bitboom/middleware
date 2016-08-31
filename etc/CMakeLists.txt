SET(ETC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

INSTALL(FILES
    ${ETC_DIR}/schema.xsd
    DESTINATION ${CERT_SVC_RO_PATH}
    )

MESSAGE("Add ssl table to certs-meta.db")

EXECUTE_PROCESS(
    COMMAND
        ${ETC_DIR}/cert_svc_create_clean_store_db.sh
        ${ETC_DIR}/cert_svc_store_db.sql
        ${ETC_DIR}/certs-meta.db
    RESULT_VARIABLE ERROR_CODE
    )

IF (ERROR_CODE)
    MESSAGE(FATAL_ERROR "[${ERROR_CODE}] Failed to create clean store db")
ENDIF (ERROR_CODE)

EXECUTE_PROCESS(
    COMMAND
        ${ETC_DIR}/initialize_store_db.sh
        ${ETC_DIR}/certs-meta.db
        ${TZ_SYS_CA_CERTS}
    RESULT_VARIABLE ERROR_CODE
    )

IF (ERROR_CODE)
    MESSAGE(FATAL_ERROR "[${ERROR_CODE}] Failed to initialize store db")
ENDIF (ERROR_CODE)

INSTALL(FILES
    ${ETC_DIR}/certs-meta.db
    DESTINATION ${CERT_SVC_DB_PATH}
    )

CONFIGURE_FILE(cert-svc-db-upgrade.sh.in cert-svc-db-upgrade.sh @ONLY)
CONFIGURE_FILE(
    cert-svc-disabled-certs-upgrade.sh.in
    cert-svc-disabled-certs-upgrade.sh @ONLY
    )

INSTALL(FILES
    ${ETC_DIR}/cert-svc-db-upgrade.sh
    ${ETC_DIR}/cert-svc-disabled-certs-upgrade.sh
    DESTINATION ${UPGRADE_SCRIPT_PATH}
    )
