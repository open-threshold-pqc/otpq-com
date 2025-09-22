#pragma once

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetIO/SocketChannel.h>

namespace otpq::network::sockets {

    /**
     * @class ClientSocketChannel
     * @brief A concrete implementation of the SocketChannel class for client-side socket communication.
     *
     * This class extends the `SocketChannel` base class to implement socket communication for a client.
     * It provides specific implementations for the `sendData` and `recvData` methods, as well as additional
     * functionality to connect to a server and manage the client socket.
     */
    class ClientSocketChannel final : public SocketChannel {
    public:
        /**
         * @brief Constructs a ClientSocketChannel with the specified network configuration.
         *
         * This constructor initializes the `ClientSocketChannel` with the given network configuration,
         * and sets up the client socket.
         *
         * @param cfg The network configuration (e.g., address, port) for this client socket.
         */
        explicit ClientSocketChannel(NodeNetworkConfig cfg);

        /**
         * @brief Destructor that performs necessary cleanup.
         *
         * Ensures proper cleanup of resources when the `ClientSocketChannel` object is destroyed.
         */
        ~ClientSocketChannel();

        /**
         * @brief Sends data over the client socket.
         *
         * This method is used to send data over the socket. It overrides the pure virtual `sendData`
         * method from `SocketChannel`.
         *
         * @param data A pointer to the data that should be sent.
         * @param len The length of the data to send.
         */
        void sendData(const void *data, std::size_t len) override;

        /**
         * @brief Receives data from the client socket.
         *
         * This method is used to receive data over the socket. It overrides the pure virtual `recvData`
         * method from `SocketChannel`.
         *
         * @param data A pointer to the buffer where the received data will be stored.
         * @param len The size of the buffer to receive data into.
         */
        void recvData(void *data, std::size_t len) override;

        void streamFlush() const;

        /**
         * @brief Connects the client socket to a remote node.
         *
         * This method is used to initiate a connection to a remote server or node using the specified
         * network configuration.
         *
         * @param cfg The network configuration (e.g., remote address, port) of the remote node to connect to.
         */
        void nodeConnect(const NodeNetworkConfig &cfg);

    private:
        /**
         * @brief Initializes the client socket.
         *
         * Sets up the necessary configurations for the client socket, such as socket options and connection
         * initialization. This is called internally during the constructor.
         */
        void setupClient();

        /**
         * @brief A flag indicating if the client has already sent data.
         *
         * This boolean flag is used to track whether the client has already sent data, to manage
         * connection behavior or retries if necessary.
         */
        bool hasSent_{false};


        /**
         * @brief A stream pointer for any necessary file operations related to the socket.
         */
        FILE *stream_{};

        /**
         * @brief The socket descriptor.
         *
         * This represents the actual socket that the operating system uses to interact with the network.
         * It is initialized to -1 to indicate that the socket is not yet open.
         */
        int connSocket_{-1};
    };
} // namespace otpq::network::sockets
