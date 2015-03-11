#ifndef WEBSOCKET_APPLICATIONS_CHAT_SESSION_HPP
#define WEBSOCKET_APPLICATIONS_CHAT_SESSION_HPP

#include <string>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "reply.hpp"
#include "request.hpp"
#include "request_parser.hpp"
#include "dataframe.hpp"
#include "dataframe_parser.hpp"
#include "participant.hpp"
#include "room.hpp"

namespace websocket {
    namespace applications {
        namespace chat {

        /// Represents a single connection from a client.
        class session
            : public participant,
            public boost::enable_shared_from_this<session>,
            private boost::noncopyable
        {
        public:
            /// The session states.
            enum state { undefined_state, reading_handshake, bad_request, writing_handshake, pumping_messages } state_;

            /// Construct a connection with the given io_service.
            explicit session(boost::asio::io_service& io_service, room& room);

            /// Get the socket associated with the connection.
            boost::asio::ip::tcp::socket& socket();

            /// Start the first asynchronous operation for the connection.
            void start();

        private:

            char* bufferToCString(char *buff, int buffSize, char *str);
            std::string& bufferToString(char* buffer, int bufflen, std::string& str);

            /// Handle completion of a read operation.
            void handle_read(const boost::system::error_code& e,
                std::size_t bytes_transferred);

            /// Handle completion of a write operation.
            void handle_write(const boost::system::error_code& e);

            /// Deliver a message to the client.
            void deliver(dataframe msg);

            /// Get the client id.
            std::string get_id();

            /// Strand to ensure the connection's handlers are not called concurrently.
            boost::asio::io_service::strand strand_;

            /// Socket for the connection.
            boost::asio::ip::tcp::socket socket_;

            /// The chat room where the messages will be posted.
            room& room_;

            /// The buffer for incoming data.
            boost::array<boost::uint8_t, 1048576> buff_;

            /// The incoming http request.
            http::request req_;

            /// The parser for the incoming http request.
            http::request_parser req_parser_;

            /// The http reply to be sent back to the client.
            http::reply reply_;

            /// The parser for the incoming messages.
            dataframe_parser frame_parser_;

            /// The incoming message.
            dataframe read_frame_;

            /// The messages to be sent to the client.
            message_queue write_msgs_;
        };

        typedef boost::shared_ptr<session> session_ptr;

        } // namespace chat
    } // namespace server
} // namespace websocket

#endif // WEBSOCKET_APPLICATIONS_CHAT_SESSION_HPP
