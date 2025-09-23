include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(NodeNetworkConfig_test NodeNetworkConfig_test.cpp)
target_link_libraries(NodeNetworkConfig_test PRIVATE libotpqcom GTest::gtest_main)

add_executable(SocketChannelServer_test SocketChannelServer_test.cpp)
target_link_libraries(SocketChannelServer_test PRIVATE libotpqcom GTest::gtest_main)

add_executable(SocketChannelClient_test SocketChannelClient_test.cpp)
target_link_libraries(SocketChannelClient_test PRIVATE libotpqcom GTest::gtest_main)

add_executable(SocketMPConnectionU1 SocketMPCommunicationUser1_test.cpp)
target_link_libraries(SocketMPConnectionU1 PRIVATE libotpqcom GTest::gtest_main)
add_executable(SocketMPConnectionU2 SocketMPCommunicationUser2_test.cpp)
target_link_libraries(SocketMPConnectionU2 PRIVATE libotpqcom GTest::gtest_main)
add_executable(SocketMPConnectionU3 SocketMPCommunicationUser3_test.cpp)
target_link_libraries(SocketMPConnectionU3 PRIVATE libotpqcom GTest::gtest_main)


include(GoogleTest)
gtest_discover_tests(NodeNetworkConfig_test)
