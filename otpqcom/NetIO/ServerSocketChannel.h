#pragma once
#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetIO/SocketChannel.h>

namespace otpq::network::sockets {

    /**
     * @class ServerSocketChannel
     * @brief A concrete implementation of the SocketChannel class for server-side socket communication.
     *
     * This class extends the `SocketChannel` base class to implement server-side socket communication.
     * It provides specific implementations for the `sendData` and `recvData` methods, as well as additional
     * functionality to listen for incoming connections and manage the server socket.
     *
     * The server socket listens for client connections, accepts those connections, and handles communication
     * with the client through the `sendData` and `recvData` methods.
     */
    class ServerSocketChannel final : public SocketChannel {
    public:
        /**
         * @brief Constructs a ServerSocketChannel with the specified network configuration.
         *
         * This constructor initializes the `ServerSocketChannel` with the given network configuration,
         * and sets up the server socket.
         *
         * @param cfg The network configuration (e.g., address, port) for this server socket.
         */
        explicit ServerSocketChannel(NodeNetworkConfig cfg);

        /**
         * @brief Destructor that performs necessary cleanup.
         *
         * Ensures proper cleanup of resources when the `ServerSocketChannel` object is destroyed.
         * This includes closing the listening socket and any client-specific sockets.
         */
        ~ServerSocketChannel();

        /**
         * @brief Sends data over the server socket.
         *
         * This method is used to send data over the socket. It overrides the pure virtual `sendData`
         * method from `SocketChannel`.
         *
         * @param data A pointer to the data that should be sent.
         * @param len The length of the data to send.
         * @throws std::runtime_error if the sending operation fails.
         */
        void sendData(const void *data, std::size_t len) override;

        /**
         * @brief Receives data from the server socket.
         *
         * This method is used to receive data over the socket. It overrides the pure virtual `recvData`
         * method from `SocketChannel`.
         *
         * @param data A pointer to the buffer where the received data will be stored.
         * @param len The size of the buffer to receive data into.
         * @throws std::runtime_error if the receiving operation fails.
         */
        void recvData(void *data, std::size_t len) override;

        void streamFlush() const;

        /**
         * @brief Begins listening for incoming client connections on the server socket.
         *
         * This method sets up the server socket to listen for incoming connections, enabling the server to
         * accept client connections. It is called before any connection can be accepted.
         *
         * @throws std::runtime_error if the listening operation fails.
         */
        void awaitConnection() const;

        /**
         * @brief Accepts an incoming client connection.
         *
         * This method accepts an incoming connection once the server has begun listening. It creates a new
         * socket dedicated to communication with the client.
         *
         * @throws std::runtime_error if no connection request is pending or the accept operation fails.
         */
        void acceptConnection();

        /**
         * @brief Listens for and serves client connections.
         *
         * This method combines the functionality of listening for connections and accepting them in a loop.
         * After accepting a connection, the server can begin communication with the client using the
         * `sendData` and `recvData` methods.
         *
         * @throws std::runtime_error if the listening or accepting operations fail.
         */
        void awaitAndServe();

    private:
        /**
         * @brief Initializes the server socket.
         *
         * Sets up the necessary configurations for the server socket, such as socket options and binding
         * to the specified address and port. This is called internally during the constructor to prepare
         * the socket for listening.
         *
         * @throws std::runtime_error if the socket setup fails.
         */
        void setupServer();

        /**
         * @brief A flag indicating if the server has already sent data.
         *
         * This boolean flag is used to track whether the server has already sent data, to manage
         * connection behavior or retries if necessary.
         */
        bool hasSent_{false};

        /**
         * @brief The socket used for listening for incoming client connections.
         *
         * This socket listens on the specified address and port, waiting for client connection requests.
         * It is initialized to -1, indicating that no connection is being listened for yet.
         */
        int listenSocket_{-1};

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
