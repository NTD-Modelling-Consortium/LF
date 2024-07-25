
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
add_executable(transfil_N ${SOURCES})

find_package(GSL REQUIRED)
# Include the library directories for linking
target_include_directories(transfil_N PRIVATE
                          ${CMAKE_CURRENT_SOURCE_DIR}
                          ${PROJECT_SOURCE_DIR}/lib/tinyxml
                          )
target_link_libraries(transfil_N GSL::gsl GSL::gslcblas tinyxml)
