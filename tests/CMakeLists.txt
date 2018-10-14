cmake_minimum_required(VERSION 3.2)

find_package(GTest)

if (GTest_FOUND)
	add_executable(yamr_test tests.cpp)

	set_target_properties(yamr_test PROPERTIES
   	 	CXX_STANDARD 14
   	 	CXX_STANDARD_REQUIRED ON
   	 	COMPILE_OPTIONS -Wpedantic -Wall -Wextra
	)

	target_link_libraries(yamr_test
	    ${GTEST_BOTH_LIBRARIES}
	    map_reduce
	)

	enable_testing()

	add_test(yamr_test yamr_test)
endif()
