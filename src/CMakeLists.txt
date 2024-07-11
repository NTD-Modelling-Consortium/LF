file(GLOB_RECURSE sources *.cpp)
add_executable(transfil_N ${sources})

find_package(GSL REQUIRED)
target_link_libraries(transfil_N GSL::gsl GSL::gslcblas)