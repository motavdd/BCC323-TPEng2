#include "config/config.hpp"

#include <asio/ip/address.hpp>
#include <charconv>
#include <cstdlib>
#include <stdexcept>
#include <string_view>
#include <system_error>

namespace bcc323 {
Config load_config()
{
    Config config;
    if (const char* host = std::getenv("HOST")) {
        std::error_code error;
        asio::ip::make_address(host, error);
        if (error) {
            throw std::invalid_argument("HOST must be an IPv4 or IPv6 address");
        }
        config.host = host;
    }
    if (const char* port = std::getenv("PORT")) {
        const std::string_view value(port);
        unsigned int parsed = 0;
        const auto result = std::from_chars(value.data(), value.data() + value.size(), parsed);
        if (result.ec != std::errc{} || result.ptr != value.data() + value.size()
            || parsed == 0 || parsed > 65535) {
            throw std::invalid_argument("PORT must be an integer between 1 and 65535");
        }
        config.port = static_cast<std::uint16_t>(parsed);
    }
    return config;
}
} // namespace bcc323
