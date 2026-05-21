#include "../neptah_shared.h"
#include <functional>

struct CommandParameters {
    std::vector<std::string> arguments;
    asio::io_context& context;
};

using CommandCallback = std::function<bool(CommandParameters)>;
struct CommandInfo {
    CommandCallback command;
    std::string description;
};

extern int GLOBAL_ARGC;
extern char** GLOBAL_ARGV;
extern std::map<std::string, CommandInfo> COMMANDS;

struct NetworkInterface {
    unsigned short port;

    NetworkInterface(asio::io_context& context, unsigned short port):
    port(port), _context(context), _acceptor(context, tcp::endpoint(tcp::v4(), port)) {}

    void listen();
private:
    asio::ip::tcp::acceptor _acceptor;
    asio::io_context& _context;
};

// Command-line interface entry method.
bool commandLineInterface(asio::io_context& context);

// Command-line command callbacks.
bool help(CommandParameters);
bool quit(CommandParameters);
