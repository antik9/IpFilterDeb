cmake_minimum_required(VERSION 3.2)

project(hello_world VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

find_package(GTest REQUIRED)

set (GMock_DIR ".")

configure_file(version.h.in version.h)

add_executable(ip_filter main.cpp)

add_library(version version.cpp)

add_library(ip_filter_src ip_filter.cpp)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test_ip_filter test_ip_filter.cpp)

set_target_properties(ip_filter ip_filter_src test_ip_filter PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

target_link_libraries(ip_filter
    version
    ip_filter_src
)

target_link_libraries(test_ip_filter
    pthread
    gmock
    ${GTEST_BOTH_LIBRARIES}
    ip_filter_src
)

install(TARGETS ip_filter RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT illarionovanton@gmail.com)

include(CPack)

enable_testing()

add_test(test_ip_filter test_ip_filter)

