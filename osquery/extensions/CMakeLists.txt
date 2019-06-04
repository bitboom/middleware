FIND_PROGRAM(THRIFT_COMPILER thrift /usr/local/bin
									/usr/bin
									NO_DEFAULT_PATH)

# Generate the thrift intermediate/interface code.
FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated")
ADD_CUSTOM_COMMAND(
	COMMAND
		${THRIFT_COMPILER} --gen cpp:dense "${CMAKE_SOURCE_DIR}/osquery.thrift"
	DEPENDS
		"${CMAKE_SOURCE_DIR}/osquery.thrift"
	WORKING_DIRECTORY
		"${CMAKE_BINARY_DIR}/generated"
	OUTPUT
		${OSQUERY_THRIFT_GENERATED_FILES})

ADD_OSQUERY_LIBRARY(TRUE osquery_extensions ${OSQUERY_THRIFT_GENERATED_FILES}
											extensions.cpp)

ADD_OSQUERY_TEST(TRUE osquery_extensions_test extensions_tests.cpp)
