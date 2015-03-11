#define BOOST_TEST_MODULE requesthandlertest
#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
#include <boost/algorithm/string.hpp>
#include "../../request_handler.hpp"
#ifndef __MINGW32__
#include <arpa/inet.h>
#endif
using namespace std;
using namespace boost::unit_test;

//////////////////////////////////////////
// TESTCASE
//////////////////////////////////////////
// howto compile :
// use makefile_cygwin.mak from inside cygwin terminal :
// use makefile.mak from linux prompt
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

BOOST_AUTO_TEST_SUITE (requesthandlertest) // name of the test suite is requesthandlertest

BOOST_AUTO_TEST_CASE(boostchecktest)
{
  BOOST_CHECK(3087 != 3086);
}

BOOST_AUTO_TEST_CASE(getNumberFromString)
{
  websocket::http::request_handler s;
  BOOST_CHECK(s.get_number_from_string((const char*)"1 2 3 4 4") == 3086); // find and remove 4 spaces and divide resulting number (12344) with amount of spaces (4)
}


BOOST_AUTO_TEST_CASE (basicMD5test)
{
  std::string str(websocket::http::CMD5((const char*)"helloworld").GetMD5s()); //helloworld should yield = fc5e038d38a57032085441e7fe7010b0
  BOOST_CHECK(str == "FC5E038D38A57032085441E7FE7010B0"); // hex characters are capital size, when running on linux ubuntu
}

BOOST_AUTO_TEST_CASE (md5challengetest)
{
  websocket::http::request_handler s;

  std::string challenge;
  std::string key1;
  std::string key2;
  int key_1;
  int key_2;

  challenge = "G0\"-Z?GX";
  key1 = "P388 O503D&ul7 {K%gX( %7  15";
  key2 = "1 N ?|k UT0or 3o  4 I97N 5-S3O 31";

  key_1 = s.get_number_from_string(key1.c_str()); // raw number / num of spaces ( remove spaces to create raw number, then divide with the amount of spaces found )
  key_2 = s.get_number_from_string(key2.c_str());

  int key_1_big_endian = htonl(key_1); // ntohl  is networktohostlong function - it converts a long from bigEndian to littleEndian format
  int key_2_big_endian = htonl(key_2); // found for linux in netinet/in.h and on windows in winsock2.h, it is from hoffman encoding library

  BOOST_CHECK(key_1 == 777007543);
  BOOST_CHECK(key_2 == 114997259);

  BOOST_CHECK(key_1_big_endian == -1221504978);
  BOOST_CHECK(key_2_big_endian == 196663814);

  BOOST_CHECK(s.md5_hash_challenge(key_1, key_2, (u_char*)challenge.c_str()) == "0st3Rl&q-2ZU^weu"); // should yield 0st3Rl&q-2ZU^weu
}

BOOST_AUTO_TEST_CASE (md5challengetest2)
{
  websocket::http::request_handler s;

  std::string challenge;
  std::string key1;
  std::string key2;
  int key_1;
  int key_2;

  challenge = "^n:ds[4U";
  key1 = "4 @1  46546xW%0l 1 5";
  key2 = "12998 5 Y3 1  .P00";

  key_1 = s.get_number_from_string(key1.c_str()); // raw number / num of spaces ( remove spaces to create raw number, then divide with the amount of spaces found )
  key_2 = s.get_number_from_string(key2.c_str());

  std::string strchal = s.md5_hash_challenge(key_1, key_2, (u_char*)challenge.c_str());
  BOOST_CHECK(strchal == (std::string)"8jKS'y:G*Co,Wxa-"); // should yield 8jKS'y:G*Co,Wxa-
}

BOOST_AUTO_TEST_CASE (md5challengetest3)
{
  websocket::http::request_handler s;

  std::string challenge;
  std::string key1;
  std::string key2;
  int key_1;
  int key_2;

  //challenge = ".lWj;A:@";
					challenge.push_back(0xf0);
					challenge.push_back(0x6c);
					challenge.push_back(0x57);
					challenge.push_back(0x6a);
					challenge.push_back(0x3b);
					challenge.push_back(0x41);
					challenge.push_back(0x3a);
					challenge.push_back(0x40);


  key1 = ";  100 +  606x[ 6720   ( /";
  key2 = "q166 010q 9 6 9 0";

  key_1 = s.get_number_from_string(key1.c_str()); // raw number / num of spaces ( remove spaces to create raw number, then divide with the amount of spaces found )
  key_2 = s.get_number_from_string(key2.c_str());

  std::string strchal = s.md5_hash_challenge(key_1, key_2, (u_char*)challenge.c_str());
  BOOST_CHECK( strchal == (std::string)"7.I..fQM..j..m.."); // test values not verified
}

BOOST_AUTO_TEST_CASE (md5challengetest4)
{
  websocket::http::request_handler s;

  std::string challenge;
  std::string key1;
  std::string key2;
  int key_1;
  int key_2;

  key1 = "18x 6]8vM;54 *(5:  {   U1]8  z [  8";
  key2 = "1_ tx7X d  <  nw  334J702) 7]o}` 0";
  challenge = "Tm[K T2u"; // last 8 bytes in handshake

  key_1 = s.get_number_from_string(key1.c_str()); // raw number / num of spaces ( remove spaces to create raw number, then divide with the amount of spaces found )
  key_2 = s.get_number_from_string(key2.c_str());

  std::string strchal = s.md5_hash_challenge(key_1, key_2, (u_char*)challenge.c_str());
  BOOST_CHECK( strchal == (std::string)"fQJ,fN/4F4!~K~MH"); //    After the fields, the server sends the aforementioned MD5 sum,
  // a 16 byte (128 bit) value, shown here as if interpreted as ASCII:   fQJ,fN/4F4!~K~MH
}


BOOST_AUTO_TEST_SUITE_END( )
