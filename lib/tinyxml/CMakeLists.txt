file(GLOB sources *.cpp)

add_library(tinyxml STATIC ${sources})
target_include_directories(tinyxml PUBLIC ${CMAKE_SOURCE_DIR})
