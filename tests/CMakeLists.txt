find_package(Catch2 3 REQUIRED)
# These tests can use the Catch2-provided main
set(TESTS_TO_RUN test_main.cpp)
add_executable(tests ${TESTS_TO_RUN})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain model)

include(CTest)
include(Catch)
catch_discover_tests(tests)