cmake_minimum_required (VERSION 3.7)
project (tree CXX)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_INCLUDE_DIR})
file(GLOB_RECURSE HEADERS "${PROJECT_INCLUDE_DIR}/*.hpp")
MESSAGE(STATUS "headers" ${HEADERS})
source_group(Headers FILES ${HEADERS})

#add_subdirectory(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)