#include "reply.hpp"
#include <string>

namespace websocket {
    namespace http {

        namespace status_strings {

            const std::string switching_protocols =
                "HTTP/1.1 101 Switching Protocols\r\n";
            const std::string bad_request =
                "HTTP/1.1 400 Bad Request\r\n";
            const std::string internal_server_error =
                "HTTP/1.1 500 Internal Server Error\r\n";
            const std::string handshake_protocols =
                "HTTP/1.1 101 WebSocket Protocol Handshake\r\n"; 

            boost::asio::const_buffer to_buffer(reply::status_type status)
            {
                switch (status)
                {
                case reply::switching_protocols:
                    return boost::asio::buffer(switching_protocols);
                case reply::handshake:
                    return boost::asio::buffer(handshake_protocols);
                case reply::bad_request:
                    return boost::asio::buffer(bad_request);
                default:
                    return boost::asio::buffer(internal_server_error);
                }
            }

        } // namespace status_strings

        namespace misc_strings {

            const char name_value_separator[] = { ':', ' ' };
            const char crlf[] = { '\r', '\n' };

        } // namespace misc_strings

        std::vector<boost::asio::const_buffer> reply::to_buffers()
        {
            std::vector<boost::asio::const_buffer> buffers;
            buffers.push_back(status_strings::to_buffer(status));

            for (std::size_t i = 0; i < headers.size(); ++i)
            {
                header& h = headers[i];
                buffers.push_back(boost::asio::buffer(h.name));
				if(h.name!=""){// old version of websocket have a blank line before last challenge line
					buffers.push_back(boost::asio::buffer(misc_strings::name_value_separator));
				}				
				buffers.push_back(boost::asio::buffer(h.value));
				
                buffers.push_back(boost::asio::buffer(misc_strings::crlf));
            }

            buffers.push_back(boost::asio::buffer(misc_strings::crlf));

            return buffers;
        }

        reply reply::stock_reply(reply::status_type status)
        {
            reply rep;
            rep.status = status;

            return rep;
        }

    } // namespace http
} // namespace websocket
