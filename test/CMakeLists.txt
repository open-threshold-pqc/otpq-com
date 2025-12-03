include(FetchContent)

# Fetch GoogleTest
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# You MUST include this before using gtest_discover_tests()
include(GoogleTest)

# ==============================================
# Automatically create test executables
# ==============================================

# Pick up all *_test.cpp files in this directory
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "*_test.cpp")

foreach(test_src ${TEST_SOURCES})
    # Extract filename without extension
    get_filename_component(test_name ${test_src} NAME_WE)

    # Create the test executable
    add_executable(${test_name} ${test_src})

    # Link the project library and GTest
    target_link_libraries(${test_name}
            PRIVATE
            libotpqcom
            GTest::gtest_main
    )

    message(STATUS "Configured test: ${test_name}")

    # Register the test with CTest
    gtest_discover_tests(${test_name})
endforeach()
