cmake_minimum_required(VERSION 3.28)
project(OTPQCOM LANGUAGES CXX)

# === C++ Standard ===
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_23)

# === Options ===
set(NETWORK_BUFFER_SIZE "1048576" CACHE STRING "Default buffer size in bytes for network I/O")
set(NETWORK_BUFFER_MODE "FullyBuffered" CACHE STRING "Default buffer mode: FullyBuffered, LineBuffered, Unbuffered")
set_property(CACHE NETWORK_BUFFER_MODE PROPERTY STRINGS FullyBuffered LineBuffered Unbuffered)

# Validate buffer size is numeric
if (NOT NETWORK_BUFFER_SIZE MATCHES "^[0-9]+$")
    message(FATAL_ERROR "NETWORK_BUFFER_SIZE must be a positive integer, got '${NETWORK_BUFFER_SIZE}'")
endif()

# === Print configuration ===
message(STATUS "====================================================")
message(STATUS "Configuring OTPQCOM")
message(STATUS "  C++ standard:           23")
message(STATUS "  Network buffer size:    ${NETWORK_BUFFER_SIZE} bytes")
message(STATUS "  Network buffer mode:    ${NETWORK_BUFFER_MODE}")
message(STATUS "====================================================")

# === Library ===
add_library(libotpqcom
        otpqcom/src/NodeNetworkConfig.cpp
        otpqcom/src/NetIO/SocketChannel.cpp
        otpqcom/src/NetIO/ClientSocketChannel.cpp
        otpqcom/src/NetIO/ServerSocketChannel.cpp
        otpqcom/src/NetIO/MPCommunication.cpp
        otpqcom/src/NetIO/SocketMPCommunication.cpp
)

target_include_directories(libotpqcom
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(libotpqcom PUBLIC project_options)

target_compile_definitions(libotpqcom
        PUBLIC
        NETWORK_BUFFER_SIZE=${NETWORK_BUFFER_SIZE}
        NETWORK_BUFFER_MODE=${NETWORK_BUFFER_MODE}
)

# === Enable testing ===
enable_testing()
add_subdirectory(test)

# === Documentation ===
add_subdirectory(doc)
