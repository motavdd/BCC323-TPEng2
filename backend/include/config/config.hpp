#pragma once

#include <cstdint>
#include <string>

namespace bcc323 {
    struct Config {
        std::string host = "127.0.0.1";
        std::uint16_t port = 8081;
    };


    Config load_config();
} // namespace bcc323
