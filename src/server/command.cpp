#include "../server_json.h"
#include "shared.h"
#include <sstream>

std::map<std::string, CommandInfo> COMMANDS = {
    { "clear", { Commands::clear, "Clears output." } },
    { "help", { Commands::help, "Displays this menu." } },
    { "send", { Commands::send, "Sends a global message." } },
    { "quit", { Commands::quit, "Disconnects everyone and quits." } }
};

void commandLineInterface(asio::io_context& context, ConnectionManager& manager) {
    auto input = std::make_shared<asio::posix::stream_descriptor>(context, ::dup(STDIN_FILENO));
    auto buffer = std::make_shared<asio::streambuf>();

    auto _read = std::make_shared<std::function<void(asio::error_code, size_t)>>();

    *_read = [&context, &manager, input, buffer, _read] (asio::error_code error, size_t length) {
        if (!error) {
            std::string raw;
            std::istream is(buffer.get());
            std::getline(is, raw);

            std::stringstream ss(raw);
            std::string command;
            std::vector<std::string> arguments;

            if (!(ss >> command)) {
                std::cout << "$ " << std::flush;
                asio::async_read_until(*input, *buffer, '\n', *_read);
                return;
            }

            // Add unexpected amount of arguments into a vector.
            std::string argument;
            while (ss >> argument) {
                arguments.push_back(argument);
            }

            // Dynamically find the command callback and execute it.
            auto callback = COMMANDS.find(command);
            // Commands will return a boolean whether to continue reading or not.
            bool keepOn = true;

            if (callback != COMMANDS.end()) {
                keepOn = callback->second.command(CommandParameters{
                    arguments,
                    context,
                    manager
                });
            }
            else std::printf("Command: %s doesn't exist.\n", command.c_str());

            if (!keepOn) return;

            std::cout << "$ " << std::flush;
            asio::async_read_until(*input, *buffer, '\n', *_read);
        }
    };

    std::cout << "$ " << std::flush;
    asio::async_read_until(*input, *buffer, '\n', *_read);
}

bool Commands::clear(CommandParameters params) {
    std::cout << "\033[2J\033[1;H" << std::flush;

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
