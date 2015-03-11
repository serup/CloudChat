// Compile with:
// g++ -std=gnu++0x example-client-cpp11.cpp -o example-client-cpp11
#include "easywsclient.hpp"
#include "easywsclient.cpp" // <-- include only if you don't want compile separately
#include <assert.h>
#include <stdio.h>
#include <string>

int main()
{
    using easywsclient::WebSocket;
    WebSocket::pointer ws = WebSocket::from_url("ws://77.75.165.130:7777");
    assert(ws);
    ws->send("message from client");
    ws->send("hello");
    while (ws->getReadyState() != WebSocket::CLOSED) {
        ws->poll();
        ws->dispatch([ws](const std::string & message) {
            printf(">>> %s\n", message.c_str());
            if (message == "world") { ws->close(); }
        });
    }
    return 0;
}
