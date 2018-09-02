cmake_minimum_required(VERSION 3.2)

find_package(Boost_REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(GTest)

if (GTest_FOUND)
	add_executable(concepts_test tests.cpp)
    
	set_target_properties(concepts_test PROPERTIES
   	 	CXX_STANDARD 14
    	CXX_STANDARD_REQUIRED ON
    	COMPILE_OPTIONS -Wpedantic -Wall -Wextra
	)

	target_link_libraries(concepts_test
    	pthread
        ${Boost_LIBRARIES}
    	${GTEST_BOTH_LIBRARIES}
	)

	enable_testing()

	add_test(concepts_test)
endif()
