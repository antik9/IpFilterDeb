cmake_minimum_required(VERSION 3.2)

find_package(GTest)

if (GTest_FOUND)
	add_executable(bulk_test tests.cpp)

	set_target_properties(bulk_test PROPERTIES
   	 	CXX_STANDARD 14
    	CXX_STANDARD_REQUIRED ON
    	COMPILE_OPTIONS -Wpedantic -Wall -Wextra
	)

	target_link_libraries(bulk_test
    	pthread
    	boost_system
    	${GTEST_BOTH_LIBRARIES}
	    bulk
	    async
	    server
	)

	enable_testing()

	add_test(bulk_test bulk_test)
endif()
