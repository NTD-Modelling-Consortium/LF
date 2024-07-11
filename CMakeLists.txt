cmake_minimum_required(VERSION 3.9)

# set the project name and version
project(LF VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add subdirectories
add_subdirectory(src)
add_subdirectory(src/tinyxml)