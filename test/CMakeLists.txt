include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(NodeNetworkConfig_test NodeNetworkConfig_test.cpp)
target_link_libraries(NodeNetworkConfig_test PRIVATE otpqcomlib GTest::gtest_main)


add_executable(EMPNetIOChannel_test EMPNetIOChannel_test.cpp)
target_link_libraries(EMPNetIOChannel_test PRIVATE otpqcomlib GTest::gtest_main)


add_executable(EMPCommunicator_test EMPCommunicator_test.cpp)
target_link_libraries(EMPCommunicator_test PRIVATE otpqcomlib GTest::gtest_main)


include(GoogleTest)
gtest_discover_tests(NodeNetworkConfig_test)
