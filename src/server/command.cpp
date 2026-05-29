#include "../server_json.h"
#include "shared.h"
#include <sstream>

std::map<std::string, CommandInfo> COMMANDS = {
    { "help", { Commands::help, "Displays this menu." } },
    { "send", { Commands::send, "Sends a global message." } },
    { "quit", { Commands::quit, "Disconnects everyone and quits." } }
};

bool commandLineInterface(asio::io_context& context, ConnectionManager& manager) {
    while (true) {
        std::string raw;

        std::printf("$ ");
        std::fflush(stdout);
        if (!getline(std::cin, raw)) {
            break;
        }

        std::stringstream ss(raw);

        std::string command;
        std::vector<std::string> arguments;

        if (!(ss >> command)) continue;

        // Add unexpected amount of arguments into a vector.
        std::string argument;
        while (ss >> argument) {
            arguments.push_back(argument);
        }

        // Dynamically find the command callback and execute it.
        auto callback = COMMANDS.find(command);

        if (callback != COMMANDS.end()) {
            bool keepOn = callback->second.command(CommandParameters{arguments, context, manager});

            if (!keepOn) return false;
        }
        else std::printf("Command: %s doesn't exist.\n", command.c_str());
    }

    return true;
}

bool Commands::help(CommandParameters params) {
    std::printf("Usage: %s [port]\n", GLOBAL_ARGV[0]);

    std::printf("Arguments:\n");
    std::printf("  %-10s %s\n", "[port]", "TCP port to listen on for incoming clients.");

    std::printf("Commands:\n");
    for (const auto& command : COMMANDS) {
        std::printf("  %-10s %s\n", command.first.c_str(), command.second.description.c_str());
    }

    return true;
}

bool Commands::send(CommandParameters params) {
    // Join vector of strings into one string so it can be transmitted over the net.
    std::string content = join_vector(params.arguments);
    params.manager.broadcast_all(content);

    return true;
}

bool Commands::quit(CommandParameters params) {
    std::printf("Shutdown initiated by administrator.\n");

    params.context.stop();

    return false;
}
