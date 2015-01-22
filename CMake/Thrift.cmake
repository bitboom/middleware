# Target for generating osquery thirft (extensions) code.
SET(OSQUERY_THRIFT_DIR "${CMAKE_BINARY_DIR}/generated/gen-cpp")
SET(OSQUERY_THRIFT_GENERATED_FILES ${OSQUERY_THRIFT_DIR}/Extension.cpp
								   ${OSQUERY_THRIFT_DIR}/Extension.h
								   ${OSQUERY_THRIFT_DIR}/ExtensionManager.cpp
								   ${OSQUERY_THRIFT_DIR}/ExtensionManager.h
								   ${OSQUERY_THRIFT_DIR}/osquery_types.cpp
								   ${OSQUERY_THRIFT_DIR}/osquery_types.h)

# Allow targets to warn if the thrift interface code is not defined.
ADD_DEFINITIONS(-DOSQUERY_THRIFT="${OSQUERY_THRIFT_DIR}")

# For the extensions targets, allow them to include thrift interface headers.
INCLUDE_DIRECTORIES("${OSQUERY_THRIFT_DIR}")
