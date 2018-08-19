cmake_minimum_required(VERSION 3.2)

find_package(GTest)

if (GTest_FOUND)
	add_executable(matrix_test tests.cpp)

	set_target_properties(matrix_test PROPERTIES
   	 	CXX_STANDARD 14
    	CXX_STANDARD_REQUIRED ON
    	COMPILE_OPTIONS -Wpedantic -Wall -Wextra
	)

	target_link_libraries(matrix_test
    	pthread
    	${GTEST_BOTH_LIBRARIES}
	)

	enable_testing()

	add_test(matrix_test matrix_test)
endif()
