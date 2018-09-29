cmake_minimum_required(VERSION 3.2)

if($ENV{TRAVIS_BUILD_NUMBER})
    set (BUILD_NUMBER $ENV{TRAVIS_BUILD_NUMBER})
else ()
    set (BUILD_NUMBER 1)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(bulk_server VERSION 0.0.${BUILD_NUMBER})

set(CMAKE_CXX_STANDARD 14)

add_executable(bulk_server main.cpp)

add_library(bulk bulk.cpp bulk.hpp)

add_library(async async.cpp async.h)

add_library(server server.cpp server.h)

set_target_properties(async bulk server PROPERTIES
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

target_link_libraries(server
    pthread
    boost_system
    bulk
    async
)

target_link_libraries(bulk_server
    pthread
    boost_system
    bulk
    async
    server
)

install(
    TARGETS bulk_server
    RUNTIME DESTINATION bin
)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT illarionovanton@gmail.com)

include(CPack)
