cmake_minimum_required(VERSION 3.2)

project(Concepts)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(Concept_creation Concept_creation.cpp)

set_target_properties(Concept_creation PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)
