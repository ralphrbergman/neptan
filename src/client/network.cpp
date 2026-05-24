#include "shared.h"

// This number will be used to implement resolve/connection retry mechanism
// so the program feels natural and forgiving if the first time didn't work.
int default_retry_count = 5;
int retry_count = 0;

NetworkInterface::NetworkInterface(
    asio::io_context& context,
    std::string address,
    std::string port
):
address(address),
port(port),
_context(context),
_resolver(context),
_socket(context),
_timer(context) {}

void NetworkInterface::resolve_and_connect() {
    if (retry_count == default_retry_count) {
        retry_count = 0;

        logger->error(std::format(
            "Could not reach address {}:{}, please try again later.",
            address,
            port
        ));
        return;
    }

    // Print this only initially so the console isn't cluttered up.
    if (retry_count == 0) {
        logger->info(std::format("Resolving address {}:{}...", address, port));
    }

    _resolver.async_resolve(
        address,
        port,
        [this](const asio::error_code& error, tcp::resolver::results_type endpoints) {
            if (!error) {
                logger->info("Successfully resolved address.");

                this->_endpoints = endpoints;
                retry_count = 0;
                this->connect();
            }
            else {
                logger->error(
                    std::format("Failed to resolve name, retrying... ({})", retry_count + 1)
                );

                _timer.expires_after(std::chrono::seconds(2));
                _timer.async_wait([this] (const asio::error_code& error) {
                    if (!error) {
                        retry_count++;
                        this->resolve_and_connect();
                    }
                    else if (error != asio::error::operation_aborted) {
                        logger->error("Unexpected error happened at timer execution.");
                    }
                });
            }
        });
}

void NetworkInterface::connect() {
    if (retry_count == default_retry_count) {
        retry_count = 0;

        logger->error(std::format(
            "Could not connect to server on {}:{}",
            address,
            port
        ));
        return;
    }

    _timer.expires_after(std::chrono::seconds(15));
    _timer.async_wait([this] (const asio::error_code& error) {
        if (!error) {
            logger->warning("Connection timed out.");

            retry_count++;
            _socket.close();
            this->connect();
        }
        else if (error != asio::error::operation_aborted) {
            logger->error(std::format(
                "Error happened at timer execution: ",
                error.message()
            ));
        }
    });

    asio::async_connect(
        this->_socket,
        _endpoints,
        [this](const asio::error_code& error, const tcp::endpoint& endpoint) {
            // Network is healthy, so cancel 15 second timer.
            _timer.cancel();

            if (!error) {
                logger->info("Successfully established connection.");

                this->_read_header();
            }
            else if (error != asio::error::operation_aborted) {
                logger->error(std::format(
                    "Connection failed: {} ({})",
                    error.message(),
                    retry_count + 1
                ));

                _socket.close();

                _timer.expires_after(std::chrono::seconds(2));
                _timer.async_wait([this] (const asio::error_code& error) {
                    if (!error) {
                        retry_count++;
                        this->connect();
                    }
                    else if (error != asio::error::operation_aborted) {
                        logger->error(std::format(
                            "Error happened at timer execution: ",
                            error.message()
                        ));
                    }
                });
            }
        });
}

void NetworkInterface::disconnect() {
    // TODO: Create a packet to notify the server I am leaving the chatroom.
    asio::error_code error;
    _socket.shutdown(tcp::socket::shutdown_both, error);

    _socket.close();
}

void NetworkInterface::receive() {}

template <typename T>
void NetworkInterface::send_json(const T& parseable) {
    neptah::send_json(this->_socket, parseable);
}

void NetworkInterface::send(const std::string& message) {
    neptah::send(this->_socket, message);
}

void NetworkInterface::_read_body() {
    _buffer.resize(_body_length);

    asio::async_read(
        _socket,
        asio::buffer(_buffer),
        [this] (const asio::error_code& error, size_t transferred) {
            if (!error) {
                std::string json_payload(_buffer.begin(), _buffer.begin() + transferred);

                // Process next receive event.
                this->_read_header();
            }
            else _socket.close();
        });
}

void NetworkInterface::_read_header() {
    asio::async_read(
        _socket,
        asio::buffer(&_body_length, sizeof(_body_length)),
        [this] (const asio::error_code& error, size_t transferred) {
            if (!error) {
                this->_read_body();
            }
            else _socket.close();
        });
}
