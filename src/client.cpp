#include "client/shared.h"

int GLOBAL_ARGC = 0;
char** GLOBAL_ARGV = nullptr;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << std::format("Usage: {} <ip-address> <port>", argv[0]) << std::endl;
        return 1;
    }
    else {
        GLOBAL_ARGC = argc;
        GLOBAL_ARGV = argv;
    }

    static Logger client_logger("client.log");
    client_logger.open();

    logger = &client_logger;

    asio::io_context context;

    auto work_guard = asio::make_work_guard(context);

    NetworkInterface interface(context, argv[1], argv[2]);
    interface.resolve_and_connect();

    context.run();

    client_logger.close();
    return 0;
}
