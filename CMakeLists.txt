cmake_minimum_required(VERSION 3.10)

project(Neptan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(asio REQUIRED)
find_package(nlohmann_json REQUIRED)

# Server #
add_executable(
    server
    src/server.cpp
    src/logging.cpp
    src/server/command.cpp
    src/server/network.cpp
)

target_link_libraries(
    server
    PRIVATE
        asio::asio
        nlohmann_json::nlohmann_json
)

# Client #
add_executable(
    client
    src/client.cpp
    src/logging.cpp
    src/client/network.cpp
)

target_link_libraries(client PRIVATE asio::asio nlohmann_json::nlohmann_json)
