#pragma once

#include "../neptah_shared.h"
#include "../client_json.h"

struct NetworkInterface {
    std::string address;
    std::string port;
    std::string username;

    NetworkInterface(asio::io_context& context, std::string address, std::string port);

    void resolve_and_connect();
    void connect();
    void disconnect();
    void request_username(std::string username);
    template <typename T>
    void send_json(const T& parseable);
    void send(const std::string& message);

private:
    asio::io_context& _context;
    tcp::resolver _resolver;
    tcp::socket _socket;
    asio::steady_timer _timer;
    tcp::resolver::results_type _endpoints;

    uint32_t _body_length = 0;
    std::string _body;
    std::vector<char> _buffer;

    void _read_body();
    void _read_header();
};
