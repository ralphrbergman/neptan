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

enum class PacketType {
    Disconnect,
    Message,
    UsernameAcceptance,
    UsernameRequest
};

NLOHMANN_JSON_SERIALIZE_ENUM(PacketType, {
    { PacketType::Disconnect, "disconnect" },
    { PacketType::Message, "message" },
    { PacketType::UsernameAcceptance, "name_acceptance" },
    { PacketType::UsernameRequest, "name_request" }
})

namespace neptah {
    template <typename T>
    void send_json(tcp::socket& socket, const T& parseable);

    template <typename T>
    void send(tcp::socket& socket, const std::string& username, const std::string& message);
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

#include "neptah_shared.tpp"
