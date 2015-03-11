#ifndef WEBSOCKET_HTTP_REQUEST_HPP
#define WEBSOCKET_HTTP_REQUEST_HPP

#include <string>
#include <vector>
#include "header.hpp"

namespace websocket {
    namespace http {

        /// A http request received from a client.
        struct request
        {
            std::string method;
            std::string uri;
            int http_version_major;
            int http_version_minor;
            std::vector<header> headers;
			std::string challenge;
			std::vector<unsigned char> vecchallenge;
        };

    } // namespace http
} // namespace websocket

#endif // WEBSOCKET_HTTP_REQUEST_HPP
