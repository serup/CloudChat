#define BOOST_TEST_MODULE DFD1_1_9_ProfileLog_Test
#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <fstream>
//#include "../../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h"
#include "../../1_1_Profile.hpp"
#include "../../md5.h"
#include "../../../../websocket_server/server.hpp"
#include "../../../../websocket_server/dataframe.hpp"
using namespace websocket;
using namespace std;
using namespace boost::unit_test;
using boost::property_tree::ptree;

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
#ifndef NOTESTRESULTFILE
#ifdef BOOST_AUTO_TEST_MAIN
std::ofstream out;

struct ReportRedirector
{
    ReportRedirector()
    {
        out.open("1_1_9_test_results.xml");
        assert( out.is_open() );
        boost::unit_test::results_reporter::set_stream(out);
    }
};

BOOST_GLOBAL_FIXTURE(ReportRedirector)
#endif
#endif


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

static bool bDirWasSet=false;
void SetDir()
{
    if(bDirWasSet==false) {
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::cout << "[DFD1_1_9_ProfileLog_Test] : Current path is : " << full_path << std::endl;
    chdir("../../../../");
    boost::filesystem::path new_full_path( boost::filesystem::current_path() );
    std::cout << "[DFD1_1_9_ProfileLog_Test] : Current path is : " << new_full_path << std::endl;
    bDirWasSet = true;
    }
}

std::string GetTime()
{
    std::string strTime="";
    using namespace boost::gregorian;
    using namespace boost::posix_time;
    using namespace std;

    stringstream ss;

    /****** format strings ******/
    time_facet* output_facet = new time_facet();
    time_input_facet* input_facet = new time_input_facet();
    ss.imbue(locale(ss.getloc(), output_facet));
    ss.imbue(locale(ss.getloc(), input_facet));
    output_facet->format(time_facet::iso_time_format_extended_specifier);
    input_facet->format(time_input_facet::iso_time_format_extended_specifier);
    /***** fetch current date ****/
    ptime t(second_clock::local_time());
    ss << t;
    ///cout << ss.str() << endl; // ex. 2012-08-03 23:46:38
    strTime = ss.str();

    //delete output_facet;
    //delete input_facet;

    return strTime;
}

std::string GetYearMonthDay()
{
    std::string strTime="";
    using namespace boost::gregorian;
    using namespace boost::posix_time;
    using namespace std;

    stringstream ss;

    /****** format strings ******/
    time_facet* output_facet = new time_facet();
    time_input_facet* input_facet = new time_input_facet();
    ss.imbue(locale(ss.getloc(), output_facet));
    ss.imbue(locale(ss.getloc(), input_facet));
    output_facet->format(time_facet::iso_time_format_extended_specifier);
    input_facet->format(time_input_facet::iso_time_format_extended_specifier);
    /***** fetch current date ****/
    ptime t(second_clock::local_time());

    ss << t.date().year();
    if(t.date().month() < 10)
        ss << (std::string)"0";
    int month = t.date().month();
    ss << month; /// avoid month being translated into name of month fx. 04 = Apr
    if(t.date().day() < 10)
        ss << (std::string)"0";
    ss << t.date().day();

    strTime = ss.str();

    return strTime;
}

void CreateTestProfileFile(std::string EntityFileName)
{
    /// create a test profile
    //+
    std::vector<Elements> DEDElements;
    std::string EntityName = "Profile";
//        std::string EntityFileName = "ProfileTest";

    CDatabaseControl CDbCtrl;
    CDbCtrl.readDDEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity
    CDbCtrl.readDDTOASTEntityRealm((std::string)EntityName, DEDElements); /// Fetch current datadictionary specs for this entity TOAST and add it to list

    /// put values to elements
    std::string strAttr[] = {"profileID",
    "profileName",
    "protocolTypeID",
    "sizeofProfileData",
    "profile_chunk_id",
    //"organizationID",
    "AccountStatus",
    "SubscriptionExpireDate",
    "ProfileStatus",
    "lifecyclestate", /// TOAST area start
    "username",
    "password"};  /// last 3 elements belongs in TOAST area ; WriteEntityFile - should find out by itself and update value inside TOAST file

     std::vector<std::string> strVecAttrValue = {EntityFileName,
                                  "TestName",
                                  "DED1.00.00",
                                  "0",
                                  "DADE8B486BAA30E8AD733671370FEDFD", // file does NOT exists, so method will consider it empty, could be considered an error, however it really is more a warning
                                  //"888",
                                  "1",  //TODO: find a way to handle when types are not string -- so far for test
                                  "20160101",
                                  "2",
                                  "1", // lifecyclestate
                                  "serup",
                                  "EB23445"};

    unsigned int n=0;
    unsigned int nn = strVecAttrValue.size();
    while(DEDElements.size() > nn)
    {
        DEDElements.pop_back(); /// remove elements that are not being updated
    }
    BOOST_FOREACH( Elements f, DEDElements )
    {
        // iterate thru elements and add value to current specs of above given entity
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::copy(strVecAttrValue[n].begin(), strVecAttrValue[n].end(), std::back_inserter(DEDElements[n].ElementData));
        }
        n++;
        if(n>=nn)
            break;
    }
    /// Write values in entity file
    bool bResult = CDbCtrl.put((std::string)EntityName,(std::string)EntityFileName,DEDElements);
    BOOST_CHECK(bResult == true);

    /// Validate that file was written with the correct values
    // Read entity file using general read for database entity files
    std::vector<Elements> wDEDElements;
    bResult = CDbCtrl.ftgt((std::string)EntityName,(std::string)EntityFileName,wDEDElements);
    BOOST_CHECK(bResult == true);

    /// validate data read
    bResult=true;
    n=0;
    /// ftgt should have read all attributes into wDEDElements, now compare with DEDElements, which is elements written
    BOOST_FOREACH( Elements v, wDEDElements )
    {
        // iterate thru elements and compare
        if(v.strElementID != (std::string)DEDElements[n].strElementID) bResult=false;
        if(bResult == false)
        {
            std::cout << "v.strElementID [ " << v.strElementID << (std::string)" ] != DEDElements[n].strElementID [ " << DEDElements[n].strElementID << " ] \n";
        }
        else
            bResult=true;

        if(v.ElementData != DEDElements[n].ElementData) bResult=false;
        if(bResult == false)
        {
            std::cout << "v.ElementDataElementData [ "  << (std::string)" ] != DEDElements[n].ElementData [ "  << " ] \n";
        }
        else
            bResult=true;

        n++;
        if(n>=DEDElements.size())
            break;
    }
    BOOST_CHECK(bResult == true);
    //-
    /// There should now exists a profile to use in this test
}

/// sandbox development of fn119 and fn1191, which when done should be put inside 1_1_Profile
bool fn1191_AddToLog(unsigned short iTransID, ProfileLogMessage profileLogMsg, ProfileLogResponse &response)
{
    bool bResult=false;
    std::vector<Elements> DEDElements;
    response.enumresp = ProfileLogResponse::enumResponse::error;
    CDatabaseControl DbCtrl;

    // TODO - Create DED Elements Vector from datastream

    DbCtrl.readDDEntityRealm("History", DEDElements); /// Fetch current datadictionary specs for this entity
    //DbCtrl.readDDTOASTEntityRealm("History", DEDElements); /// Fetch current datadictionary specs for this entity TOAST and add it to list

    /// Helpful script to cut attributes out of DD files
    /// $cat DD_HISTORY.xml | grep -e '<PhysicalDataElementName>' | sed -e 's/<PhysicalDataElementName>/\"/' | sed -e 's/<\/PhysicalDataElementName>/\",/'
    ///
    /// put values to elements -- NB this code is not taking into account that attributes could be moved around - it assumes that username is first in toast file attributes
    std::string strAttr[] = {"LogEntryCounter",
        "protocolTypeID",
        "profileID",
        "src",
        "dest",
        "srcUsername",
        "destUsername",
        "showdelivery",
        "timeReceived",
        "timeSend",
        "delivered",
        "message",
        "acceptedIndex",
        "history_chunk_id"
    };



    std::vector<std::string> strVecAttrValue = {profileLogMsg.LogEntryCounter,
                                  profileLogMsg.protocolTypeID, //"DED1.00.00"
                                  profileLogMsg.profileID,
                                  profileLogMsg.src,
                                  profileLogMsg.dest,
                                  profileLogMsg.srcUsername,
                                  profileLogMsg.destUsername,
                                  profileLogMsg.showdelivery,
                                  profileLogMsg.timeReceived,
                                  profileLogMsg.timeSend,
                                  profileLogMsg.delivered,
                                  profileLogMsg.message,
                                  profileLogMsg.acceptedIndex,
                                  profileLogMsg.history_chunk_id
                                  };

    unsigned int n=0;
    unsigned int nn = strVecAttrValue.size();
    while(DEDElements.size() > nn)
    {
        DEDElements.pop_back(); /// remove elements that are not being updated
    }

    BOOST_FOREACH( Elements f, DEDElements )
    {
        // iterate thru elements and add value to current specs of above given entity
        if( f.strElementID == (std::string)strAttr[n] )
        {
            std::copy(strVecAttrValue[n].begin(), strVecAttrValue[n].end(), std::back_inserter(DEDElements[n].ElementData));
        }
        n++;
        if(n>=nn)
            break;
    }

    /// Write values in entity file
///    std::string strdate = profileLogMsg.timeReceived;
///    std::string strRecord = strdate + "_HIST_" + profileLogMsg.profileID; /// example : 20140414_HIST_9BDAF4D383214E309D885315C164C6EF.xml (xml will be added later)
    std::string strRecord = GetYearMonthDay() + "_HIST_" + profileLogMsg.profileID; /// example : 20140414_HIST_9BDAF4D383214E309D885315C164C6EF.xml (xml will be added later)
    bResult = DbCtrl.append("History", strRecord, DEDElements); /// append to file !!!


    if(bResult==true)
    {
        /// History was updated, now make a response "Log Saved in database"
        response.enumresp = ProfileLogResponse::enumResponse::WriteLogSuccess;

        DED_START_ENCODER(encoder_ptr);
        DED_PUT_STRUCT_START( encoder_ptr, "DFDResponse" );
        DED_PUT_METHOD	( encoder_ptr, "Method",  "ProfileLogResponse" );
        DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)iTransID);
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)"DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "dest", (std::string)profileLogMsg.src); // Send back to requester
        DED_PUT_STDSTRING	( encoder_ptr, "src", (std::string)"DFD_1.1" );// This is HERE!!!
        DED_PUT_STDSTRING	( encoder_ptr, "status", (std::string)"Log Saved in database" );
        DED_PUT_STRUCT_END( encoder_ptr, "DFDResponse" );

        DED_GET_WSCLIENT_DATAFRAME(encoder_ptr, response.frame);
    }
    else
    {
        /// Some error occurred when appending/writing to log
        response.enumresp = ProfileLogResponse::enumResponse::error;
    }

    return bResult;
}

bool fn119_HandleProfileLog(ProfileLogRequest profileLogReq, ProfileLogResponse &response)
{
    bool bResult=false;
    response.enumresp = ProfileLogResponse::enumResponse::error;

    bResult = fn1191_AddToLog(profileLogReq.iTransID, profileLogReq.profileLogMsg, response);

    return bResult;
}

////////////////////////
/// BOOST TEST AREA
////////////////////////
///
BOOST_AUTO_TEST_SUITE( DFD1_1_9_ProfileLog_Test ) // name of the test suite
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE( ConnectToWebserver )
{
    C1_1_Profile C11;

    /// start a local webserver, based on current version
    WebServer server;
    server.start("127.0.0.1","8998");

    /// wait some time for server to start
    boost::posix_time::seconds workTime(1);
    boost::this_thread::sleep(workTime);


    bool bResult = C11.ConnectToWebserver("ws://127.0.0.1:8998"); // dataframes will be handled inside C1_1_Profile::HandleDataframe_Response
    if(bResult)
    {
        C11.StopHandling();
        bResult = C11.WaitForCompletion();
    }

    server.stop();// signal stop to server
    /// wait some time for server to stop
    boost::this_thread::sleep(workTime);

    BOOST_CHECK(bResult == true);
}

BOOST_AUTO_TEST_CASE( CommunicateWithWebserver )
{
    C1_1_Profile C11;

    //boost::filesystem::path full_path( boost::filesystem::current_path() );
    //std::cout << "[CommunicateWithWebserver] : Current path is : " << full_path << std::endl;

    //system("cd ../../../../"); // change dir to focus on home branch of codeblocks_project -- eg. ~/codeblocks_projects/serup
    //chdir("../../../../");
    SetDir();

    //boost::filesystem::path new_full_path( boost::filesystem::current_path() );
    //std::cout << "[CommunicateWithWebserver] : Current path is : " << new_full_path << std::endl;

    /// start a local webserver, based on current version
    WebServer server;
    server.start("127.0.0.1","9898");

    /// wait some time for server to start
    boost::posix_time::seconds workTime(1);
    boost::this_thread::sleep(workTime);


    bool bResult = C11.ConnectToWebserver("ws://127.0.0.1:9898"); // dataframes will be handled inside C1_1_Profile::HandleDataframe_Response
    if(bResult)
    {
        /// send a dataframe to yourself - meaning webserver will throw it back here
        dataframe tempframe;
        DED_START_ENCODER(encoder_ptr);
          DED_PUT_STRUCT_START( encoder_ptr, "DFDRequest" );
          //+ fixed area start
            DED_PUT_METHOD	( encoder_ptr, "Method",  "1_1_6_LoginProfile" );
            DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)66);
            DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)"DED1.00.00" );
            DED_PUT_STDSTRING	( encoder_ptr, "dest", (std::string)"DFD_1.1" ); // destination is profile DFD
            DED_PUT_STDSTRING	( encoder_ptr, "src", (std::string)"DFD_1.1" ); /// since DFD_1_1 is profile and connected as such on scanvaserver - round trip means this is here
          //- fixed area end
          //+ Profile request area start
            DED_PUT_STDSTRING	( encoder_ptr, "STARTrequest", (std::string)"LoginProfileRequest" );
            DED_PUT_STDSTRING	( encoder_ptr, "STARTDATAstream", (std::string)"116" ); // TODO: add datadictionary id for the datastream
            DED_PUT_STDSTRING	( encoder_ptr, "profileID", (std::string)"chromeonwin81" ); // unique id for registered profile
            DED_PUT_STDSTRING	( encoder_ptr, "username", (std::string)"22980574" );
            DED_PUT_STDSTRING	( encoder_ptr, "password", (std::string)"EB23445" );
            DED_PUT_STDSTRING	( encoder_ptr, "ENDDATAstream", (std::string)"116" ); // TODO: add datadictionary id for the datastream
            DED_PUT_STDSTRING	( encoder_ptr, "ENDrequest", (std::string)"LoginProfileRequest" );
          //- Profile request area end
          DED_PUT_STRUCT_END( encoder_ptr, "DFDRequest" );
        DED_GET_ENCODED_DATAFRAME(encoder_ptr, tempframe);

        bResult = C11.SendDataframe(tempframe);
        if(bResult==true)
        {
            /// wait for dataframe to come back
            bResult = C11.WaitForReceivingDataframe(8000);
            /// stop communication
            C11.StopHandling();
            C11.WaitForCompletion();
        }
    }

    /// stop local server
    server.stop();// signal stop to server
    /// wait some time for server to stop
    boost::this_thread::sleep(workTime);

    BOOST_CHECK(bResult == true);
}

BOOST_AUTO_TEST_CASE( LoginToProfile )
{
    bool bResult = false;
    WebServer server;
    C1_1_Profile C11;

    SetDir();
    boost::filesystem::path new_full_path( boost::filesystem::current_path() );

    std::string EntityFileName = "DADEC3PO6BAA30E8AD733671370FECFC";
    std::string strFilepath = new_full_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity

    /// start a local webserver, based on current version
    server.start("127.0.0.1","7770");

    /// wait some time for server to start
    boost::posix_time::seconds workTime(3);
    boost::this_thread::sleep(workTime);

    bResult = C11.ConnectToWebserver("ws://127.0.0.1:7770"); // dataframes will be handled inside C1_1_Profile::HandleDataframe_Response
    if(bResult)
    {
        /// First create a test profile
        CreateTestProfileFile(EntityFileName);

        /// send a dataframe to yourself - meaning webserver will throw it back here
        dataframe tempframe;
        DED_START_ENCODER(encoder_ptr);
        DED_PUT_STRUCT_START( encoder_ptr, "DFDRequest" );
        //+ fixed area start
        DED_PUT_METHOD	( encoder_ptr, "Method",  "1_1_6_LoginProfile" );
        DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)22);
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)"DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "dest", (std::string)"DFD_1.1" ); // destination is profile DFD
        DED_PUT_STDSTRING	( encoder_ptr, "src", (std::string)"DFD_1.1" ); /// since DFD_1_1 is profile and connected as such on scanvaserver - round trip means this is here
        //- fixed area end
        //+ Profile request area start
        DED_PUT_STDSTRING	( encoder_ptr, "STARTrequest", (std::string)"LoginProfileRequest" );
        DED_PUT_STDSTRING	( encoder_ptr, "STARTDATAstream", (std::string)"116" ); // TODO: add datadictionary id for the datastream
        DED_PUT_STDSTRING	( encoder_ptr, "profileID", (std::string)EntityFileName ); // unique id for registered profile - this is actually the name of the main profile entityfile
        DED_PUT_STDSTRING	( encoder_ptr, "username", (std::string)"serup" );
        DED_PUT_STDSTRING	( encoder_ptr, "password", (std::string)"EB23445" );
        DED_PUT_STDSTRING	( encoder_ptr, "ENDDATAstream", (std::string)"116" ); // TODO: add datadictionary id for the datastream
        DED_PUT_STDSTRING	( encoder_ptr, "ENDrequest", (std::string)"LoginProfileRequest" );
        //- Profile request area end
        DED_PUT_STRUCT_END( encoder_ptr, "DFDRequest" );
        DED_GET_ENCODED_DATAFRAME(encoder_ptr, tempframe);

        bResult = C11.SendDataframe(tempframe);
        if(bResult==true)
            std::cout << "LoginProfileRequest send " << std::endl;
        /// wait for dataframe to come back
        //bResult = C11.WaitForReceivingDataframe(8000);
        /// wait for login procedure
        //bResult = C11.WaitForLoginProfile(8000);
        /// wait for a response dataframe
        bResult = C11.WaitForReceivingUnknownDataframe(3000);
        bResult=true; // test anyway if dataframe is in trashbucket
        if(bResult==true)
        {
            /// check if server is accepting the login
            // parse incomming data ( from trash, since dest is src
            std::string strMethod=(std::string)"";
            unsigned short iTransID = 0;
            std::string strProtocolTypeID=(std::string)"";
            std::string strDestination=(std::string)"";
            std::string strSource=(std::string)"";
            std::string strStatus=(std::string)"";
            DED_PUT_DATA_IN_DECODER(trashbucket_decoder_ptr,(unsigned char*)C11.trashbucketDED.pDEDarray,C11.trashbucketDED.sizeofDED);
            DED_GET_STRUCT_START( trashbucket_decoder_ptr, "DFDResponse" );
            DED_GET_METHOD( trashbucket_decoder_ptr, "Method", strMethod );
            DED_GET_USHORT( trashbucket_decoder_ptr, "TransID", iTransID);
            DED_GET_STDSTRING( trashbucket_decoder_ptr, "protocolTypeID", strProtocolTypeID );
            DED_GET_STDSTRING( trashbucket_decoder_ptr, "dest", strDestination );
            DED_GET_STDSTRING( trashbucket_decoder_ptr, "src", strSource );
            DED_GET_STDSTRING( trashbucket_decoder_ptr, "status", strStatus );
            if(strStatus == (std::string)"ACCEPTED" && iTransID == (unsigned short)22)
            {
                std::cout << "LoginProfileRequest ACCEPTED " << std::endl;
                bResult = true;
            }
            else
            {
                std::cout << "LoginProfileRequest FAILED " << std::endl;
                bResult = false;
            }
        }
        BOOST_CHECK(bResult == true);

        /// stop communication
        C11.StopHandling();
        C11.WaitForCompletion();
    }
    else
    {
        std::cout << "ConnectToWebserver ERROR " << std::endl;
    }


    /// stop local server
    server.stop();// signal stop to server

    /// cleanup after this test
    boost::filesystem::remove( strFilepath );
    std::string strFilepathToast = new_full_path.string() + relativeENTITIES_DATABASE_TOAST_PLACE + (std::string)"22980574" + ".xml"; // database file for entity TOAST
    boost::filesystem::remove( strFilepathToast );

    /// wait some time for server to stop
    boost::this_thread::sleep(workTime);

    BOOST_CHECK(bResult == true);
}

BOOST_AUTO_TEST_CASE( _1191_AddToLog )
{
    bool bResult = false;
//    C1_1_Profile C11;
    std::string EntityFileName = "DADEC3PO6BAA30E8AD733671370FECFC";

    SetDir();

    ProfileLogRequest plogreq;
    /// Now try and add something to LOG
    plogreq.eLogRequest = ProfileLogRequest::enumLogRequest::AddToLog;
    plogreq.iTransID = 88;
    plogreq.strProfileID = EntityFileName;
    plogreq.profileLogMsg.LogEntryCounter = GetTime(); /// should be a time with seconds, so it will function as a unique continues counter
    plogreq.profileLogMsg.protocolTypeID = "";
    plogreq.profileLogMsg.profileID = EntityFileName;
    plogreq.profileLogMsg.src = "serup"; // unique id for connection of source
    plogreq.profileLogMsg.dest = "clientapp"; // unique id for connection of client
    plogreq.profileLogMsg.srcUsername = "serup";
    plogreq.profileLogMsg.destUsername = "clientapp";
    plogreq.profileLogMsg.showdelivery = "true";
    plogreq.profileLogMsg.timeReceived = GetYearMonthDay(); /// also used to combine filename of log
    plogreq.profileLogMsg.timeSend = "";
    plogreq.profileLogMsg.delivered = "Tue, 15 Apr 2014 10:28:09 GMT";
    plogreq.profileLogMsg.message = "Hello world, this is a test of writing into the history log";
    plogreq.profileLogMsg.acceptedIndex = "2";
    plogreq.profileLogMsg.history_chunk_id = "";


//    bResult = C11.fn119_HandleProfileLog(plogreq);
    ProfileLogResponse response;
    bResult = fn119_HandleProfileLog(plogreq, response);

    BOOST_CHECK(bResult == true);
}


