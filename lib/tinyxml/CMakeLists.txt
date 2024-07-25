# set source files
set(SOURCES
    tinystr.cpp
    tinyxml.cpp
    tinyxmlerror.cpp
    tinyxmlparser.cpp
)

add_library(tinyxml STATIC ${SOURCES})
target_include_directories(tinyxml PUBLIC ${CMAKE_SOURCE_DIR})
