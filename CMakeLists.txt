cmake_minimum_required(VERSION 3.12)

project(Neptan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# Conan libraries.
find_package(asio REQUIRED)
find_package(nlohmann_json REQUIRED)

# Fetch libraries.
include(FetchContent)

FetchContent_Declare(
    webview
    GIT_REPOSITORY https://github.com/webview/webview.git
    GIT_TAG 0.12.0
)
FetchContent_MakeAvailable(webview)

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

target_link_libraries(
    client
    PRIVATE
        asio::asio
        nlohmann_json::nlohmann_json
        webview_core_static
        Threads::Threads
)

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WEBVIEW_DEPS REQUIRED gtk+-3.0 webkit2gtk-4.1)

    target_compile_options(client PRIVATE ${WEBVIEW_DEPS_CFLAGS_OTHER})
    target_link_libraries(client PRIVATE ${WEBVIEW_DEPS_LIBRARIES})
    target_include_directories(client PRIVATE ${WEBVIEW_DEPS_INCLUDE_DIRS})
endif()
