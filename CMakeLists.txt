cmake_minimum_required(VERSION 3.28)
project(OTPQCOM LANGUAGES CXX)

# ================================================
#  Project-wide compile settings (INTERFACE target)
# ================================================
add_library(otpqcom_project_options INTERFACE)
target_compile_features(otpqcom_project_options INTERFACE cxx_std_23)

# ================================================
#  User-configurable build options
# ================================================
set(NETWORK_BUFFER_SIZE "1048576" CACHE STRING "Default buffer size for network I/O (bytes)")
set(NETWORK_BUFFER_MODE "FullyBuffered" CACHE STRING "Default buffer mode: FullyBuffered, LineBuffered, Unbuffered")
set_property(CACHE NETWORK_BUFFER_MODE PROPERTY STRINGS FullyBuffered LineBuffered Unbuffered)

if (NOT NETWORK_BUFFER_SIZE MATCHES "^[0-9]+$")
    message(FATAL_ERROR "NETWORK_BUFFER_SIZE must be numeric, got '${NETWORK_BUFFER_SIZE}'")
endif()

message(STATUS "====================================================")
message(STATUS "Configuring OTPQCOM")
message(STATUS "  C++ standard:           23")
message(STATUS "  Network buffer size:    ${NETWORK_BUFFER_SIZE} bytes")
message(STATUS "  Network buffer mode:    ${NETWORK_BUFFER_MODE}")
message(STATUS "====================================================")

# ================================================
#  OTPQCOM Library
# ================================================
add_library(libotpqcom
        src/lib/NodeNetworkConfig.cpp
        src/lib/Sockets/SocketChannel.cpp
        src/lib/Sockets/ClientSocketChannel.cpp
        src/lib/Sockets/ServerSocketChannel.cpp
        src/lib/Sockets/SocketMPChannel.cpp
        src/lib/NetHelpers.cpp
)

# Public includes = installed headers
target_include_directories(libotpqcom
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/lib    # private headers (if any)
)

target_link_libraries(libotpqcom
        PUBLIC
        otpqcom_project_options
)

target_compile_definitions(libotpqcom
        PUBLIC
        NETWORK_BUFFER_SIZE=${NETWORK_BUFFER_SIZE}
        NETWORK_BUFFER_MODE=${NETWORK_BUFFER_MODE}
)

# ================================================
#  Testing + Examples
# ================================================
enable_testing()
add_subdirectory(test)

add_subdirectory(doc)
add_subdirectory(examples)
