cmake_minimum_required(VERSION 3.2)

if($ENV{TRAVIS_BUILD_NUMBER})
    set (BUILD_NUMBER $ENV{TRAVIS_BUILD_NUMBER})
else ()
    set (BUILD_NUMBER 1)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(async)

set(CMAKE_CXX_STANDARD 14)

add_library(bulk SHARED bulk.cpp bulk.hpp)

add_library(async SHARED async.cpp async.h)

set_target_properties(async bulk PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

target_link_libraries(bulk
    pthread
)

target_link_libraries(async
    pthread
    bulk
)

install(
    TARGETS async
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT illarionovanton@gmail.com)

include(CPack)
