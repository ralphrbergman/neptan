cmake_minimum_required(VERSION 3.10)

project(Neptan LANGUAGES CXX)

add_executable(
    server
    src/server.cpp
    src/logging.cpp
    src/server/command.cpp
    src/server/network.cpp
)
