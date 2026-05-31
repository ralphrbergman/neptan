#include "shared.h"

void NetworkInterface::listen() {
    auto socket = std::make_shared<tcp::socket>(_context);

    _acceptor.async_accept([this] (const asio::error_code& error, tcp::socket socket) {
        if (!error) {
            asio::error_code endpoint_error;
            auto endpoint = socket.remote_endpoint(endpoint_error);

            if (!endpoint_error) {
                auto connection = std::make_shared<Connection>(std::move(socket), "", manager);
                manager.add(connection);

                connection->read_header();
            }
            else logger->error("Client disconnected before could identify.");
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

void NetworkInterface::terminate() {
    _acceptor.close();
    manager.remove_all();
    _context.stop();
}

void Connection::_read_body() {
    _buffer.resize(_body_length);
    auto self(shared_from_this());

    asio::async_read(
        this->socket,
        asio::buffer(_buffer),
        [this, self] (const asio::error_code& error, size_t transferred) {
            if (!error) {
                std::string json_payload(_buffer.begin(), _buffer.begin() + transferred);
                // TODO: Create a shared function that obtains JSON object
                // and is written only once.
                auto json = nlohmann::json::parse(json_payload);
                auto packet_type = json.at("header").at("type").get<PacketType>();

                switch (packet_type) {
                    case PacketType::Disconnect: {
                        auto packet = json.get<Disconnect>();

                        _manager.remove(shared_from_this());
                    }

                    case PacketType::Message: {
                        auto message = json.get<ServerMessage>();
                        // Ignore malformed messages.
                        if (message.username.size() == 0 || message.content.size() == 0) break;

                        std::cout << std::format(
                            "{}: {}",
                            message.username,
                            message.content
                        ) << std::endl;

                        break;
                    }

                    case PacketType::UsernameRequest: {
                        auto letter = json.get<UsernameRequest>();
                        UsernameAcceptance send_letter(
                            letter.username,
                            letter.username != "Server"
                        );

                        if (send_letter.accepted) this->username = send_letter.username;
                        this->send_json(send_letter);

                        break;
                    }

                    default: break;
                }

                this->read_header();
            }
            else _manager.remove(self);
        });
}

void Connection::read_header() {
    auto self = shared_from_this();

    asio::async_read(
        this->socket,
        asio::buffer(&_body_length, sizeof(_body_length)),
        [this, self] (const asio::error_code& error, size_t transferred) {
            if (!error) {
                this->_read_body();
            }
            else _manager.remove(self);
        });
}

template <typename T>
void Connection::send_json(const T& parseable) {
    neptah::send_json(this->socket, parseable);
}

void Connection::send(const std::string message) {
    neptah::send<ServerMessage>(this->socket, "Server", message);
}

void ConnectionManager::add(std::shared_ptr<Connection> connection) {
    if (connection->username.empty()) {
        // By default users will have an anonymous username.
        u_int64_t anon_index = ++_user_id;
        std::string username = std::format("Anonymous {}", anon_index);

        connection->username = username;
    }

    // Notify user about their username in the chat room.
    connection->send_json(UsernameAcceptance(connection->username, true));

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

        auto welcome_message = std::format("'Sup, {}", connection->username);
        this->broadcast_all(welcome_message);
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

void ConnectionManager::remove_all() {
    // Copy set of connections so we don't mutate whilst iterating.
    auto connections = _connections;

    for (auto connection : connections) {
        this->remove(connection);
    }
}

void ConnectionManager::broadcast(
    std::shared_ptr<Connection> connection,
    const std::string message
) {
    connection->send(message);
}

void ConnectionManager::broadcast_all(const std::string message) {
    for (auto connection : _connections) {
        this->broadcast(connection, message);
    }
}
