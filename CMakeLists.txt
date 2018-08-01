cmake_minimum_required(VERSION 3.2)

if (NOT DEFINED $ENV{TRAVIS_BUILD_NUMBER})
    set(BUILD_NUMBER 1)
else()
    set(BUILD_NUMBER $ENV{TRAVIS_BUILD_NUMBER})
endif()

project(allocator VERSION 0.0.${BUILD_NUMBER})

add_executable(allocator main.cpp)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test_allocator test.cpp)

set_target_properties(allocator test_allocator PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

target_link_libraries(test_allocator
    pthread
    gtest
)

install(TARGETS allocator RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT illarionovanton@gmail.com)

include(CPack)

enable_testing()

add_test(GTEST_ON_ALLOCATOR test_allocator)
