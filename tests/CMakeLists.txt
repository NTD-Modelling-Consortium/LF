find_package(Catch2 3 REQUIRED)
#These tests can use the Catch2 - provided main
    set(TESTS_TO_RUN test_main.cpp test_host.cpp test_model.cpp)
        list(SORT TESTS_TO_RUN) file(GLOB ALL_FILES RELATIVE $ {
          CMAKE_CURRENT_SOURCE_DIR
        } "*.cpp") list(SORT ALL_FILES)

#Check globbing doesn't find any other tests, whilst still ensuring that generated Make files rebuild appropriately. 
            if (NOT("${ALL_FILES}" STREQUAL "${TESTS_TO_RUN}"))
                message(INFO " Expected tests = ${TESTS_TO_RUN}")
                    message(INFO " Found tests = ${ALL_FILES}")
                        message(SEND_ERROR
                                "Some tests are missing from TESTS_TO_RUN, "
                                "please add in tests/CMakeLists.txt") endif()

                            add_executable(tests ${TESTS_TO_RUN})
                                target_link_libraries(
                                    tests PRIVATE Catch2::Catch2WithMain model)

                                    include(CTest) include(Catch)
                                        catch_discover_tests(tests)
