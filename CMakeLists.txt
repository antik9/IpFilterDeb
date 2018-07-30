cmake_minimum_required(VERSION 3.2)

set(BUILD_NUMBER 1)

project(allocator VERSION 0.0.${BUILD_NUMBER})

add_executable(allocator main.cpp)

#add_library(clist list.hpp)

#add_library(callocator allocator.hpp)

set_target_properties(allocator PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

#target_link_libraries(allocator
#    clist
#    callocator
#)

install(TARGETS allocator RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT illarionovanton@gmail.com)

include(CPack)

