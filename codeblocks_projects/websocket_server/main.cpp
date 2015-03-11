#include "./server.hpp"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
/*
How to compile in linux
make -f makefile_webserver_ubuntu.mak
*/

/*
how to compile in cygwin windows
make -f makefile_webserver_cygwin.mak
*/

/* cygwin boost
 * ./b2 -j4 cxxflags="-std=gnu++11" -d+2 --prefix="/cygdrive/c/Program Files/boost/boost_1_53_0" --build-type=complete --layout=tagged --with-date_time --with-thread --with-program_options --with-regex --with-test --with-system --with-serialization --with-graph --with-filesystem --disable-filesystem2 stage
 */

int main(int argc, char* argv[])
{
  try
  {
    // Check command line arguments.
    if (argc != 3)
    {
      std::cerr << "Usage: server <address> <port>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    server 0.0.0.0 7777\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    server 0::0 7777\n";

      return 1;
    }

    std::cout << "Server address: " << argv[1] << std::endl;
    std::cout << "Server port: " << argv[2] << std::endl;
    std::cout << "Server is starting..." << std::endl;

    // Initialise the server.
    websocket::applications::chat::server s(argv[1], argv[2]);

    std::cout << "Server has started." << std::endl;
    std::cout << "Press Ctrl+C (Ctrl+Break) to exit." << std::endl << std::endl;
    // Run the server until stopped.
    s.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
