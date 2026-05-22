#pragma once

#include "../neptah_shared.h"
#include <functional>
#include <unordered_set>

struct ConnectionManager;

struct CommandParameters {
    std::vector<std::string> arguments;
    asio::io_context& context;
    ConnectionManager& manager;
};

using CommandCallback = std::function<bool(CommandParameters)>;
struct CommandInfo {
    CommandCallback command;
    std::string description;
};

extern std::map<std::string, CommandInfo> COMMANDS;

// Forward declare ConnectionManager to satisfy Connection struct.
struct ConnectionManager;

struct Connection : public std::enable_shared_from_this<Connection> {
    tcp::socket socket;
    std::string username;

    Connection(tcp::socket incoming_socket, std::string username, ConnectionManager& manager):
    socket(std::move(incoming_socket)), username(username), _manager(manager) {}

    void read();
    void send(const std::string json_payload);

private:
    std::array<char, 1024> _buffer;
    ConnectionManager& _manager;
};

struct ConnectionManager {
    void add(std::shared_ptr<Connection> connection);
    void remove(std::shared_ptr<Connection> connection);
    void broadcast(std::shared_ptr<Connection> connection, const std::string json_payload);
    void broadcast_all(const std::string json_payload);

private:
    std::unordered_set<std::shared_ptr<Connection>> _connections;
    uint64_t _user_id = 0;
};

struct NetworkInterface {
    unsigned short port;
    ConnectionManager& manager;

    NetworkInterface(asio::io_context& context, unsigned short port, ConnectionManager& manager):
    port(port),
    _context(context),
    _acceptor(context, tcp::endpoint(tcp::v4(), port)),
    manager(manager) {}

    void listen();
private:
    asio::ip::tcp::acceptor _acceptor;
    asio::io_context& _context;
};

// Command-line interface entry method.
bool commandLineInterface(asio::io_context& context, ConnectionManager& manager);

// Command-line command callbacks.
namespace Commands {
    bool help(CommandParameters);
    bool send(CommandParameters);
    bool quit(CommandParameters);
}
