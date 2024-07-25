file(GLOB sources *.cpp)
add_executable(transfil_N ${sources})

find_package(GSL REQUIRED)
# Include the library directories for linking
target_include_directories(transfil_N PRIVATE
                          ${CMAKE_CURRENT_SOURCE_DIR}
                          ${PROJECT_SOURCE_DIR}/lib/tinyxml
                          )
target_link_libraries(transfil_N GSL::gsl GSL::gslcblas tinyxml)
