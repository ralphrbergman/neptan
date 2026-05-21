#include <asio.hpp>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <source_location>
#include <vector>

using asio::ip::tcp;

enum class LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };

struct Logger {
    std::string filename;

    Logger(const std::string& filename);
    ~Logger();

    bool open();
    void close();

    void log(
        LogLevel level,
        const std::string& message,
        const std::source_location source = std::source_location::current()
    );

    void debug(
        const std::string& message,
        const std::source_location source = std::source_location::current()
    );

    void info(
        const std::string& message,
        const std::source_location source = std::source_location::current()
    );

    void warning(
        const std::string& message,
        const std::source_location source = std::source_location::current()
    );

    void error(
        const std::string& message,
        const std::source_location source = std::source_location::current()
    );

    void critical(
        const std::string& message,
        const std::source_location source = std::source_location::current()
    );

private:
    std::ofstream _file;

    std::string _current_time();
    void _write(const std::string& content);
};

extern Logger* logger;
