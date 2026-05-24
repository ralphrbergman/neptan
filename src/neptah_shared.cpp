#include "neptah_shared.h"
#include "json.h"

void neptah::send(tcp::socket& socket, const std::string& message) {
    Message message_obj("User", message);
    neptah::send_json(socket, message_obj);
}
