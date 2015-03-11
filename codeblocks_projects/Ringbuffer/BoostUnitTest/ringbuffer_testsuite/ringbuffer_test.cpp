#define BOOST_TEST_MODULE ringbuffertest
#include <boost/test/included/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>      // std::ifstream
#include "../../ringbuffer.hpp"
#include "../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h"
#include "../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h"
using namespace std;
using namespace boost::unit_test;

//////////////////////////////////////////
// TESTCASE
//////////////////////////////////////////
// howto compile :
// use makefile_cygwin.mak from inside cygwin terminal :
// use makefile.mak from linux prompt
// command line in linux:
// g++ -g -o bin/Debug/ringbuffer_test ringbuffer_test.cpp ../../ringbuffer.hpp  -L"/usr/local/lib/"  -D__MSABI_LONG=long -lboost_system -lboost_signals -lboost_thread -lpthread -lrt  -std=gnu++11
//
// ex.
// make -f makefile_cygwin.mak
// make -f makefile.mak
// NB! makefile will not only build and link, it will also run the testcase
//     and convert the generated result output into the file test_result.html
//////////////////////////////////////////
// how to test:
// make -f makefile_cygwin.mak test
// This will convert test_results.xml file to test_results.html, based on test_results.xslt file
//////////////////////////////////////////

#define BOOST_AUTO_TEST_MAIN
#ifndef NOTESTRESULTFILE
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
#endif
#endif
BOOST_AUTO_TEST_SUITE (ringbuffertest) // name of the test suite

/**
	 * C++ version 0.4 std::string style "itoa":
	 * Contributions from Stuart Lowe, Ray-Yuan Sheu,
	 * Rodrigo de Salvo Braz, Luc Gallant, John Maloney
	 * and Brian Hunt
	 *
	 * or use : http://stackoverflow.com/questions/4668760/converting-an-int-to-stdstring
	 * boost::lexical_cast<std::string>(yourint) from boost/lexical_cast.hpp
	 */
std::string itoa(int value, int base=10)
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




const unsigned long QUEUE_SIZE     = 1000L;

BOOST_AUTO_TEST_CASE(ringbuffer_instantiated)
{
    //RingBuffer<unsigned char> testRingbuffer(QUEUE_SIZE);
    RingBuffer<unsigned char> *ptestRingbuffer = new RingBuffer<unsigned char>(QUEUE_SIZE);

    BOOST_CHECK(ptestRingbuffer != 0);

    delete ptestRingbuffer;
}

BOOST_AUTO_TEST_CASE(WriteBinaryIntoRingBuffer_which_is_too_small)
{
    //RingBuffer<unsigned char> testRingbuffer(QUEUE_SIZE);
    RingBuffer<unsigned char> *ptestRingbuffer = new RingBuffer<unsigned char>(QUEUE_SIZE);

    BOOST_CHECK(ptestRingbuffer != 0);

    int iBufLen = 2000;
    unsigned char Buffer[iBufLen];
    unsigned char *pBuf = Buffer;

    // Fill buffer with FF
    for(int n=0;n<iBufLen;n++)
    {
        Buffer[n] = (unsigned char)0xff;
    }

    // Put Buffer into RingBuffer
    int iBytesTransfered = ptestRingbuffer->WriteBinaryIntoRingBuffer(pBuf, iBufLen);

    BOOST_CHECK(iBytesTransfered < iBufLen);

    delete ptestRingbuffer;
}

BOOST_AUTO_TEST_CASE(WriteBinaryIntoRingBuffer_which_has_room_enough)
{
    //RingBuffer<unsigned char> testRingbuffer(QUEUE_SIZE);
    RingBuffer<unsigned char> *ptestRingbuffer = new RingBuffer<unsigned char>(QUEUE_SIZE);

    BOOST_CHECK(ptestRingbuffer != 0);

    int iBufLen = QUEUE_SIZE-1;
    unsigned char Buffer[iBufLen];
    unsigned char *pBuf = Buffer;

    // Fill buffer with FF
    for(int n=0;n<iBufLen;n++)
    {
        Buffer[n] = (unsigned char)0xff;
    }

    // Put Buffer into RingBuffer
    int iBytesTransfered = ptestRingbuffer->WriteBinaryIntoRingBuffer(pBuf, iBufLen);

    BOOST_CHECK(iBytesTransfered == iBufLen);

    delete ptestRingbuffer;
}

BOOST_AUTO_TEST_CASE(ReadBinaryFromRingBuffer)
{
    RingBuffer<unsigned char> *ptestRingbuffer = new RingBuffer<unsigned char>(QUEUE_SIZE);

    BOOST_CHECK(ptestRingbuffer != 0);

    int iBufLen = QUEUE_SIZE-1;
    unsigned char Buffer[iBufLen];
    unsigned char *pBuf = Buffer;

    // Fill buffer with FF
    for(int n=0;n<iBufLen;n++)
    {
        Buffer[n] = (unsigned char)0xff;
    }

    // Put Buffer into RingBuffer
    int iBytesTransfered = ptestRingbuffer->WriteBinaryIntoRingBuffer(pBuf, iBufLen);

    BOOST_CHECK(iBytesTransfered == iBufLen);


    // Read from ringbuffer
    unsigned char ReadBuffer[iBufLen];
    unsigned char *pReadBuf = ReadBuffer;

    int iBytesRead = ptestRingbuffer->ReadBinaryFromRingBuffer(pReadBuf,iBufLen);
    BOOST_CHECK(iBytesRead == iBufLen);

    delete ptestRingbuffer;
}

BOOST_AUTO_TEST_CASE(WriteDataframeToRingbuffer)
{
    RingBuffer<unsigned char> *ptestRingbuffer = new RingBuffer<unsigned char>(QUEUE_SIZE);
    ProduceToRingbuffer<RingBuffer<unsigned char>> producer(ptestRingbuffer);
    //ProduceToRingbuffer<RingBuffer<unsigned char>> *pProducer = new ProduceToRingbuffer<RingBuffer<unsigned char>>(ptestRingbuffer);

    BOOST_CHECK(ptestRingbuffer != 0);

    // Create dataframe
    dataframe frm;
    std::string header = "HELLO WORLD"; // test message
    std::copy(header.begin(), header.end(), std::back_inserter(frm.payload));
    //std::copy(msg.payload.begin(), msg.payload.end(), std::back_inserter(frm.payload));

    // Put dataframe into RingBuffer
    bool bResult = producer.StoreDataframe(frm);
    //pProducer->StoreDataframe(pData,iDataLen);

    BOOST_CHECK(bResult == true);

    delete ptestRingbuffer;
}


void handle_message(void *pParent)
{
    CDED* pDED = (CDED*)pParent;
    ConsumeFromRingbuffer<RingBuffer<unsigned char>>* pOwner = (ConsumeFromRingbuffer<RingBuffer<unsigned char>>*) pDED->pOwner;

    pDED->status = "";
    pDED->status = (std::string)(char*)pDED->pDEDarray;
}

BOOST_AUTO_TEST_CASE(ReadDataframeFromRingbuffer)
{
    RingBuffer<unsigned char> testRingbuffer(QUEUE_SIZE);
    ProduceToRingbuffer<RingBuffer<unsigned char>> producer(&testRingbuffer);
    ConsumeFromRingbuffer<RingBuffer<unsigned char>> consumer(&testRingbuffer,(void (*)(void * ))&handle_message);

    // Create dataframe
    dataframe frm;
    std::string header = "HELLO WORLD"; // test message
    std::copy(header.begin(), header.end(), std::back_inserter(frm.payload));

    // Put dataframe into RingBuffer
    bool bResult = producer.StoreDataframe(frm);

    BOOST_CHECK(bResult == true);

    // Read dataframe from Ringbuffer
    CDED ded;
    consumer.SetResultDED(ded);

    boost::thread consume(consumer);
    consume.join();

    BOOST_CHECK(ded.status == (std::string)header);
}

void handle_DED(void *pParent)
{
    CDED* pDED = (CDED*)pParent;
    ConsumeFromRingbuffer<RingBuffer<unsigned char>>* pOwner = (ConsumeFromRingbuffer<RingBuffer<unsigned char>>*) pDED->pOwner;

    pDED->status = "DED not yet inside";

    DED_PUT_DATA_IN_DECODER(decoder_ptr,(unsigned char*)pDED->pDEDarray,pDED->sizeofDED);
    BOOST_CHECK(decoder_ptr != 0);

    bool bDecoded=false;
    std::string strValue;
    unsigned short iValue;
    bool bValue;

    // decode data ...
    if( DED_GET_STRUCT_START( decoder_ptr, "event" ) &&
         DED_GET_METHOD	( decoder_ptr, "name", strValue ) &&
         DED_GET_USHORT	( decoder_ptr, "trans_id", iValue) &&
         DED_GET_BOOL	( decoder_ptr, "startstop", bValue ) &&
        DED_GET_STRUCT_END( decoder_ptr, "event" ))
    {
        bDecoded=true;
        pDED->status = "DED inside";
    }
    else
    {
        bDecoded=false;
    }


/*	// decode data ...
  	if( DED_GET_STRUCT_START( decoder_ptr, (std::string)"event" ) == true )
	{
		if(DED_GET_METHOD( decoder_ptr, "name", strValue ) == true)
		{
			if(DED_GET_USHORT( decoder_ptr, "trans_id", iValue) == true)
			{
				if(DED_GET_BOOL( decoder_ptr, "startstop", bValue ) == true)
		        {
					if(DED_GET_STRUCT_END( decoder_ptr, "event" ) == true)
                    {
                        bDecoded=true;
                        pDED->status = "DED inside";
                    }
                    else
                    {
                        bDecoded=false;
					}
		       	}
		    }
	    }
	}
*/

  	BOOST_CHECK(bDecoded == true);
	BOOST_CHECK(strValue == "MusicPlayer");
	BOOST_CHECK(iValue == 24);
	BOOST_CHECK(bValue == true);
}

BOOST_AUTO_TEST_CASE(ReadDEDFromDataframeFromRingbuffer)
{
    RingBuffer<unsigned char> testRingbuffer(QUEUE_SIZE);
    ProduceToRingbuffer<RingBuffer<unsigned char>> producer(&testRingbuffer);
    ConsumeFromRingbuffer<RingBuffer<unsigned char>> consumer(&testRingbuffer,(void (*)(void * ))&handle_DED);

    // Create dataframe
    dataframe frm;

    // Create DED structure
    unsigned short trans_id = 24;
    bool action = true;

    DED_START_ENCODER(encoder_ptr);
    DED_PUT_STRUCT_START( encoder_ptr, "event" );
    DED_PUT_METHOD	( encoder_ptr, "name",  "MusicPlayer" );
    DED_PUT_USHORT	( encoder_ptr, "trans_id",	trans_id);
    DED_PUT_BOOL	( encoder_ptr, "startstop", action );
    DED_PUT_STRUCT_END( encoder_ptr, "event" );
    BOOST_CHECK(encoder_ptr != 0);

    DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);
    if(sizeofCompressedData==0) // if sizeofcompresseddata is 0 then compression was not possible and size is the same as for uncompressed
        sizeofCompressedData = iLengthOfTotalData;

    BOOST_CHECK(data_ptr != 0);
    BOOST_CHECK(iLengthOfTotalData != 0);
    BOOST_CHECK(pCompressedData != 0);

    // Put DED structure in dataframe payload
    //std::vector<char> vec(pCompressedData, pCompressedData+sizeofCompressedData);
    //std::copy(vec.begin(), vec.end(), std::back_inserter(frm.payload));
    frm.putBinaryInPayload(pCompressedData,sizeofCompressedData);

    // Put dataframe into RingBuffer
    bool bResult = producer.StoreDataframe(frm);

    BOOST_CHECK(bResult == true);

    // Read dataframe from Ringbuffer
    CDED ded;
    consumer.SetResultDED(ded);

    boost::thread consume(consumer); // Will call "handle_DED" for each dataframe read from ringbuffer, in this test there should only be one
    consume.join(); // will wait until no more dataframes in ringbuffer

    BOOST_CHECK(ded.status == (std::string)"DED inside");

}

void handle_multiple_DED(void *pParent)
{
    CDED* pDED = (CDED*)pParent;
    ConsumeFromRingbuffer<RingBuffer<unsigned char>>* pOwner = (ConsumeFromRingbuffer<RingBuffer<unsigned char>>*) pDED->pOwner;

    pDED->status = "DED not yet inside";

    DED_PUT_DATA_IN_DECODER(decoder_ptr,(unsigned char*)pDED->pDEDarray,pDED->sizeofDED);
    BOOST_CHECK(decoder_ptr != 0);

    bool bDecoded=false;
    std::string strValue;
    unsigned short iValue;
    bool bValue;
    static int DEDcount = 1;

    // decode data ...
    if( DED_GET_STRUCT_START( decoder_ptr, "event" ) &&
         DED_GET_METHOD	( decoder_ptr, "name", strValue ) &&
         DED_GET_USHORT	( decoder_ptr, "trans_id", iValue) &&
         DED_GET_BOOL	( decoder_ptr, "startstop", bValue ) &&
        DED_GET_STRUCT_END( decoder_ptr, "event" ))
    {
        bDecoded=true;
        std::string tmp("DED inside:");
        pDED->status = tmp+itoa(DEDcount);
        DEDcount++;
    }
    else
    {
        bDecoded=false;
    }

  	BOOST_CHECK(bDecoded == true);
	BOOST_CHECK(strValue == "MusicPlayer");
	BOOST_CHECK(iValue == 24);
	BOOST_CHECK(bValue == true);
}

BOOST_AUTO_TEST_CASE(ReadMultipleDEDFromDataframeFromRingbuffer)
{
    RingBuffer<unsigned char> testRingbuffer(QUEUE_SIZE);
    ProduceToRingbuffer<RingBuffer<unsigned char>> producer(&testRingbuffer);
    ConsumeFromRingbuffer<RingBuffer<unsigned char>> consumer(&testRingbuffer,(void (*)(void * ))&handle_multiple_DED);

    // Create dataframe
    dataframe frm;

    // Create DED structure
    unsigned short trans_id = 24;
    bool action = true;

    DED_START_ENCODER(encoder_ptr);
    DED_PUT_STRUCT_START( encoder_ptr, "event" );
    DED_PUT_METHOD	( encoder_ptr, "name",  "MusicPlayer" );
    DED_PUT_USHORT	( encoder_ptr, "trans_id",	trans_id);
    DED_PUT_BOOL	( encoder_ptr, "startstop", action );
    DED_PUT_STRUCT_END( encoder_ptr, "event" );
    BOOST_CHECK(encoder_ptr != 0);

    DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);
    if(sizeofCompressedData==0) // if sizeofcompresseddata is 0 then compression was not possible and size is the same as for uncompressed
        sizeofCompressedData = iLengthOfTotalData;

    BOOST_CHECK(data_ptr != 0);
    BOOST_CHECK(iLengthOfTotalData != 0);
    BOOST_CHECK(pCompressedData != 0);

    // Put DED structure in dataframe payload
    frm.putBinaryInPayload(pCompressedData,sizeofCompressedData);

    // Put multiple dataframes into RingBuffer
    bool bResult = producer.StoreDataframe(frm);
    BOOST_CHECK(bResult == true);
    bResult = producer.StoreDataframe(frm);
    BOOST_CHECK(bResult == true);
    bResult = producer.StoreDataframe(frm);
    BOOST_CHECK(bResult == true);

    // Read dataframes from Ringbuffer - using consumer thread
    CDED ded;
    consumer.SetResultDED(ded);

    boost::thread consume(consumer); // Will call "handle_DED" for each dataframe read from ringbuffer, in this test there should only be one

    // insert while consume is happening
    bResult = producer.StoreDataframe(frm);
    BOOST_CHECK(bResult == true);

    // wait for consume thread to finish
    consume.join(); // will wait until no more dataframes in ringbuffer

    BOOST_CHECK(ded.status == (std::string)"DED inside:4"); // should have received and parsed 3 identical dataframes
}

BOOST_AUTO_TEST_SUITE_END( )
