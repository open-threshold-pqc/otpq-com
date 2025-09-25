include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(TestNodeNetworkConfig_test NodeNetworkConfig_test.cpp)
target_link_libraries(TestNodeNetworkConfig_test PRIVATE libotpqcom GTest::gtest_main)


add_executable(TestSocketChannel SocketChannel_test.cpp)
target_link_libraries(TestSocketChannel PRIVATE libotpqcom GTest::gtest_main)


add_executable(TestSocketMPChannel SocketMPChannel_test.cpp)
target_link_libraries(TestSocketMPChannel PRIVATE libotpqcom GTest::gtest_main)


include(GoogleTest)
gtest_discover_tests(TestNodeNetworkConfig_test)
