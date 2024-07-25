file(GLOB sources *.cpp)
add_executable(transfil_N ${sources})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(GSL REQUIRED)
# Include the library directories for linking
target_include_directories(transfil_N PRIVATE
                        ${PROJECT_BINARY_DIR}
                          ${PROJECT_SOURCE_DIR}/lib/tinyxml
                          )
target_link_libraries(transfil_N GSL::gsl GSL::gslcblas tinyxml)
