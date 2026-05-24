#pragma once

#include <asio.hpp>
#include <nlohmann/json.hpp>

#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <source_location>
#include <thread>
#include <vector>

using asio::ip::tcp;
using json = nlohmann::json;

extern int GLOBAL_ARGC;
extern char** GLOBAL_ARGV;

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

namespace neptah {
    template <typename T>
    inline void send_json(tcp::socket& socket, const T& parseable) {
        json j = parseable;
        std::string json_payload = j.dump();

        uint32_t body_length = static_cast<uint32_t>(json_payload.size());
        auto safe_json_payload = std::make_shared<std::pair<uint32_t, std::string>>(
            body_length,
            json_payload
        );

        std::array<asio::const_buffer, 2> buffers = {
            asio::buffer(&(safe_json_payload->first), sizeof(uint32_t)),
            asio::buffer(safe_json_payload->second)
        };

        asio::async_write(
            socket,
            buffers,
            [safe_json_payload](const asio::error_code& error, size_t transferred) {
                if (!error) logger->debug(std::format("Sent: {}", safe_json_payload->second));
                else {}
            });
    }

    void send(tcp::socket& socket, const std::string& message);
}

inline std::string join_vector(
    const std::vector<std::string>& vector,
    const std::string& delimeter = " "
) {
    if (vector.empty()) return "";

    std::stringstream ss;

    ss << vector[0];

    for (size_t index = 1; index < vector.size(); ++index) {
        ss << delimeter << vector[index];
    }

    return ss.str();
}
