#ifndef WEBSOCKET_HTTP_HEADER_HPP
#define WEBSOCKET_HTTP_HEADER_HPP

#include <string>

namespace websocket {
    namespace http {

        struct header
        {
            std::string name;
            std::string value;
        };

    } // namespace http
} // namespace websocket

#endif // WEBSOCKET_HTTP_HEADER_HPP