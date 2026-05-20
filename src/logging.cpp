#include "neptah_shared.h"
#include <chrono>

template <>
struct std::formatter<LogLevel> : std::formatter<std::string_view> {
    auto format(LogLevel level, format_context& ctx) const {
        std::string_view name = "UNKNOWN";

        switch (level) {
            case LogLevel::DEBUG: name = "DEBUG"; break;
            case LogLevel::INFO: name = "INFO"; break;
            case LogLevel::WARNING: name = "WARNING"; break;
            case LogLevel::ERROR: name = "ERROR"; break;
            case LogLevel::CRITICAL: name = "CRITICAL"; break;
        }

        return std::formatter<std::string_view>::format(name, ctx);
    }
};

Logger::Logger(const std::string& filename):
filename(filename) {
    if (open()) this->info("Successfully opened log file: " + filename);
    else this->error("Could not open log file: " + filename);
}

Logger::~Logger() {}

bool Logger::open() {
    if (!_file.is_open()) {
        _file.open(filename, std::ios::out | std::ios::app);
    }

    return _file.is_open();
}

void Logger::close() {
    if (_file.is_open()) {
        _file.close();
    }

    this->info("Successfully closed log file: " + this->filename);
}

void Logger::log(
    LogLevel level,
    const std::string& message,
    const std::source_location source
) {
    const std::string log_message = std::format(
        "[{}] {} | {}:{} - {}\n",
        _current_time(),
        level,
        source.file_name(),
        source.line(),
        message
    );

    if (_file.is_open()) _write(log_message);
    std::cout << log_message;
}

void Logger::debug(const std::string& message, const std::source_location source) {
    this->log(LogLevel::DEBUG, message, source);
}

void Logger::info(const std::string& message, const std::source_location source) {
    this->log(LogLevel::INFO, message, source);
}

void Logger::warning(const std::string& message, const std::source_location source) {
    this->log(LogLevel::WARNING, message, source);
}

void Logger::error(const std::string& message, const std::source_location source) {
    this->log(LogLevel::ERROR, message, source);
}

void Logger::critical(const std::string& message, const std::source_location source) {
    this->log(LogLevel::CRITICAL, message, source);
}

std::string Logger::_current_time() {
    auto now = std::chrono::system_clock::now();

    return std::format("{:%Y-%m-%dT%H:%M:%S:%Z}", now);
}

void Logger::_write(const std::string& content) {
    if (_file.is_open()) _file << content;
    else this->error("Could not write content: " + content + " to log file: " + filename);
}

Logger* logger = nullptr;
