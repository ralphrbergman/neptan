#pragma once

#include "../neptah_shared.h"

struct NetworkInterface {
    std::string address;
    std::string port;

    NetworkInterface(asio::io_context& context, std::string address, std::string port);

    void resolve_and_connect();
    void connect();
    void disconnect();

private:
    asio::io_context& _context;
    tcp::resolver _resolver;
    tcp::socket _socket;
    asio::steady_timer _timer;
    tcp::resolver::results_type _endpoints;
};
