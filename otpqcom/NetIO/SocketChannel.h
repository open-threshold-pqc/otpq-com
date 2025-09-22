#pragma once

#include <cstring>
#include <memory>
#include <otpqcom/NodeNetworkConfig.h>

namespace otpq::network::sockets {

    /**
     * @brief Enum class that defines various socket options.
     */
    enum class SocketOptions {
        REUSEADDR, ///< Option to allow reuse of local addresses.
        SETDELAY,  ///< Option to enable delay for TCP connections.
        SETNODELAY ///< Option to disable delay for TCP connections (Nagle's algorithm).
    };

    /**
     * @class SocketChannel
     * @brief A base class representing a socket channel with virtual methods for sending, receiving data,
     *        and setting socket options.
     *
     * This class provides the necessary structure for handling socket communications.
     * Derived classes are expected to implement the `sendData` and `recvData` methods to perform the actual
     * sending and receiving operations.
     */
    class SocketChannel {
    public:
        /**
         * @brief Constructs a SocketChannel with the given network configuration.
         *
         * @param cfg The configuration object containing network settings for the socket.
         */
        explicit SocketChannel(NodeNetworkConfig cfg);

        /**
         * @brief Destructor that performs necessary cleanup. Ensures the socket is closed if it was opened.
         */
        virtual ~SocketChannel();

        /**
         * @brief Sends data over the socket.
         *
         * This is a pure virtual method. Derived classes must provide their implementation for sending data.
         *
         * @param data A pointer to the data to send.
         * @param len The length of the data to send.
         * @throws std::runtime_error if not implemented by the derived class.
         */
        virtual void sendData(const void *data, std::size_t len) = 0;

        /**
         * @brief Receives data from the socket.
         *
         * This is a pure virtual method. Derived classes must provide their implementation for receiving data.
         *
         * @param data A pointer to the buffer where received data will be stored.
         * @param len The length of the buffer.
         * @throws std::runtime_error if not implemented by the derived class.
         */
        virtual void recvData(void *data, std::size_t len) = 0;

    protected:
        /**
         * @brief Sets socket options, such as address reuse and TCP_NODELAY.
         *
         * This method allows derived classes to configure socket-level options for the connection.
         *
         * @param connSocket
         * @param opt The socket option to set (e.g., REUSEADDR, SETDELAY, SETNODELAY).
         * @throws std::invalid_argument if an unknown option is passed.
         * @throws std::runtime_error if setting the socket option fails.
         */
        static void setOption(int connSocket, SocketOptions opt);

        /**
         * @brief The network configuration for this socket.
         *
         * Holds the settings for the socket connection, such as the IP address and port.
         */
        NodeNetworkConfig netcfg_;

        /**
         * @brief A buffer used for sending and receiving data.
         *
         * Allocates a buffer to hold data being sent or received via the socket.
         */
        std::unique_ptr<uint8_t[]> buffer_;

    };
} // namespace otpq::network::sockets
