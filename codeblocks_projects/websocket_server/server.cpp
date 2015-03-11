#include "server.hpp"
#include <iostream>
#include <vector>
//#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

namespace websocket {
    namespace applications {
        namespace chat {

        server::server(const std::string& address, const std::string& port)
            : io_service_(),
            signals_(io_service_),
            acceptor_(io_service_)
        {
            // Register to handle the signals that indicate when the server should exit.
            // It is safe to register for the same signal multiple times in a program,
            // provided all registration for the specified signal is made through Asio.
            signals_.add(SIGINT);
            signals_.add(SIGTERM);
#if defined(SIGQUIT)
            signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
            signals_.async_wait(boost::bind(&server::handle_stop, this));

            // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
            boost::asio::ip::tcp::resolver resolver(io_service_);
            boost::asio::ip::tcp::resolver::query query(address, port);
            boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
            acceptor_.open(endpoint.protocol());
            acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            acceptor_.bind(endpoint);
            acceptor_.listen();

            start_accept();
        }

        void server::run()
        {
            //TODO: consider putting this inside a Thread !!! to avoid having it block users thread, perhaps multiple servers are needed, hence the need for thread
            io_service_.run();
        }

        void server::stop()
        {
            handle_stop();
        }

        void server::start_accept()
        {
            new_session_.reset(new session(io_service_, room_));
            acceptor_.async_accept(new_session_->socket(),
                boost::bind(&server::handle_accept, this,
                boost::asio::placeholders::error));
        }

        void server::handle_accept(const boost::system::error_code& error)
        {
            if (!error)
            {
                new_session_->start();
            }
            else
            {
                std::cerr << error.message() << std::endl;
            }

            start_accept();
        }

        void server::handle_stop()
        {
            io_service_.stop();
        }

        } // namespace chat
    } // namespace applications
} // namespace websocket
