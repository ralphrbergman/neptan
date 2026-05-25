#pragma once
#include "neptah_shared.h"
#include "client_json.h"

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

struct ServerMessage : public ClientMessage {
    ServerMessage(std::string username, std::string content):
    ClientMessage(username, content) {}

    Base header;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ServerMessage, username, content)
};
