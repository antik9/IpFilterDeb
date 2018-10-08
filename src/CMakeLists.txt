cmake_minimum_required(VERSION 3.2)

if($ENV{TRAVIS_BUILD_NUMBER})
    set (BUILD_NUMBER $ENV{TRAVIS_BUILD_NUMBER})
else ()
    set (BUILD_NUMBER 1)
endif()

enable_language(CXX)

project(join_server VERSION 0.0.${BUILD_NUMBER})

set(CMAKE_CXX_STANDARD 14)

add_executable(join_server main.cpp)

add_library(join join.cpp join.h)

add_library(server server.cpp server.h)

set_target_properties(join_server join server PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

target_link_libraries(server
    join
)

target_link_libraries(join_server
    join
    server
    Boost::thread
)

install(
    TARGETS join_server
    RUNTIME DESTINATION bin
)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT illarionovanton@gmail.com)

include(CPack)
