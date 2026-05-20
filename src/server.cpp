#include "server/shared.h"
#include <thread>

int GLOBAL_ARGC = 0;
char** GLOBAL_ARGV = nullptr;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }
    else {
        GLOBAL_ARGC = argc;
        GLOBAL_ARGV = argv;
    }

    asio::io_context context;

    // Tell ASIO to wait for new tasks to pop up and not exit automatically.
    auto work_guard = asio::make_work_guard(context);

    // Create a new thread to handle command-line input from the admin.
    std::thread cliThread(commandLineInterface, std::ref(context));

    context.run();

    // Clean up command-line interface thread.
    if (cliThread.joinable()) cliThread.join();

    return 0;
}
