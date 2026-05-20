#include <asio.hpp>
#include <functional>
#include <iostream>
#include <map>
#include <print>
#include <string>
#include <vector>

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

// Command-line interface entry method.
bool commandLineInterface(asio::io_context& context);

// Command-line command callbacks.
bool help(CommandParameters);
bool quit(CommandParameters);
