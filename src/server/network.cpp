#include "shared.h"

void NetworkInterface::listen() {
    auto socket = std::make_shared<tcp::socket>(_context);

    _acceptor.async_accept([this](const asio::error_code& error, tcp::socket socket) {
        if (!error) {
            asio::error_code endpoint_error;
            auto endpoint = socket.remote_endpoint(endpoint_error);

            if (!endpoint_error) {
                auto connection = std::make_shared<Connection>(std::move(socket), "", manager);
                manager.add(connection);

                connection->read();
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
                logger->error("Connection was aborted before it could be accepted.");
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

            return;
        }

        this->listen();
    });
}

void Connection::read() {
    // Ensure the connection to stay alive whilst waiting for network.
    auto self(shared_from_this());

    socket.async_read_some(
        asio::buffer(_buffer),
        [this, self](asio::error_code error, std::size_t transferred) {
            if (!error) {
                this->read();
            }
            else {
                if (error == asio::error::eof) {}
                else if (error == asio::error::connection_reset) {}

                _manager.remove(self);
            }
        });
}

void ConnectionManager::add(std::shared_ptr<Connection> connection) {
    if (connection->username.empty()) {
        // By default users will have an anonymous username.
        u_int64_t anon_index = ++_user_id;
        std::string username = std::format("Anonymous {}", anon_index);

        connection->username = username;
    }

    asio::error_code error;
    tcp::endpoint endpoint = connection->socket.remote_endpoint(error);

    if (!error) {
        _connections.insert(connection);

        logger->info(
            std::format(
                "Connected user {} from {}:{}",
                connection->username,
                endpoint.address().to_string(),
                endpoint.port()
            )
        );
    }
    else {
        logger->error(
            std::format("Terminating connection, did not receive IP address from: {}",
                connection->username
            )
        );

        connection->socket.close();
    }
}

void ConnectionManager::remove(std::shared_ptr<Connection> connection) {
    _connections.erase(connection);
    connection->socket.close();

    logger->info(std::format("Disconnected user {}", connection->username));
}
