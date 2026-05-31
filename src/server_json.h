#pragma once
#include "neptah_shared.h"
#include "client_json.h"

struct ServerMessage : public ClientMessage {
    ServerMessage(): ClientMessage("", "") {}

    ServerMessage(std::string username, std::string content):
    header(PacketType::Message),
    ClientMessage(username, content) {}

    Base header;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ServerMessage, header, username, content)
};
