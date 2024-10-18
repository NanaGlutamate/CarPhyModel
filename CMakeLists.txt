cmake_minimum_required(VERSION 3.6)
project(CarPhyModel VERSION 0.1.0)
set(PROJ_NAME CarPhyModel)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# if(CMAKE_BUILD_TYPE EQUAL Debug)
# add_definitions(-D__DEBUG__)
# endif()

include_directories(/)
include_directories(src/extern/)
include_directories(src/extern/SP++3.0/)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
# set(CMAKE_RELEASE_POSTFIX _release)
# set(CMAKE_DEBUG_POSTFIX _debug)
# set(CMAKE_RELWITHDEBINFO_POSTFIX _relwithdebinfo)
# set(CMAKE_MINSIZEREL_POSTFIX _minsizerel)

# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
# set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

if(MSVC)
  string(APPEND CMAKE_CXX_FLAGS " /permissive- /Zc:__cplusplus /utf-8 ")
endif()

#πÿ±’”≈ªØ
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi -Od -Ob0")

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)

# message(STATUS "standard:" ${CMAKE_CXX_STANDARD})