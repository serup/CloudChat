#define BOOST_TEST_MODULE compressiontest
#include <boost/test/included/unit_test.hpp>
#include "../compression.h"
#include <string>

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
//
// http://www.commandlinefu.com/commands/view/3902/remove-cr-lf-from-a-text-file
// how to remove unwanted CRLF and replacing with LF
// sed -i 's/\r\n/\n/' compressionTest.cpp
//

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

//////////////////////////////////////////
// TESTCASE compressiontest
// howto compile : g++ -g -o compressiontest compressiontest.cpp ../compression.cpp -lboost_system -std=c++11
// howto compile on windows cygwin : g++ -g -o compressiontest compressiontest.cpp ../compression.cpp -std=gnu++0x
//////////////////////////////////////////
// http://www.drdobbs.com/cpp/c11-uniqueptr/240002708
// http://gcc.gnu.org/wiki/C11Status
// http://gcc.gnu.org/gcc-4.8/cxx0x_status.html
// http://boost.teeks99.com/
// http://boost-win.tumblr.com/
// http://wiki.codeblocks.org/index.php?title=BoostWindowsQuickRef
//////////////////////////////////////////
BOOST_AUTO_TEST_SUITE (compressiontest) // name of the test suite is dataencodertest


BOOST_AUTO_TEST_CASE (InstantiateCompressionClass)
{
    HandleCompression::kext_tools59 *obj = new HandleCompression::kext_tools59();
    BOOST_CHECK(obj != 0);
}

typedef unsigned char byte;
#define DED_COMPRESS_DATA(data_ptr_c,sizeofdata,compresseddata,sizeofcompresseddata) \
    std::unique_ptr<byte> m_ptotalCompresseddata;\
	m_ptotalCompresseddata.reset(new unsigned char[sizeofdata]);\
	int sizeofcompresseddata;\
	{kext_tools59 Okumura;\
	sizeofcompresseddata = Okumura.compress_lzss(m_ptotalCompresseddata.get(),sizeofdata,(unsigned char*)data_ptr_c,sizeofdata);}byte* compresseddata = m_ptotalCompresseddata.get();

#define	DED_DECOMPRESS_DATA(pCompressedData,sizeofCompressedData,pUnCompressedData,sizeofUncompressedData) \
	std::unique_ptr<byte> m_pUnCmpressed;\
	int iEstimatedLength=sizeofCompressedData * 8;\
	kext_tools59 Okumura;\
	m_pUnCmpressed.reset(new unsigned char[iEstimatedLength]);\
	ZeroMemory(m_pUnCmpressed.get(),iEstimatedLength);\
	int sizeofUncompressedData = Okumura.decompress_lzss(m_pUnCmpressed.get(),pCompressedData,sizeofCompressedData);\
	byte* pUnCompressedData = m_pUnCmpressed.get();

BOOST_AUTO_TEST_CASE (insert_node)
{
    char data_ptr_c[] = {1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,8,8};
    std::unique_ptr<byte> m_ptotalCompresseddata;
    int sizeofdata=sizeof(data_ptr_c);
    m_ptotalCompresseddata.reset(new unsigned char[sizeofdata]);
    int sizeofcompresseddata=0;
    HandleCompression::kext_tools59 Okumura;

/// parameters
    __u_int8_t *dst = m_ptotalCompresseddata.get();
    __u_int32_t dstlen = sizeofdata;
    __u_int8_t *src = (unsigned char*)data_ptr_c;
    __u_int32_t srcLen = sizeofdata;
///

#define cN         4096  /* size of ring buffer - must be power of 2 */
#define cF         18    /* upper limit for match_length */
#define THRESHOLD 2     /* encode string into position and length
                           if match_length is greater than this */
#define NIL       cN     /* index for root of binary search trees */

    int size=0;

    /* Encoding state, mostly tree but some current match stuff */
    struct encode_state fsp;
    struct encode_state *sp;

    int c, r, s, last_match_length, code_buf_ptr;
    register int i;
    register int len;
    __u_int8_t code_buf[17], mask;
    __u_int8_t *srcend = src + srcLen;
    __u_int8_t *dstend = dst + dstlen;
    //u_int8_t *begindst = dst;

    /* initialize trees */
    sp = (struct encode_state *)&fsp;
    Okumura.init_state(sp);

    /*
     * code_buf[1..16] saves eight units of code, and code_buf[0] works
     * as eight flags, "1" representing that the unit is an unencoded
     * letter (1 byte), "0" a position-and-length pair (2 bytes).
     * Thus, eight units require at most 16 bytes of code.
     */
    code_buf[0] = 0;
    code_buf_ptr = mask = 1;

    /* Clear the buffer with any character that will appear often. */
    s = 0;  r = cN - cF;

    /* Read F bytes into the last F bytes of the buffer */
    for (len = 0; len < cF && src < srcend; len++)
        sp->text_buf[r + len] = *src++;
//    if (!len) {
//        return 0;  /* text of size zero */
//    }
    /*
     * Insert the F strings, each of which begins with one or more
     * 'space' characters.  Note the order in which these strings are
     * inserted.  This way, degenerate trees will be less likely to occur.
     */
    for (i = 1; i <= cF; i++)
        Okumura.insert_node(sp, r - i);

    BOOST_CHECK(len > 0);

    //sizeofcompresseddata = Okumura.compress_lzss(m_ptotalCompresseddata.get(),sizeofdata,(unsigned char*)data_ptr_c,sizeofdata);
    //BOOST_CHECK(sizeofcompresseddata < sizeofdata);
}

BOOST_AUTO_TEST_CASE (compressData1)
{
    char data_ptr_c[] = {1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,8,8};
    std::unique_ptr<byte> m_ptotalCompresseddata;
    int sizeofdata=sizeof(data_ptr_c);
    m_ptotalCompresseddata.reset(new unsigned char[sizeofdata]);
    int sizeofcompresseddata=0;
    HandleCompression::kext_tools59 Okumura;

    sizeofcompresseddata = Okumura.compress_lzss(m_ptotalCompresseddata.get(),sizeofdata,(unsigned char*)data_ptr_c,sizeofdata);
    BOOST_CHECK(sizeofcompresseddata < sizeofdata);
}

BOOST_AUTO_TEST_CASE (compressData2)
{
    char data_ptr_c[] = {1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,8,8};
    int sizeofdata=sizeof(data_ptr_c);

    DED_COMPRESS_DATA(data_ptr_c,sizeofdata,compresseddata,sizeofcompresseddata)
    BOOST_CHECK(sizeofcompresseddata < sizeofdata);
    BOOST_CHECK(compresseddata != 0);
}

BOOST_AUTO_TEST_CASE (compressAndUncompress)
{
    char data_ptr_c[] = {1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,8,8};
    int sizeofdata=sizeof(data_ptr_c);

    DED_COMPRESS_DATA(data_ptr_c,sizeofdata,compresseddata,sizeofcompresseddata)
    BOOST_CHECK(sizeofcompresseddata < sizeofdata);

	std::unique_ptr<byte> m_pUnCmpressed;
	int iEstimatedLength=sizeofcompresseddata * 8;
	kext_tools59 Okumura;
	m_pUnCmpressed.reset(new unsigned char[iEstimatedLength]);
	ZeroMemory(m_pUnCmpressed.get(),iEstimatedLength);
	int sizeofUncompressedData = Okumura.decompress_lzss(m_pUnCmpressed.get(),compresseddata,sizeofcompresseddata);

    BOOST_CHECK(sizeofUncompressedData == sizeofdata);
    BOOST_CHECK(data_ptr_c[99] == 8);
}


BOOST_AUTO_TEST_CASE (chckDataAfterCompressUncompress)
{
    char data_ptr_c[] = {1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,8,8};
    int sizeofdata=sizeof(data_ptr_c);

    DED_COMPRESS_DATA(data_ptr_c,sizeofdata,compresseddata,sizeofcompresseddata)
    BOOST_CHECK(sizeofcompresseddata < sizeofdata);

    DED_DECOMPRESS_DATA(compresseddata,sizeofcompresseddata,pUnCompressedData,sizeofUncompressedData);
    BOOST_CHECK(sizeofUncompressedData == sizeofdata);
    for(int n=0;n<sizeofdata;n++)
        BOOST_CHECK(pUnCompressedData[n] == data_ptr_c[n]);
}

BOOST_AUTO_TEST_CASE (compressAndUncompress2)
{
    std::string str("I am Sam\nSam I am\n\nThat Sam-I-am!\nThat Sam-I-am!\nI do not like\nthat Sam-I-am!\n\nDo you like green eggs and ham?\n\nI do not like them, Sam-I-am.\nI do not like green eggs and ham.");
    const std::vector<char> data_ptr_c(str.begin(), str.end());
    int sizeofdata=str.size();

    DED_COMPRESS_DATA((unsigned char*)&data_ptr_c[0],sizeofdata,compresseddata,sizeofcompresseddata)
    BOOST_CHECK(sizeofcompresseddata < sizeofdata);

	std::unique_ptr<byte> m_pUnCmpressed;
	int iEstimatedLength=sizeofcompresseddata * 8;
	kext_tools59 Okumura;
	m_pUnCmpressed.reset(new unsigned char[iEstimatedLength]);
	ZeroMemory(m_pUnCmpressed.get(),iEstimatedLength);
	int sizeofUncompressedData = Okumura.decompress_lzss(m_pUnCmpressed.get(),compresseddata,sizeofcompresseddata);

    BOOST_CHECK(sizeofUncompressedData == sizeofdata);
}

BOOST_AUTO_TEST_CASE (chckForUgglyWS_hack)
{
    //std::string str("I am Sam\nSam I am\n\nThat Sam-I-am!\nThat Sam-I-am!\nI do not like\nthat Sam-I-am!\n\nDo you like green eggs and ham?\n\nI do not like them, Sam-I-am.\nI do not like green eggs and ham.");
    std::string str("\x19\x19\x19\x19       I am Sam\nSad I am\n\nThat Sam-I-am!\n I \nThat Sam-I-am!\nI do not like\nthat Sam-I-am!\n\nDo you like green eggs and ham?\n\nI do not like them, Sam-I-am.\nI do not like green eggs and ham.");
    std::vector<unsigned char> data_ptr_c_before(str.begin(), str.end());
    std::vector<unsigned char> data_ptr_c(str.begin(), str.end());
    int sizeofdata=str.size();

	cout << "Found non-alphas in data :" << endl;
	for(int n=0;n<data_ptr_c.size();n++)
	{
		if(!isalpha(data_ptr_c[n]))
			fprintf(stdout, "%02X%s", data_ptr_c[n], ( n + 1 ) % 16 == 0 ? "\r\n" : " " );  

	}
	cout << endl;

	/**
	 * HACK - should be done in CDataEncoder::Encodestdvector
	 */
	//increment all to avoid whitespace issue - UGGLY Hack
	for(int n=0;n<data_ptr_c.size();n++)
	{
		data_ptr_c[n] = data_ptr_c[n] + 1;

	}
		

    DED_COMPRESS_DATA((unsigned char*)&data_ptr_c[0],sizeofdata,compresseddata,sizeofcompresseddata)
    BOOST_CHECK(sizeofcompresseddata < sizeofdata);

    DED_DECOMPRESS_DATA(compresseddata,sizeofcompresseddata,pUnCompressedData,sizeofUncompressedData);
    BOOST_CHECK(sizeofUncompressedData == sizeofdata);

	/**
	 * HACK - should be done in CDataEncoder::GetElement - under DED_ELEMENT_TYPE_STDVECTOR
	 */
	//decrement all to recreate whitespace - UGGLY Hack
	for(int n=0;n<sizeofUncompressedData;n++)
	{
		pUnCompressedData[n] = pUnCompressedData[n] - 1;
	}

	bool bError=false;
	cout << "Data : " << endl;
	cout << "/*{{{*/" << endl;  
    for(int n=0;n<sizeofdata;n++) {
        if(pUnCompressedData[n] != data_ptr_c_before[n]) {
			fprintf(stdout, "FAIL:[%02X!=%02X] ", pUnCompressedData[n], data_ptr_c_before[n] );  
			bError=true;
		}
		else
			fprintf(stdout, "%02X%s", pUnCompressedData[n], ( n + 1 ) % 16 == 0 ? "\r\n" : " " );  
	}
	cout << "/*}}}*/" << endl;  
	BOOST_CHECK(!bError);
}

BOOST_AUTO_TEST_CASE (chckDataAfterSmallCompressUncompress)
{
    std::string str("aaaaaaaaaa");
    const std::vector<char> data_ptr_c(str.begin(), str.end());
    int sizeofdata=str.size();


    DED_COMPRESS_DATA((unsigned char*)&data_ptr_c[0],sizeofdata,compresseddata,sizeofcompresseddata)
    BOOST_CHECK(sizeofcompresseddata < sizeofdata);

    DED_DECOMPRESS_DATA(compresseddata,sizeofcompresseddata,pUnCompressedData,sizeofUncompressedData);
    BOOST_CHECK(sizeofUncompressedData == sizeofdata);
    for(int n=0;n<sizeofdata;n++)
        BOOST_CHECK(pUnCompressedData[n] == data_ptr_c[n]);
}

BOOST_AUTO_TEST_CASE (SmallCompressAndUncompress)
{
//    std::string str("aaaaaaaaaa");
//    std::string str("aaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbaaaaaaaabbbbbbbaaabbbbbbbbbbaaaacccccccccccccccccddddddccccddddeeeee");
    std::string str("aabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeff");
    const std::vector<char> data_ptr_c(str.begin(), str.end());
    int sizeofdata=str.size();

    DED_COMPRESS_DATA((unsigned char*)&data_ptr_c[0],sizeofdata,compresseddata,sizeofcompresseddata)
    BOOST_CHECK(sizeofcompresseddata < sizeofdata);

	std::unique_ptr<byte> m_pUnCmpressed;
	int iEstimatedLength=sizeofcompresseddata * 8;
	kext_tools59 Okumura;
	m_pUnCmpressed.reset(new unsigned char[iEstimatedLength]);
	ZeroMemory(m_pUnCmpressed.get(),iEstimatedLength);
	int sizeofUncompressedData = Okumura.decompress_lzss(m_pUnCmpressed.get(),compresseddata,sizeofcompresseddata);

    BOOST_CHECK(sizeofUncompressedData == sizeofdata);
}

BOOST_AUTO_TEST_SUITE_END( )
