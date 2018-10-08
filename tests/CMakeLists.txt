cmake_minimum_required(VERSION 3.2)

find_package(GTest)

if (GTest_FOUND)
	add_executable(join_test tests.cpp)

	set_target_properties(join_test PROPERTIES
   	 	CXX_STANDARD 14
   	 	CXX_STANDARD_REQUIRED ON
   	 	COMPILE_OPTIONS -Wpedantic -Wall -Wextra
	)

	target_link_libraries(join_test
	    pthread
	    boost_system
	    Boost::thread
	    ${GTEST_BOTH_LIBRARIES}
	    join
	)

	enable_testing()

	add_test(join_test join_test)
endif()
