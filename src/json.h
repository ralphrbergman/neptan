#pragma once
#include "neptah_shared.h"

inline uint64_t get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();

    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
    );
}

struct Base {
    Base():
    created_at(get_timestamp()) {}

    uint64_t created_at;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Base, created_at)
};

struct Message {
    Message(std::string username, std::string content):
    username(username), content(content) {}

    Base header;
    std::string username;
    std::string content;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Message, username, content)
};
