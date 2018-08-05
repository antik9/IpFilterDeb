cmake_minimum_required(VERSION 3.2)

find_package(GTest)

if (GTest_FOUND)
	add_executable(print_ip_test tests.cpp)

	set_target_properties(print_ip_test PROPERTIES
   	 	CXX_STANDARD 14
    	CXX_STANDARD_REQUIRED ON
    	COMPILE_OPTIONS -Wpedantic -Wall -Wextra
	)

	target_link_libraries(print_ip_test
    	pthread
    	${GTEST_BOTH_LIBRARIES}
	)

	enable_testing()

	add_test(print_ip_test print_ip_test)
endif()
