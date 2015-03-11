#ifndef WEBSOCKET_APPLICATIONS_CHAT_SERVER_HPP
#define WEBSOCKET_APPLICATIONS_CHAT_SERVER_HPP
// http://www.boost.org/doc/libs/1_53_0/doc/html/boost_asio/overview/core/async.html
// \boost_1_53_0\doc\html\boost_asio\example\chat
//
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "room.hpp"
#include "session.hpp"
#include "request_handler.hpp"
// problems with -mt-sgd-1_53.lib
// http://boost.2283326.n4.nabble.com/Question-about-building-VS2012-mt-sgd-td4638130.html
// Invoke b2 with runtime-link=static. This will produce required boost
// libs (mt-s, mt-sgd). I think you can also pass --build-type=complete to
// get all variants.
// http://www.boost.org/doc/libs/1_53_0/more/getting_started/windows.html
// http://www.boost.org/doc/display_build.php/boost-build/boost-build/doc/html/bbv2/overview/configuration.html
// http://www.boost.org/doc/libs/1_53_0/doc/html/bbv2/tutorial.html#bbv2.tutorial.linkage
//
// http://www.boost.org/doc/libs/1_53_0/doc/html/boost_asio/examples.html
namespace websocket {
    namespace applications {
        namespace chat {

        /// The top-level class of the chat server.
        class server
            : private boost::noncopyable
        {
        public:
            /// Construct the server to listen on the specified TCP address and port, and
            /// serve up files from the given directory.
            explicit server(const std::string& address, const std::string& port);

            /// Run the server's io_service loop.
            void run();

            /// Stop the server's io_service loop.
            void stop();

        private:
            /// Initiate an asynchronous accept operation.
            void start_accept();

            /// Handle completion of an asynchronous accept operation.
            void handle_accept(const boost::system::error_code& e);

            /// Handle a request to stop the server.
            void handle_stop();

            /// The io_service used to perform asynchronous operations.
            boost::asio::io_service io_service_;

            /// The signal_set is used to register for process termination notifications.
            boost::asio::signal_set signals_;

            /// Acceptor used to listen for incoming connections.
            boost::asio::ip::tcp::acceptor acceptor_;

            /// The next connection to be accepted.
            session_ptr new_session_;

            /// The handler for all incoming requests.
            http::request_handler request_handler_;

            /// The chat room.
            room room_;
        };

        } // namespace chat
    } // namespace applications
} // namespace websocket


#endif // WEBSOCKET_APPLICATIONS_CHAT_SERVER_HPP
