cmake_minimum_required(VERSION 3.9)

# set the project name and version
project(LF VERSION 1.0)

option(BUILD_TESTS "Build the tests for the project" ON)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add libraries
add_subdirectory(lib/tinyxml)

# add source code
add_subdirectory(src)

# add tests
if(BUILD_TESTS)
    add_subdirectory(tests)
endif(BUILD_TESTS)
