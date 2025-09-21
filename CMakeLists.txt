cmake_minimum_required(VERSION 3.10)
project(OTPQCOM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# === Individual libs ===
add_library(otpqcomlib
        otpqcom/src/NodeNetworkConfig.cpp
        otpqcom/src/NetIO/EMPCommunicator.cpp
        otpqcom/src/NetIO/EMPNetIOChannel.cpp)
target_include_directories(otpqcomlib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
        $<INSTALL_INTERFACE:include>
)

# === Enable testing ===
enable_testing()
add_subdirectory(test)

# === Documentation ===
add_subdirectory(doc)