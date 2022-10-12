cmake_minimum_required(VERSION 3.6)
project(CarModel VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# if(CMAKE_BUILD_TYPE EQUAL Debug)
# add_definitions(-D__DEBUG__)
# endif()

include_directories(./)
include_directories(/)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)

# get_property(dirs DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# message(">>> include_dirs=${dirs}")

# message(STATUS "build type:" ${CMAKE_BUILD_TYPE})
# add_library(CarModel src/model/carbuilder.cpp)

message(STATUS "file:" ${PROJ_SRC})
add_executable(TestMain ${PROJ_SRC})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)