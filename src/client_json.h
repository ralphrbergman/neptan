#pragma once
#include "neptah_shared.h"

struct ClientMessage {
    ClientMessage(std::string username, std::string content):
    username(username), content(content) {}

    std::string username;
    std::string content;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ClientMessage, username, content)
};
