#include "server/shared.h"

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

    unsigned short port = static_cast<unsigned short>(std::stoi(argv[1]));

    static Logger server_logger("server.log");
    server_logger.open();

    logger = &server_logger;

    asio::io_context context;
    ConnectionManager manager{};

    // Tell ASIO to wait for new tasks to pop up and not exit automatically.
    auto work_guard = asio::make_work_guard(context);

    NetworkInterface interface(context, port, manager);
    interface.listen();

    commandLineInterface(context, manager);

    asio::signal_set signals(context, SIGINT, SIGTERM);
    signals.async_wait([&interface] (const asio::error_code& error, int signal_no) {
        interface.terminate();
    });

    context.run();

    server_logger.close();
    return 0;
}
