cmake_minimum_required(VERSION 2.8)

set (TARGET jasl)

project (${TARGET})

#add_definitions(-DDEBUG)

include_directories ("${PROJECT_SOURCE_DIR}")
set (CMAKE_INCLUDE_PATH "/usr/local/include")
set (CMAKE_LIBRARY_PATH "/usr/local/lib")

LINK_DIRECTORIES(${PROJECT_BINARY_DIR})

set (CMAKE_CXX_COMPLILER "g++")
set (CMAKE_CXX_FLAGS "-std=c++0x -ggdb3 -Wall")

file (GLOB_RECURSE SOURCE_LIST ${PROJECT_SOURCE_DIR}/*.cpp)

add_library (${TARGET} SHARED ${SOURCE_LIST})
