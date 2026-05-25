#include "neptah_shared.h"

namespace neptah {
    template <typename T>
    inline void send_json(tcp::socket& socket, const T& parseable) {
        json j = parseable;
        std::string json_payload = j.dump();

        uint32_t body_length = static_cast<uint32_t>(json_payload.size());
        auto safe_json_payload = std::make_shared<std::pair<uint32_t, std::string>>(
            body_length,
            json_payload
        );

        std::array<asio::const_buffer, 2> buffers = {
            asio::buffer(&(safe_json_payload->first), sizeof(uint32_t)),
            asio::buffer(safe_json_payload->second)
        };

        asio::async_write(
            socket,
            buffers,
            [safe_json_payload](const asio::error_code& error, size_t transferred) {
                if (!error) logger->debug(std::format("Sent: {}", safe_json_payload->second));
                else {}
            });
    }

    template <typename T>
    void send(
        tcp::socket& socket,
        const std::string& username,
        const std::string& message
    ) {
        T message_obj(username, message);
        neptah::send_json(socket, message_obj);
    }
}
