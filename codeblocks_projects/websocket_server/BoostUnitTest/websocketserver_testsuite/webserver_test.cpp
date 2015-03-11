#define BOOST_TEST_MODULE webservertest
#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include "../../server.hpp"
#include "../../easywsclient/easywsclient.hpp"
#include "../../wsclient/wsclient.hpp"
#include <type_traits>
#include <iostream>
#include <string>
//#include "../../../Ringbuffer/ringbuffer.hpp"
#include "../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h"
#include "../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h"
using namespace std;
using namespace boost::unit_test;

//////////////////////////////////////////
// TESTCASE
//////////////////////////////////////////
// howto compile :
// use makefile_cygwin.mak from inside cygwin terminal :
// use makefile_ubuntu.mak from linux prompt
//
// ex.
// make -f makefile_cygwin.mak
// make -f makefile_ubuntu.mak
// NB! makefile will not only build and link, it will also run the testcase
//     and convert the generated result output into the file test_result.html
//////////////////////////////////////////
// how to test:
// make -f makefile_cygwin.mak test
// This will convert test_results.xml file to test_results.html, based on test_results.xslt file
//////////////////////////////////////////
#define BOOST_AUTO_TEST_MAIN
//#ifndef NOTESTRESULTFILE
#ifdef BOOST_AUTO_TEST_MAIN
std::ofstream out;

struct ReportRedirector
{
    ReportRedirector()
    {
        out.open("test_results.xml");
        assert( out.is_open() );
        boost::unit_test::results_reporter::set_stream(out);
    }
};

BOOST_GLOBAL_FIXTURE(ReportRedirector)
//#endif
#endif

/**
	 * C++ version 0.4 std::string style "itoa":
	 * Contributions from Stuart Lowe, Ray-Yuan Sheu,

	 * Rodrigo de Salvo Braz, Luc Gallant, John Maloney
	 * and Brian Hunt
	 *
	 * or use : http://stackoverflow.com/questions/4668760/converting-an-int-to-stdstring
	 * boost::lexical_cast<std::string>(yourint) from boost/lexical_cast.hpp
	 */
std::string itoa(int value, int base)
{

		std::string buf;

		// check that the base if valid
		if (base < 2 || base > 16) return buf;

		enum { kMaxDigits = 35 };
		buf.reserve( kMaxDigits ); // Pre-allocate enough space.


		int quotient = value;

		// Translating number to string with base:
		do {
			buf += "0123456789abcdef"[ std::abs( quotient % base ) ];
			quotient /= base;
		} while ( quotient );

		// Append the negative sign
		if ( value < 0) buf += '-';

		std::reverse( buf.begin(), buf.end() );
		return buf;
}


BOOST_AUTO_TEST_SUITE (webservertest) // name of the test suite
/*
BOOST_AUTO_TEST_CASE(errortest)
{
  BOOST_CHECK(3087 == 3086);
}
*/


BOOST_AUTO_TEST_CASE(InstantiateWebServerObject)
{
    std::string address=(std::string)"0.0.0.0";
    std::string port=(std::string)"7766";
    bool bInstantiated = false;
    websocket::applications::chat::server *pServer = new websocket::applications::chat::server(address,port);

    if(pServer)
        bInstantiated=true;

    BOOST_CHECK(bInstantiated == true);

    if(pServer)
        delete pServer;
}


BOOST_AUTO_TEST_CASE(InstantiateWebClientObject)
{
    std::string strTmpMessage = (std::string)"hello from unittest";
    bool bInstantiated = false;
    using easywsclient::WebSocket;
    WebSocket::pointer ws = WebSocket::from_url("ws://77.75.165.130:7755");

    if(ws){
        bInstantiated=true;
        ws->close();
        delete ws;
    }

    BOOST_CHECK(bInstantiated == true);
}

class WebServer
{
public:

    websocket::applications::chat::server *pServer;

    WebServer()
    {
        // the thread is not-a-thread until we call start()
        pServer=nullptr;
    }

    void start(std::string address, std::string port)
    {
        m_Thread = boost::thread(&WebServer::processQueue, this, address,port);
    }

    void join()
    {
        m_Thread.join();
    }

    void stop()
    {
        if(pServer){
            pServer->stop();
        }
    }

    void processQueue(std::string address, std::string port)
    {
        //float ms = N * 1e3;
        //boost::posix_time::milliseconds workTime(ms);

        std::cout << "WebServer: started" << std::endl;
        std::cout << std::endl;

        // We're busy, honest!
        //http://www.boost.org/doc/libs/1_54_0/doc/html/thread.html
        //http://en.wikipedia.org/wiki/Futures_and_promises
        //http://stackoverflow.com/questions/8800341/c-boost-promise-boost-unique-future-and-move-semantics
        //http://www.boost.org/doc/libs/1_54_0/doc/html/thread/thread_management.html
        //http://thbecker.net/articles/rvalue_references/section_01.html
        //http://antonym.org/2009/05/threading-with-boost---part-i-creating-threads.html

//        std::string address=(std::string)"127.0.0.1";
//        std::string port=(std::string)"8778";
        bool bServerInstantiated = false;
        pServer = new websocket::applications::chat::server(address,port);

        if(pServer){
            bServerInstantiated=true;
            pServer->run();
        }

        std::cout << "WebServer: stopped" << std::endl;

        //boost::this_thread::sleep(workTime);

        std::cout << "WebServer thread: ended" << std::endl;
    }

private:

    boost::thread m_Thread;
};


BOOST_AUTO_TEST_CASE(easyWebClientCommunicateWithLocalServer)
{
    bool bClientInstantiated = false;
    using easywsclient::WebSocket;

    WebServer server;
    server.start("127.0.0.1","8668");
    // Three seconds of pure, hard work!
    boost::posix_time::seconds workTime(1);
    boost::this_thread::sleep(workTime);

    WebSocket::pointer ws = WebSocket::from_url("ws://127.0.0.1:8668");
    if(ws){
        bClientInstantiated=true;
        ws->send((std::string)"hello from unittest");
        ws->poll();// first send
        ws->poll();// second receive answer
        std::string text = ws->fetchtext();// third fetch the text received
        std::cout << "LocalServer responded with: " << (std::string)text << std::endl;
        ws->close();
        delete ws;
    }

    server.stop();// signal stop to server
     // Three seconds of pure, hard work!
    boost::this_thread::sleep(workTime);

   BOOST_CHECK(bClientInstantiated == true);
}

BOOST_AUTO_TEST_CASE(NewWebClientClassObject)
{
    std::string strTmpMessage = (std::string)"hello from unittest, using wsclient";
    bool bInstantiated = false;

    WebServer server;
    server.start("127.0.0.1","8778");

    // seconds of pure, hard work!
    boost::posix_time::seconds workTime(1);
    boost::this_thread::sleep(workTime);

    using wsclient::ClientWebSocket;
    ClientWebSocket::pointer ws = ClientWebSocket::from_url("ws://127.0.0.1:8778",this);

    if(ws){
        bInstantiated=true;
        ws->close();
        delete ws;
    }

    server.stop();// signal stop to server
    // seconds of pure, hard work!
    boost::this_thread::sleep(workTime);

    BOOST_CHECK(bInstantiated == true);
}

void handle_server_response(void *pParent)
{
    bool bDecoded=false;
    std::string strMethod="";
    std::string strProtocolTypeID="";
    std::string strFunctionName="";
    std::string strStatus="";
    unsigned short iTransID=0;
    bool bValue=false;

    if (pParent == 0) { printf(">>> ERROR null pointer received\n"); return; }

    wsclient::CDED* pDED = (wsclient::CDED*)pParent;
    if (pDED->pOwner == 0) { printf(">>> ERROR NOT VALID DED object received -- could be a standard text_frame (%s), it should be a binary_frame\n",pParent); }
    else {
//    ConsumeFromRingbuffer<RingBuffer<unsigned char>>* pOwner = (ConsumeFromRingbuffer<RingBuffer<unsigned char>>*) pDED->pOwner;

    pDED->status = "DED not yet inside";

    DED_PUT_DATA_IN_DECODER(decoder_ptr,(unsigned char*)pDED->pDEDarray,pDED->sizeofDED);
    BOOST_CHECK(decoder_ptr != 0);


    // decode data ...
    if( DED_GET_STRUCT_START( decoder_ptr, "WSResponse" ) &&
         DED_GET_METHOD	( decoder_ptr, "Method", strMethod ) &&
         DED_GET_USHORT	( decoder_ptr, "TransID", iTransID) &&
         DED_GET_STDSTRING	( decoder_ptr, "protocolTypeID", strProtocolTypeID ) &&
         DED_GET_STDSTRING	( decoder_ptr, "functionName", strFunctionName ) &&
         DED_GET_STDSTRING	( decoder_ptr, "status", strStatus ) &&
        DED_GET_STRUCT_END( decoder_ptr, "WSResponse" ))
    {
        bDecoded=true;
        std::string tmp("DED inside, status = ");
        pDED->status = tmp + strStatus;
    }
    else
    {
        pDED->status = (std::string)"WARNING: NO legal DED in dataframe payload";
        printf("%s\n",pDED->status.c_str());
        bDecoded=false;
    }

    }
  	BOOST_CHECK(bDecoded == true);
	BOOST_CHECK(strMethod == "DFDconnect");
	BOOST_CHECK(iTransID == 8); // Transaktionsid are used for making sure that the handshaking is happening between the 2 correct entities. fx. client sends a frame with transid = 8 and server response with a frame with transid = 8
	BOOST_CHECK(strProtocolTypeID == "DED1.00.00");
	BOOST_CHECK(strFunctionName == "DFD_1.1.1");
	BOOST_CHECK(strStatus == "ACCEPTED");

}

BOOST_AUTO_TEST_CASE(NewWebClientCommunicateWithLocalServer)
{
    bool bInstantiated = false;

    /// start a local webserver, based on current version
    WebServer server;
    server.start("127.0.0.1","9779");

    /// wait some time for server to start
    boost::posix_time::seconds workTime(1);
    boost::this_thread::sleep(workTime);

    /// use new webclient class to communicate with local webserver
    using wsclient::ClientWebSocket;
    ClientWebSocket::pointer ws = ClientWebSocket::from_url("ws://127.0.0.1:9779", this);

    if(ws){
        bInstantiated=true;
        /// Simulate dataframe with DED protocol handling connection request to server from client DFDs function
        DED_START_ENCODER(encoder_ptr);
          DED_PUT_STRUCT_START( encoder_ptr, "DFDRequest" );
            DED_PUT_METHOD	( encoder_ptr, "Method",  "DFDconnect" );
            DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)8);
            DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)"DED1.00.00" );
            DED_PUT_STDSTRING	( encoder_ptr, "functionName", (std::string)"DFD_1.1.1" );
          DED_PUT_STRUCT_END( encoder_ptr, "DFDRequest" );
        DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);

        /// Create a binary dataframe
        wsclient::dataframe frame;
        frame.opcode = wsclient::dataframe::operation_code::binary_frame;

        if(sizeofCompressedData==0) sizeofCompressedData = iLengthOfTotalData; // if sizeofcompresseddata is 0 then compression was not possible and size is the same as for uncompressed
        BOOST_CHECK(data_ptr != 0);
        BOOST_CHECK(iLengthOfTotalData != 0);
        BOOST_CHECK(pCompressedData != 0);
        frame.putBinaryInPayload(pCompressedData,sizeofCompressedData); // Put DED structure in dataframe payload

        /// start thread handling response from webserver
        ws->start(&handle_server_response);

        /// put dataframe in outgoing buffer -- first test with txbuf, then with ringbuffer
        ws->insertDataframeIn_txbuf(frame);

        ws->close(); // send close dataframe, stops internal threads and socket handling, when the stop dataframe is being handled
        ws->join(); // wait until internal thread stops
        delete ws;
    }

    server.stop();// signal stop to server
    /// wait some time for server to stop
    boost::this_thread::sleep(workTime);

    BOOST_CHECK(bInstantiated == true);
}

BOOST_AUTO_TEST_SUITE_END( )
