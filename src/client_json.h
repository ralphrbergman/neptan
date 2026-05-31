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
    Base(): created_at(get_timestamp()), type(PacketType::Message) {}

    Base(PacketType type):
    created_at(get_timestamp()),
    type(type) {}

    uint64_t created_at;
    PacketType type;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Base, created_at, type)
};

struct Disconnect {
    Disconnect(): header(PacketType::Disconnect) {}

    Base header;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Disconnect, header)
};

struct ClientMessage {
    ClientMessage(): username(""), content(""), header(PacketType::Message) {}

    ClientMessage(std::string username, std::string content):
    header(PacketType::Message),
    username(username), content(content) {}

    Base header;
    std::string username;
    std::string content;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ClientMessage, header, username, content)
};

struct UsernameAcceptance {
    UsernameAcceptance(): username(""), accepted(false) {}

    UsernameAcceptance(std::string username, bool accepted):
    header(PacketType::UsernameAcceptance),
    username(username), accepted(accepted) {}

    Base header;
    std::string username;
    bool accepted;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UsernameAcceptance, header, username, accepted)
};

struct UsernameRequest {
    UsernameRequest(): username("") {}

    UsernameRequest(std::string username):
    header(PacketType::UsernameRequest),
    username(username) {}

    Base header;
    std::string username;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UsernameRequest, header, username)
};
