cmake_minimum_required(VERSION 3.2)

if($ENV{TRAVIS_BUILD_NUMBER})
    set (BUILD_NUMBER $ENV{TRAVIS_BUILD_NUMBER})
else ()
    set (BUILD_NUMBER 1)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(realty VERSION 0.0.${BUILD_NUMBER})

set(CMAKE_CXX_STANDARD 14)

add_library(advisor advisor.cpp)

add_library(read read.cpp)

add_library(unifier unifier.cpp)

add_executable(rclst rclst.cpp)

add_executable(rclss rclss.cpp)

set_target_properties(advisor read rclss rclst unifier PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

target_link_libraries(unifier
    read
)

target_link_libraries(advisor
    dlib
    blas
    unifier
)

target_link_libraries(rclss
    advisor
)

target_link_libraries(rclst
    advisor
)

install(
    TARGETS rclss rclst
    RUNTIME DESTINATION bin
)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT illarionovanton@gmail.com)

include(CPack)
