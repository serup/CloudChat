#include "wsclient.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>

#include <vector>
#include <string>
//#include <sys/select.h>

namespace wsclient {

int server_client_connect(std::string hostname, int port) {
    struct addrinfo hints;
    struct addrinfo *result;
    struct addrinfo *p;
    int ret;
    int sockfd = -1;
    char sport[16];
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    snprintf(sport, 16, "%d", port);
    if ((ret = getaddrinfo(hostname.c_str(), sport, &hints, &result)) != 0)
    {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
      return 1;
    }
    for(p = result; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) { continue; }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) != -1) {
            break;
        }
        close(sockfd);
        sockfd = -1;
    }
    freeaddrinfo(result);
    return sockfd;
}

struct _DummyClientWebSocket : public ClientWebSocket
{
    void poll() { }
    void close() { }
    bool _dispatch(Callback & callable) { return true; }
    readyStateValues getReadyState() { return CLOSED; }
    void start(void (*pt2Function)(void* pParent)) { }
    bool insertDataframeIn_txbuf(dataframe frm) { return false; } // should be overloaded
    void join() { }
    void processQueue() { }
    void setOwner(void* pParent) { }
    void*  pOwner = 0;
};



struct _RealClientWebSocket : public ClientWebSocket
{
    #if 0
    http://tools.ietf.org/html/rfc6455#section-5.2  Base Framing Protocol

     0                   1                   2                   3
     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-------+-+-------------+-------------------------------+
    |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
    |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
    |N|V|V|V|       |S|             |   (if payload len==126/127)   |
    | |1|2|3|       |K|             |                               |
    +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
    |     Extended payload length continued, if payload len == 127  |
    + - - - - - - - - - - - - - - - +-------------------------------+
    |                               |Masking-key, if MASK set to 1  |
    +-------------------------------+-------------------------------+
    | Masking-key (continued)       |          Payload Data         |
    +-------------------------------- - - - - - - - - - - - - - - - +
    :                     Payload Data continued ...                :
    + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
    |                     Payload Data continued ...                |
    +---------------------------------------------------------------+
    #endif
    struct wsheader_type {
        int header_size;
        bool fin;
        bool mask;
        enum opcode_type {
            CONTINUATION = 0x0,
            TEXT_FRAME = 0x1,
            BINARY_FRAME = 0x2,
            CLOSE = 8,
            PING = 9,
            PONG = 0xa,
        } opcode;
        int N0;
        uint64_t N;
        uint8_t masking_key[4];
    };

    //
    std::vector<uint8_t> rxbuf;
    std::vector<uint8_t> txbuf;
    std::vector<uint8_t> receivedData;

    int sockfd;
    readyStateValues readyState;

    boost::thread m_Thread; // used when spawning a thread for handling poll()  -- receiving,sending dataframes
    void* pOwner;

    _RealClientWebSocket(int sockfd) : sockfd(sockfd), readyState(OPEN) {
    }

    readyStateValues getReadyState() {
      return readyState;
    }

    void start(void (*pt2Function)(void* pParent))
    {
        pt2YourFunction = pt2Function;
        m_Thread = boost::thread(&ClientWebSocket::processQueue, this);
    }

    void join()
    {
        m_Thread.join();
    }
    void processQueue()
    {
        while(readyState != CLOSED) {
            poll();
            if(pt2YourFunction == 0)
            {
                dispatch([this](void * pParent) // lambda functionality
                {
                    printf("***************************************************************************\n");
                    printf("WARNING No callback function added, thus this default catcher function \n");
                    if (pParent == 0) { printf(">>> no data received\n"); }
                    printf("***************************************************************************\n");
                });
            }
            else
                dispatch(pt2YourFunction);
        }
    }

    void setOwner(void* pParent)
    {
        pOwner = pParent;
    }

    /** \brief Poll will read incomming dataframes on socket and put in buffer, also send dataframes in outbuffer to socket - should run in its own thread
     *
     * \return void
     *
     */
    void poll() { // timeout in milliseconds
        if (readyState == CLOSED) {
//            if (timeout > 0) {
//                timeval tv = { timeout/1000, (timeout%1000) * 1000 };
//                select(0, NULL, NULL, NULL, &tv);
//            }
            return;
        }

            long timeout = 10000;
//        if (timeout > 0) {
            fd_set rfds;
            fd_set wfds;
            timeval tv = { timeout/1000, (timeout%1000) * 1000 };
            FD_ZERO(&rfds);
            FD_ZERO(&wfds);
            //FD_SET(sockfd, &rfds);
//            if (rxbuf.size()) { FD_SET(sockfd, &rfds); }
//            select(sockfd + 1, &rfds, &wfds, NULL, &tv);
//        }

//        unsigned long SOCKET_READ_TIMEOUT_SEC = 10;
//        struct timeval timeout;
//        timeout.tv_sec = SOCKET_READ_TIMEOUT_SEC;
//        timeout.tv_usec = 0;
//        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        //setsockopt(sockfd, SOL_SOCKET, SO_RCVLOWAT, &timeout, sizeof(timeout));

        while (txbuf.size()) {
            int ret;
            ret = ::send(sockfd, &txbuf[0], txbuf.size(), 0);
            if (ret > 0) { txbuf.erase(txbuf.begin(), txbuf.begin() + ret); }
            else { break; }
        }
        if (!txbuf.size() && readyState == CLOSING) {
            ::close(sockfd);
            readyState = CLOSED;
        }
        while (true) {
            // FD_ISSET(0, &rfds) will be true
            int N = rxbuf.size();
            ssize_t ret;
            rxbuf.resize(N + 1500);
            ret = ::recv(sockfd, &rxbuf[0] + N, 1500, 0);
            if (false) { }
            else if (ret < 0) {
                if(errno == 11)
                {
                    /// No data available yet - lets wait a bit
                    usleep(100); // wait milliseconds
                }
                else {
                    fprintf (stderr, "Error no is : %d\n", errno);
                    fprintf(stderr, "Error description is : %s\n",strerror(errno));
                }
                rxbuf.resize(N);
                break;
            }
            else if (ret == 0) {
                rxbuf.resize(N);
                ::close(sockfd);
                readyState = CLOSED;
                fprintf(stderr, "Connection closed!\n");
                break;
            }
            else {
                rxbuf.resize(N + ret);
            }


        }

    }


    bool _dispatch(Callback & callable)
    {
        bool bResult = false; // Result will be true if dataframe is read
        while (true) {
            wsheader_type ws;
            if (rxbuf.size() < 2) { return bResult; /* Need at least 2 */ }
            const uint8_t * data = (uint8_t *) &rxbuf[0]; // peek, but don't consume
            ws.fin = (data[0] & 0x80) == 0x80;
            ws.opcode = (wsheader_type::opcode_type) (data[0] & 0x0f);
            ws.mask = (data[1] & 0x80) == 0x80;
            ws.N0 = (data[1] & 0x7f);
//            ws.header_size = 2 + (ws.N0 == 126? 2 : 0) + (ws.N0 == 127? 6 : 0) + (ws.mask? 4 : 0);
            ws.header_size = 2 + (ws.N0 == 126? 2 : 0) + (ws.N0 == 127? 8 : 0) + (ws.mask? 4 : 0); /// http://tools.ietf.org/html/rfc6455#section-5.2
            if (rxbuf.size() < (unsigned int)ws.header_size) { return bResult; /* Need: ws.header_size - rxbuf.size() */ }
            int i;
            if (ws.N0 < 126) {
                ws.N = ws.N0;
                i = 2;
            }
            else if (ws.N0 == 126) {
                ws.N = 0;
                ws.N |= ((uint64_t) data[2]) << 8;
                ws.N |= ((uint64_t) data[3]) << 0;
                i = 4;
            }
            else if (ws.N0 == 127) {
                ws.N = 0;
                ws.N |= ((uint64_t) data[2]) << 56;
                ws.N |= ((uint64_t) data[3]) << 48;
                ws.N |= ((uint64_t) data[4]) << 40;
                ws.N |= ((uint64_t) data[5]) << 32;
                ws.N |= ((uint64_t) data[6]) << 24;
                ws.N |= ((uint64_t) data[7]) << 16;
                ws.N |= ((uint64_t) data[8]) << 8;
                ws.N |= ((uint64_t) data[9]) << 0;
                i = 10;
            }
            if (ws.mask) {
                ws.masking_key[0] = ((uint8_t) data[i+0]) << 0;
                ws.masking_key[1] = ((uint8_t) data[i+1]) << 0;
                ws.masking_key[2] = ((uint8_t) data[i+2]) << 0;
                ws.masking_key[3] = ((uint8_t) data[i+3]) << 0;
            }
            else {
                ws.masking_key[0] = 0;
                ws.masking_key[1] = 0;
                ws.masking_key[2] = 0;
                ws.masking_key[3] = 0;
            }
            if (rxbuf.size() < ws.header_size+ws.N) { return bResult; /* Need: ws.header_size+ws.N - rxbuf.size() */ }
//            if (ws.fin == false) {
//                    return bResult;
//            }

            // We got a whole message, now do something with it:
            if (false) { }
//            else if (ws.opcode == wsheader_type::TEXT_FRAME && ws.fin) {
//                if (ws.mask) { for (size_t i = 0; i != ws.N; ++i) { rxbuf[i+ws.header_size] ^= ws.masking_key[i&0x3]; } }
//                std::vector<uint8_t> data;
//                std::copy(rxbuf.begin()+ws.header_size, rxbuf.begin()+ws.header_size+ws.N, std::back_inserter(data));
//                data.push_back(0);
//                callable(&data[0]); // direct pointer to txt
//                bResult=true;
//            }
            else if (ws.opcode == wsheader_type::TEXT_FRAME ) { }
//            else if (ws.opcode == wsheader_type::BINARY_FRAME && ws.fin) {
//                if (ws.mask) { for (size_t i = 0; i != ws.N; ++i) { rxbuf[i+ws.header_size] ^= ws.masking_key[i&0x3]; } }
//                std::vector<uint8_t> data;
//                data.resize(ws.N);
//                data.clear();// remove potential garbage
//                std::copy(rxbuf.begin()+ws.header_size, rxbuf.begin()+ws.header_size+ws.N, std::back_inserter(data));
//                data.push_back(0);
//
//                CDED ded;
//                ded.pDEDarray = &data[0];
//                ded.sizeofDED = data.size()-1;
////                ded.pOwner = this;
//                ded.pOwner = this->pOwner;
//                callable(&ded); // direct pointer to DED class with DED object in pDEDarray -- callable function must beable to parse the object!!
//
//                //rxbuf.clear(); // ready for next receiving next dataframe
//                bResult=true;
//            }
            else if (ws.opcode == wsheader_type::BINARY_FRAME || ws.opcode == wsheader_type::CONTINUATION) {
                if (ws.mask) { for (size_t i = 0; i != ws.N; ++i) { rxbuf[i+ws.header_size] ^= ws.masking_key[i&0x3]; } }
                receivedData.insert(receivedData.end(), rxbuf.begin()+ws.header_size, rxbuf.begin()+ws.header_size+(size_t)ws.N);// just feed
                if (ws.fin) {
                    CDED ded;
                    receivedData.push_back(0);
                    ded.pDEDarray = &receivedData[0];
                    ded.sizeofDED = receivedData.size()-1;
                    ded.pOwner = this->pOwner;
                    callable(&ded); // direct pointer to DED class with DED object in pDEDarray -- callable function must beable to parse the object!!
                    receivedData.erase(receivedData.begin(), receivedData.end());
                    std::vector<uint8_t> ().swap(receivedData);// free memory
                    bResult=true;
                }
            }

            else if (ws.opcode == wsheader_type::PING) { }
            else if (ws.opcode == wsheader_type::PONG) { }
            else if (ws.opcode == wsheader_type::CLOSE) { close(); }
            else { fprintf(stderr, "ERROR: Got unexpected WebSocket message.\n"); close(); }
//            else
//            {
//                fprintf(stderr, "[websocket_server wsclient.cpp] ERROR: Got unexpected WebSocket message. Will try to clean rxbuf\n");
//                //rxbuf.erase(rxbuf.begin(), rxbuf.end());
//                rxbuf.clear();
//            }

            rxbuf.erase(rxbuf.begin(), rxbuf.begin() + ws.header_size+(size_t)ws.N);
        }
        return bResult;
    }


    bool insertDataframeIn_txbuf(dataframe frm)
    {
        bool bResult=true;
        std::vector<boost::asio::const_buffer> buffers = frm.to_buffers();
        std::vector<uint8_t> tmpbuf(boost::asio::buffer_size(buffers));
        boost::asio::buffer_copy(boost::asio::buffer(tmpbuf), buffers);
        txbuf.insert(txbuf.end(), tmpbuf.begin(), tmpbuf.end());
        if(txbuf.size()<tmpbuf.size())
            bResult=false;
        return bResult;
    }

    void close() {
        if(readyState == CLOSING || readyState == CLOSED) { return; }
        readyState = CLOSING;
        uint8_t closeFrame[4] = {0x88, 0x00, 0x00, 0x00};
        std::vector<uint8_t> header(closeFrame, closeFrame+4);
        txbuf.insert(txbuf.end(), header.begin(), header.end());
    }

};






ClientWebSocket::pointer ClientWebSocket::create_dummy() {
    static pointer dummy = pointer(new _DummyClientWebSocket);
    return dummy;
}

ClientWebSocket::pointer ClientWebSocket::from_url(std::string url,void* pParent) {
    char host[128];
    int port;
    char path[128];
    if (false) { }
    else if (sscanf(url.c_str(), "ws://%[^:/]:%d/%s", host, &port, path) == 3) {
    }
    else if (sscanf(url.c_str(), "ws://%[^:/]/%s", host, path) == 2) {
        port = 80;
    }
    else if (sscanf(url.c_str(), "ws://%[^:/]:%d", host, &port) == 2) {
        path[0] = '\0';
    }
    else if (sscanf(url.c_str(), "ws://%[^:/]", host) == 1) {
        port = 80;
        path[0] = '\0';
    }
    else {
        fprintf(stderr, "ERROR: Could not parse WebSocket url: %s\n", url.c_str());
        return NULL;
    }
    fprintf(stderr, "wsclient: connecting: host=%s port=%d path=/%s\n", host, port, path);
    int sockfd = server_client_connect(host, port);
    if (sockfd == -1) {
        fprintf(stderr, "Unable to connect to %s:%d\n", host, port);
        return NULL;
    }
    {
        // XXX: this should be done non-blocking,
        char line[256];
        int status;
        int i;
        snprintf(line, 256, "GET /%s HTTP/1.1\r\n", path); ::send(sockfd, line, strlen(line), 0);
        snprintf(line, 256, "Host: %s:%d\r\n", host, port); ::send(sockfd, line, strlen(line), 0);
        snprintf(line, 256, "Upgrade: websocket\r\n"); ::send(sockfd, line, strlen(line), 0);
        snprintf(line, 256, "Connection: Upgrade\r\n"); ::send(sockfd, line, strlen(line), 0);
        snprintf(line, 256, "Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n"); ::send(sockfd, line, strlen(line), 0);
        snprintf(line, 256, "Sec-WebSocket-Version: 13\r\n"); ::send(sockfd, line, strlen(line), 0);
        snprintf(line, 256, "\r\n"); ::send(sockfd, line, strlen(line), 0);
//        for (i = 0; i < 2 || i < 255 && line[i-2] != '\r' && line[i-1] != '\n'; ++i) { if (recv(sockfd, line+i, 1, 0) == 0) { return NULL; } }
        for (i = 0; (i < 2 || i < 255) && line[i-2] != '\r' && line[i-1] != '\n'; ++i) { if (recv(sockfd, line+i, 1, 0) == 0) { return NULL; } }
        line[i] = 0;
        if (i == 255) { fprintf(stderr, "ERROR: Got invalid status line connecting to: %s\n", url.c_str()); return NULL; }
        if (sscanf(line, "HTTP/1.1 %d", &status) != 1 || status != 101) { fprintf(stderr, "ERROR: Got bad status connecting to %s: %s", url.c_str(), line); return NULL; }
        // TODO: verify response headers,
        while (true) {
//            for (i = 0; i < 2 || i < 255 && line[i-2] != '\r' && line[i-1] != '\n'; ++i) { if (recv(sockfd, line+i, 1, 0) == 0) { return NULL; } }
            for (i = 0; (i < 2 || i < 255) && line[i-2] != '\r' && line[i-1] != '\n'; ++i) { if (recv(sockfd, line+i, 1, 0) == 0) { return NULL; } }
            if (line[0] == '\r' && line[1] == '\n') { break; }
        }
    }
    int flag = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(flag)); // Disable Nagle's algorithm
    fcntl(sockfd, F_SETFL, O_NONBLOCK); // SOCKET is set as NON-BLOCKING !!!!
    fprintf(stderr, "Connected to: %s\n", url.c_str());
//    return pointer(new _RealClientWebSocket(sockfd));
    ClientWebSocket::pointer pointer(new _RealClientWebSocket(sockfd));
    pointer->setOwner(pParent);
    return pointer;
}

} // namespace wsclient
