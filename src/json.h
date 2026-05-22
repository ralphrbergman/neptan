#pragma once
#include "neptah_shared.h"

inline uint64_t get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();

    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
    );
}

struct Message {
    Message(std::string username, std::string content):
    username(username), content(content), timestamp(get_timestamp()) {}

    std::string username;
    std::string content;
    uint64_t timestamp;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Message, username, content, timestamp)
