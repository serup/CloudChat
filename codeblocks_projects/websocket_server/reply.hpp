#ifndef WEBSOCKET_HTTP_REPLY_HPP
#define WEBSOCKET_HTTP_REPLY_HPP

#include <vector>
#include <boost/asio.hpp>
#include "header.hpp"

namespace websocket {
    namespace http {

        /// A http reply to be sent to a client.
        struct reply
        {
            /// The status of the reply.
            enum status_type
            {
                switching_protocols = 101,
                handshake = 102, 
                bad_request = 400,
                internal_server_error = 500,
            } status;

            /// The headers to be included in the reply.
            std::vector<header> headers;

            /// Convert the reply into a vector of buffers. The buffers do not own the
            /// underlying memory blocks, therefore the reply object must remain valid and
            /// not be changed until the write operation has completed.
            std::vector<boost::asio::const_buffer> to_buffers();

            /// Get a stock reply.
            static reply stock_reply(status_type status);
        };

    } // namespace http
} // namespace websocket

#endif // WEBSOCKET_HTTP_REPLY_HPP
