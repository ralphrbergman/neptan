#include "shared.h"

void NetworkInterface::listen() {
    auto socket = std::make_shared<tcp::socket>(_context);

    _acceptor.async_accept(*socket, [this, socket](const asio::error_code& error) {
        if (!error) {
            asio::error_code endpoint_error;
            auto endpoint = socket->remote_endpoint(endpoint_error);

            if (!endpoint_error) {
                logger->info(std::format("New connection: {}\n", socket->remote_endpoint()));

                // TODO: Save socket to a map of IP address to socket.
            }
            else {
                logger->error(
                    std::format("Client disconnected after accept: {}",
                    endpoint_error.message()
                ));
            }
        }
        else {
            if (error == asio::error::connection_aborted) {
                logger->error("");
            }
            else if (error == asio::error::no_descriptors) {
                logger->critical(
                    "Server has run out of file descriptors "
                    "for new sockets, closing the connection."
                );
            }
            else if (error == asio::error::network_unreachable) {
                logger->critical("Server doesn't have connection to a network.");
            }
        }

        this->listen();
    });
}
