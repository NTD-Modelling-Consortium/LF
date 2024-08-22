
# set source files
set(SOURCES
    BedNetEvent.cpp
    Host.cpp
    ImportationRateEvent.cpp
    MDAEvent.cpp
    Model.cpp
    Output.cpp
    Population.cpp
    PrevalenceEvent.cpp
    RecordedPrevalence.cpp
    Scenario.cpp
    ScenariosList.cpp
    Statistics.cpp
    Vector.cpp
    Worm.cpp
    main.cpp
)

find_package(GSL REQUIRED)

# build the executable for running the model, usual transfil_N name
add_executable(transfil_N ${SOURCES})
target_include_directories(transfil_N PRIVATE
                          ${CMAKE_CURRENT_SOURCE_DIR}
                          )
target_link_libraries(transfil_N GSL::gsl GSL::gslcblas tinyxml)

# add as a library for testing, we call the library "model"
add_library(model ${SOURCES})
target_compile_definitions(model PRIVATE DISABLE_MAIN_METHOD)  
target_include_directories(model PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(model GSL::gsl GSL::gslcblas tinyxml)