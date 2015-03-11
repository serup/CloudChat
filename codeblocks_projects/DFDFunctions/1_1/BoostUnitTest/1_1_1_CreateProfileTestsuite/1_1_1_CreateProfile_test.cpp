#define BOOST_TEST_MODULE DFD1_1_1_CreateProfile_Test
#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>      // std::ifstream
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
        out.open("1_1_1_test_results.xml");
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
        if(pServer)
        {
            pServer->stop();
        }
    }

    ~WebServer()
    {
        //stop();
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

        if(pServer)
        {
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
    std::cout << "[DFD1_1_1 - Testsetup] : Current path is : " << full_path << std::endl;
    chdir("../../../../");
    boost::filesystem::path new_full_path( boost::filesystem::current_path() );
    std::cout << "[DFD1_1_1 - Testsetup] : Current path is : " << new_full_path << std::endl;
    bDirWasSet = true;
    }
}

BOOST_AUTO_TEST_SUITE (DFD1_1_1_CreateProfile_Test) // name of the test suite
BOOST_AUTO_TEST_SUITE_END( )

BOOST_AUTO_TEST_CASE(ConnectToWebserver)
{
    C1_1_Profile C11;
    bool bResult;
    /// wait some time for server to start
    boost::posix_time::seconds workTime(1);
    boost::this_thread::sleep(workTime);


    {/// start a local webserver, based on current version
    WebServer server;
    server.start("127.0.0.1","8998");
    /// wait some time for server to start
    boost::this_thread::sleep(workTime);


    bResult = C11.ConnectToWebserver("ws://127.0.0.1:8998"); // dataframes will be handled inside C1_1_Profile::HandleDataframe_Response
    if(bResult)
    {
        C11.StopHandling();
        bResult = C11.WaitForCompletion();
    }

    //server.stop();// signal stop to server
    }
    /// wait some time for server to stop
    boost::this_thread::sleep(workTime);

    BOOST_CHECK(bResult == true);
}

//NB! Remember that DFD has to also be updated if datadictionary have changed !!!
BOOST_AUTO_TEST_CASE(CreateProfile)
{
    C1_1_Profile C11;
    bool bResult;

    /// wait some time for server to start
    boost::posix_time::seconds workTime(1);
    boost::this_thread::sleep(workTime);

    /// Setup local dir
    SetDir();

    {/// start a local webserver, based on current version
    WebServer server;
    server.start("127.0.0.1","7798");
    /// wait some time for server to start
    boost::this_thread::sleep(workTime);


    bResult = C11.ConnectToWebserver("ws://127.0.0.1:7798"); // dataframes will be handled inside C1_1_Profile::HandleDataframe_Response
    if(bResult)
    {
        std::string EntityFileName = C11.guid(); // random guid
        std::string EntityTOASTFileName = C11.guid(); // random guid

        /// send a dataframe to yourself - meaning webserver will throw it back here
        dataframe tempframe;
        DED_START_ENCODER(encoder_ptr);
        DED_PUT_STRUCT_START( encoder_ptr, "DFDRequest" );
        //+ fixed area start
        DED_PUT_METHOD	( encoder_ptr, "Method",  "1_1_1_CreateProfile" );
        DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)66);
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)"DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "dest", (std::string)"DFD_1.1" ); // This is HERE!!!
        DED_PUT_STDSTRING	( encoder_ptr, "src", (std::string)"DFD_1.1" );
        //- fixed area end
        //+ Profile record area start
        DED_PUT_STDSTRING	( encoder_ptr, "STARTrequest", (std::string)"EmployeeRequest" );
        DED_PUT_STDSTRING	( encoder_ptr, "STARTrecord", (std::string)"record" );
        DED_PUT_STDSTRING	( encoder_ptr, "profileID", (std::string)EntityFileName );
        DED_PUT_STDSTRING	( encoder_ptr, "profileName", (std::string)"TestProfile" );
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID",(std::string)"DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "sizeofProfileData", (std::string)"0" );
        DED_PUT_STDSTRING	( encoder_ptr, "profile_chunk_id", (std::string)EntityTOASTFileName );
        DED_PUT_STDSTRING   ( encoder_ptr, "AccountStatus", (std::string)"1" );
        DED_PUT_STDSTRING   ( encoder_ptr, "ExpireDate", (std::string)"20160101" );
        DED_PUT_STDSTRING   ( encoder_ptr, "ProfileStatus", (std::string)"2" );
        DED_PUT_STDSTRING	( encoder_ptr, "STARTtoast", (std::string)"elements" );
        ///+ start toast elements
        DED_PUT_ELEMENT( encoder_ptr, "profile", "lifecyclestate", "1") /// will add profile_chunk_id and profile_chunk_data
        DED_PUT_ELEMENT( encoder_ptr, "profile", "username", "johndoe" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "password", "12345678" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "devicelist", " " )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "firstname", "John" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "lastname", "Doe" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "streetname", "nostreet" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "streetno", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "postalcode", "90210" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "city", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "state", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "country", "USA" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "email", "johndoe@email.com" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "mobilephone", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "title", "homeless" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "about", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "foto", "http://77.75.165.130/DOPS/CloudChat/CloudChatManager/images/janedoe5.png" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "type", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "status", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "expiredate", "20160101" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "creditor", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "terms", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "subscriptions", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "paymentdetails", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "notificationlevel", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "bankaccount", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "memberships", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "friendships", "whitehouse" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "presence", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "location", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "context", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "schedule", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "deviceID", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "experience", "none" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "skills", "shoplifting" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "messagesboard", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "eventlog", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "supervisor", "DADER2D26BAA30E8AD733671370FECFC" )
        ///- end toast elements
        DED_PUT_STDSTRING	( encoder_ptr, "ENDtoast", (std::string)"elements" );
        //- Profile record area end
        DED_PUT_STDSTRING	( encoder_ptr, "ENDrecord", (std::string)"record" );
        DED_PUT_STDSTRING	( encoder_ptr, "ENDrequest", (std::string)"EmployeeRequest" );
        //- Profile request area end
        DED_PUT_STRUCT_END( encoder_ptr, "DFDRequest" );
        DED_GET_ENCODED_DATAFRAME(encoder_ptr, tempframe);

        /// First clean from previous test run
        // Remove Entity file
        boost::filesystem::path my_current_path( boost::filesystem::current_path() );
        std::string strFilepath = my_current_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
        boost::filesystem::remove( strFilepath );
        // Remove TOAST File
        std::string strFilepathToast = my_current_path.string() + relativeENTITIES_DATABASE_TOAST_PLACE + EntityTOASTFileName + ".xml"; // database file for entity TOAST
        boost::filesystem::remove( strFilepathToast );


        bResult = C11.SendDataframe(tempframe);
        if(bResult==true)
        {
            /// wait for dataframe to come back
            bResult = C11.WaitForReceivingDataframe(8000);
            if(bResult==true) /// wait for creation of Profile in database file - meaning request is parsed
                bResult = C11.WaitForCreateProfile(8000);
            if(bResult==true)
            {
                /// wait for response of request - meaning a response dataframe is send and received (since dest is same as source - dataframe will be send and received inside C11 object)
                bResult = C11.WaitForReceivingUnknownDataframe(8000);
                BOOST_CHECK(bResult == true);

                /// stop communication
                C11.StopHandling();
                C11.WaitForCompletion();

                /// verify that profile data stored, matches the provided data
                std::vector<Elements> wDEDElements;
                CDatabaseControl DbCtrl;
                bResult=false;
                bResult = DbCtrl.ftgt((std::string)"Profile",(std::string)EntityFileName,wDEDElements);
                BOOST_CHECK(bResult == true);
                std::vector<Elements> vec;
                ADD_ELEMENT(vec, "lifecyclestate","1");
                ADD_ELEMENT(vec, "username","johndoe");
                ADD_ELEMENT(vec, "password","12345678");
                ADD_ELEMENT(vec, "devicelist"," ");
                ADD_ELEMENT(vec, "firstname","John");
                ADD_ELEMENT(vec, "lastname","Doe");
                ADD_ELEMENT(vec, "streetname", "nostreet" );
                ADD_ELEMENT(vec, "streetno", "..." );
                ADD_ELEMENT(vec, "postalcode", "90210" );
                ADD_ELEMENT(vec, "city", "..." );
                ADD_ELEMENT(vec, "state", "..." );
                ADD_ELEMENT(vec, "country", "USA" );
                ADD_ELEMENT(vec, "email", "johndoe@email.com" );
                ADD_ELEMENT(vec, "mobilephone", "..." );
                ADD_ELEMENT(vec, "title", "homeless" );
                ADD_ELEMENT(vec, "about", "..." );
                ADD_ELEMENT(vec, "foto", "http://77.75.165.130/DOPS/CloudChat/CloudChatManager/images/janedoe5.png" );
                ADD_ELEMENT(vec, "type", "..." );
                ADD_ELEMENT(vec, "status", "..." );
                ADD_ELEMENT(vec, "expiredate", "20160101" );
                ADD_ELEMENT(vec, "creditor", "..." );
                ADD_ELEMENT(vec, "terms", "..." );
                ADD_ELEMENT(vec, "subscriptions", "..." );
                ADD_ELEMENT(vec, "paymentdetails", "..." );
                ADD_ELEMENT(vec, "notificationlevel", "..." );
                ADD_ELEMENT(vec, "bankaccount", "..." );
                ADD_ELEMENT(vec, "memberships", "..." );
                ADD_ELEMENT(vec, "friendships", "whitehouse" );
                ADD_ELEMENT(vec, "presence", "..." );
                ADD_ELEMENT(vec, "location", "..." );
                ADD_ELEMENT(vec, "context", "..." );
                ADD_ELEMENT(vec, "schedule", "..." );
                ADD_ELEMENT(vec, "deviceID", "..." );
                ADD_ELEMENT(vec, "experience", "none" );
                ADD_ELEMENT(vec, "skills", "shoplifting" );
                ADD_ELEMENT(vec, "messagesboard", "..." );
                ADD_ELEMENT(vec, "eventlog", "..." );
                ADD_ELEMENT(vec, "supervisor", "DADER2D26BAA30E8AD733671370FECFC" );
                BOOST_FOREACH(Elements f, vec)
                {
                    bResult=false;
                    bResult = DbCtrl.compare_element(wDEDElements,(std::string)f.strElementID,f.ElementData);
                    std::cout << "compare toast element : " << f.strElementID << " : check : " << bResult << std::endl;
                    BOOST_CHECK(bResult == true);
                }
            }
        }
        /// cleanup from this test run
        boost::filesystem::remove( strFilepath );
        boost::filesystem::remove( strFilepathToast );
    }

    /// stop local server
    //server.stop();// signal stop to server -- NB! somehow this conflicts with next test case - why I do not know
    }
    /// wait some time for server to stop
    boost::this_thread::sleep(workTime);

    BOOST_CHECK(bResult == true);
}


BOOST_AUTO_TEST_CASE(CreateProfile_with_embedded_foto)
{
    C1_1_Profile C11;
    bool bResult;

    /// wait some time for server to start
    boost::posix_time::seconds workTime(1);
    boost::this_thread::sleep(workTime);

    /// Setup local dir
    SetDir();

    {/// start a local webserver, based on current version
    WebServer server;
    server.start("127.0.0.1","7687");
    /// wait some time for server to start
    boost::this_thread::sleep(workTime);


    bResult = C11.ConnectToWebserver("ws://127.0.0.1:7687"); // dataframes will be handled inside C1_1_Profile::HandleDataframe_Response
    if(bResult)
    {
        std::string EntityFileName = C11.guid(); // random guid
        std::string EntityTOASTFileName = C11.guid(); // random guid

        /// send a dataframe to yourself - meaning webserver will throw it back here
        dataframe tempframe;
        DED_START_ENCODER(encoder_ptr);
        DED_PUT_STRUCT_START( encoder_ptr, "DFDRequest" );
        //+ fixed area start
        DED_PUT_METHOD	( encoder_ptr, "Method",  "1_1_1_CreateProfile" );
        DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)66);
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)"DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "dest", (std::string)"DFD_1.1" ); // This is HERE!!!
        DED_PUT_STDSTRING	( encoder_ptr, "src", (std::string)"DFD_1.1" );
        //- fixed area end
        //+ Profile record area start
        DED_PUT_STDSTRING	( encoder_ptr, "STARTrequest", (std::string)"EmployeeRequest" );
        DED_PUT_STDSTRING	( encoder_ptr, "STARTrecord", (std::string)"record" );
        DED_PUT_STDSTRING	( encoder_ptr, "profileID", (std::string)EntityFileName );
        DED_PUT_STDSTRING	( encoder_ptr, "profileName", (std::string)"TestProfile" );
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID",(std::string)"DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "sizeofProfileData", (std::string)"0" );
        DED_PUT_STDSTRING	( encoder_ptr, "profile_chunk_id", (std::string)EntityTOASTFileName );
        DED_PUT_STDSTRING   ( encoder_ptr, "AccountStatus", (std::string)"1" );
        DED_PUT_STDSTRING   ( encoder_ptr, "ExpireDate", (std::string)"20160101" );
        DED_PUT_STDSTRING   ( encoder_ptr, "ProfileStatus", (std::string)"2" );
        DED_PUT_STDSTRING	( encoder_ptr, "STARTtoast", (std::string)"elements" );
        ///+ start toast elements
        DED_PUT_ELEMENT( encoder_ptr, "profile", "lifecyclestate", "1") /// will add profile_chunk_id and profile_chunk_data
        DED_PUT_ELEMENT( encoder_ptr, "profile", "username", "serup" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "password", "sussiskoller" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "devicelist", " " )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "firstname", "Johnny" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "lastname", "Serup" )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "streetname", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "streetno", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "postalcode", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "city", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "state", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "country", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "email", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "mobilephone", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "title", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "about", "..." )
        DED_PUT_ELEMENT( encoder_ptr, "profile", "foto", "data:image/jpeg;base64,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" )
//        data:image/jpeg;base64,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
        ///- end toast elements
        DED_PUT_STDSTRING	( encoder_ptr, "ENDtoast", (std::string)"elements" );
        //- Profile record area end
        DED_PUT_STDSTRING	( encoder_ptr, "ENDrecord", (std::string)"record" );
        DED_PUT_STDSTRING	( encoder_ptr, "ENDrequest", (std::string)"EmployeeRequest" );
        //- Profile request area end
        DED_PUT_STRUCT_END( encoder_ptr, "DFDRequest" );
        DED_GET_ENCODED_DATAFRAME(encoder_ptr, tempframe);

        /// First clean from previous test run
        // Remove Entity file
        boost::filesystem::path my_current_path( boost::filesystem::current_path() );
        std::string strFilepath = my_current_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
        boost::filesystem::remove( strFilepath );
        // Remove TOAST File
        std::string strFilepathToast = my_current_path.string() + relativeENTITIES_DATABASE_TOAST_PLACE + EntityTOASTFileName + ".xml"; // database file for entity TOAST
        boost::filesystem::remove( strFilepathToast );


        bResult = C11.SendDataframe(tempframe);
        if(bResult==true)
        {
            /// wait for dataframe to come back
            bResult = C11.WaitForReceivingDataframe(8000);
            if(bResult==true) /// wait for creation of Profile in database file - meaning request is parsed
                bResult = C11.WaitForCreateProfile(8000);
            if(bResult==true)
            {
                /// wait for response of request - meaning a response dataframe is send and received (since dest is same as source - dataframe will be send and received inside C11 object)
                bResult = C11.WaitForReceivingUnknownDataframe(8000);
                BOOST_CHECK(bResult == true);

                /// stop communication
                C11.StopHandling();
                C11.WaitForCompletion();

                /// verify that profile data stored, matches the provided data
                std::vector<Elements> wDEDElements;
                CDatabaseControl DbCtrl;
                bResult=false;
                bResult = DbCtrl.ftgt((std::string)"Profile",(std::string)EntityFileName,wDEDElements);
                BOOST_CHECK(bResult == true);
                std::vector<Elements> vec;
                ADD_ELEMENT(vec, "lifecyclestate","1");
                ADD_ELEMENT(vec, "username","serup");
                ADD_ELEMENT(vec, "password","sussiskoller");
                ADD_ELEMENT(vec, "devicelist"," ");
                ADD_ELEMENT(vec, "firstname","Johnny");
                ADD_ELEMENT(vec, "lastname","Serup");
                ADD_ELEMENT(vec, "streetname", "..." );
                ADD_ELEMENT(vec, "streetno", "..." );
                ADD_ELEMENT(vec, "postalcode", "..." );
                ADD_ELEMENT(vec, "city", "..." );
                ADD_ELEMENT(vec, "state", "..." );
                ADD_ELEMENT(vec, "country", "..." );
                ADD_ELEMENT(vec, "email", "..." );
                ADD_ELEMENT(vec, "mobilephone", "..." );
                ADD_ELEMENT(vec, "title", "..." );
                ADD_ELEMENT(vec, "about", "..." );
                ADD_ELEMENT(vec, "foto", "data:image/jpeg;base64,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" );
                BOOST_FOREACH(Elements f, vec)
                {
                    bResult=false;
                    bResult = DbCtrl.compare_element(wDEDElements,(std::string)f.strElementID,f.ElementData);
                    if(f.strElementID == "foto")
                        std::cout << "[CreateProfile_with_embedded_foto] compare toast element : " << f.strElementID << " : check : " << bResult << std::endl;
                    BOOST_CHECK(bResult == true);
                }
            }
        }
        /// cleanup from this test run
        boost::filesystem::remove( strFilepath );
        boost::filesystem::remove( strFilepathToast );
    }

    /// stop local server
    //server.stop();// signal stop to server
    }
    /// wait some time for server to stop
    boost::this_thread::sleep(workTime);

    BOOST_CHECK(bResult == true);
}

/*
BOOST_AUTO_TEST_CASE(CreateNewProfile)
{
    bool bResult = false;
    C1_1_Profile C11;

	/// Setup local dir
    SetDir();

    /// start a local webserver, based on current version
    WebServer server;
    server.start("127.0.0.1","8898");

    /// wait some time for server to start
    boost::posix_time::seconds workTime(1);
    boost::this_thread::sleep(workTime);

    bResult = C11.ConnectToWebserver("ws://127.0.0.1:8898"); // dataframes will be handled inside C1_1_Profile::HandleDataframe_Response
    BOOST_CHECK(bResult == true);
    if(bResult)
    {
        /// send a dataframe to yourself - meaning webserver will throw it back here
        dataframe tempframe;
        DED_START_ENCODER(encoder_ptr);
        DED_PUT_STRUCT_START( encoder_ptr, "DFDRequest" );
        //+ fixed area start
        DED_PUT_METHOD	( encoder_ptr, "Method",  "1_1_1_CreateProfile" );
        DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)66);
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)"DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "dest", (std::string)"DFD_1.1" ); // This is HERE!!!
        DED_PUT_STDSTRING	( encoder_ptr, "src", (std::string)"DFD_1.1" );
        //- fixed area end
        //+ Profile record area start
        DED_PUT_STDSTRING	( encoder_ptr, "STARTrequest", (std::string)"EmployeeRequest" );
        DED_PUT_STDSTRING	( encoder_ptr, "STARTrecord", (std::string)"record" );
        DED_PUT_STDSTRING	( encoder_ptr, "ProfileID", (std::string)"TestProfile1" );
        DED_PUT_STDSTRING	( encoder_ptr, "ProfileName", (std::string)"steander" );
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID",(std::string)"DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "sizeofProfileData", (std::string)"0" );
        DED_PUT_STDSTRING	( encoder_ptr, "Profile_chunk_id", (std::string)"0" );
        DED_PUT_STDSTRING	( encoder_ptr, "ENDrecord", (std::string)"record" );
        DED_PUT_STDSTRING	( encoder_ptr, "ENDrequest", (std::string)"EmployeeRequest" );
        //- Profile request area end
        DED_PUT_STRUCT_END( encoder_ptr, "DFDRequest" );
        DED_GET_ENCODED_DATAFRAME(encoder_ptr, tempframe);

        /// First clean from previous test run

        // Remove Entity file
        boost::filesystem::path my_current_path( boost::filesystem::current_path() );
        std::string EntityFileName = "steander";
        std::string strFilepath = my_current_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
        boost::filesystem::remove( strFilepath );

        // Remove TOAST File
        //std::string strFilepathToast = my_current_path.string() + relativeENTITIES_DATABASE_TOAST_PLACE + (std::string)"123456" + ".xml"; // database file for entity TOAST
        //boost::filesystem::remove( strFilepathToast );


        bResult = C11.SendDataframe(tempframe);
        BOOST_CHECK(bResult == true);
        if(bResult==true)
        {
            /// wait for dataframe to come back
            bResult = C11.WaitForReceivingDataframe(8000);
            if(bResult==true) /// wait for creation of Profile in database file - meaning request is parsed
                bResult = C11.WaitForCreateProfile(8000);
            if(bResult==true) {            /// stop communication
            C11.StopHandling();
            C11.WaitForCompletion();

            /// TODO - verify that profile data stored, matches the provided data
            // Read entity file using general read for database entity files
            std::vector<Elements> wDEDElements;
            CDatabaseControl DbCtrl;
            bResult = DbCtrl.ftgt((std::string)"Profile",(std::string)"steander",wDEDElements);
            BOOST_CHECK(bResult == true);
            }
        }
        /// cleanup from this test run
        boost::filesystem::remove( strFilepath );
        //boost::filesystem::remove( strFilepathToast );
    }

    /// stop local server
    server.stop();// signal stop to server
    /// wait some time for server to stop
    boost::this_thread::sleep(workTime);

    BOOST_CHECK(bResult == true);
}


BOOST_AUTO_TEST_CASE(CreateProfileFileResponse)
{
    C1_1_Profile C11;

    /// Setup local dir
    SetDir();

    /// start a local webserver, based on current version
    WebServer server;
    server.start("127.0.0.1","7878");

    /// wait some time for server to start
    boost::posix_time::seconds workTime(1);
    boost::this_thread::sleep(workTime);

    bool bResult = C11.ConnectToWebserver("ws://127.0.0.1:7878"); // dataframes will be handled inside C1_1_Profile::HandleDataframe_Response
    if(bResult)
    {
        /// send a dataframe to yourself - meaning webserver will throw it back here
        dataframe tempframe;
        DED_START_ENCODER(encoder_ptr);
        DED_PUT_STRUCT_START( encoder_ptr, "DFDRequest" );
        //+ fixed area start
        DED_PUT_METHOD	( encoder_ptr, "Method",  "1_1_1_CreateProfile" );
        DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)22);
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)"DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "dest", (std::string)"DFD_1.1" ); // This is HERE!!!
        DED_PUT_STDSTRING	( encoder_ptr, "src", (std::string)"DFD_1.1" );
        //- fixed area end
        //+ Profile record area start
        DED_PUT_STDSTRING	( encoder_ptr, "STARTrequest", (std::string)"EmployeeRequest" );
        DED_PUT_STDSTRING	( encoder_ptr, "STARTrecord", (std::string)"record" );
        DED_PUT_STDSTRING	( encoder_ptr, "ProfileID", (std::string)"TestProfile2" );
        DED_PUT_STDSTRING	( encoder_ptr, "ProfileName", (std::string)"HelloWorld" );
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID",(std::string)"DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "sizeofProfileData", (std::string)"0" );
        DED_PUT_STDSTRING	( encoder_ptr, "Profile_chunk_id", (std::string)"0" );
        DED_PUT_STDSTRING	( encoder_ptr, "ENDrecord", (std::string)"record" );
        DED_PUT_STDSTRING	( encoder_ptr, "ENDrequest", (std::string)"EmployeeRequest" );
        //- Profile request area end
        DED_PUT_STRUCT_END( encoder_ptr, "DFDRequest" );
        DED_GET_ENCODED_DATAFRAME(encoder_ptr, tempframe);

        /// First clean from previous test run

        // Remove Entity file
        boost::filesystem::path my_current_path( boost::filesystem::current_path() );
        std::string EntityFileName = "HelloWorld";
        std::string strFilepath = my_current_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
        boost::filesystem::remove( strFilepath );

        // Remove TOAST File
        //std::string strFilepathToast = my_current_path.string() + relativeENTITIES_DATABASE_TOAST_PLACE + (std::string)"123456" + ".xml"; // database file for entity TOAST
        //boost::filesystem::remove( strFilepathToast );


        bResult = C11.SendDataframe(tempframe);
        if(bResult==true)
        {
            /// wait for dataframe to come back - meaning request is now inside the DFD1.1.1
            bResult = C11.WaitForReceivingDataframe(8000);
            if(bResult) /// wait for creation of Profile in database file - meaning request is parsed
                bResult = C11.WaitForCreateProfile(8000);
            if(bResult==true) {
            /// wait for response of request - meaning a response dataframe is send and received (since dest is same as source - dataframe will be send and received inside C11 object)
            bResult = C11.WaitForReceivingUnknownDataframe(8000);

            /// stop communication
            C11.StopHandling();
            C11.WaitForCompletion();
            }
        }

        /// cleanup from this test run
        boost::filesystem::remove( strFilepath );
        //boost::filesystem::remove( strFilepathToast );

    }

    /// stop local server
    server.stop();// signal stop to server
    /// wait some time for server to stop
    boost::this_thread::sleep(workTime);

    BOOST_CHECK(bResult == true);
}

BOOST_AUTO_TEST_CASE(ReadProfileFileResponse)
{
    C1_1_Profile C11;
    unsigned short FetchProfile_iTransID = 44;

    /// Setup local dir
    SetDir();

    /// start a local webserver, based on current version
    WebServer server;
    server.start("127.0.0.1","5859");

    /// wait some time for server to start
    boost::posix_time::seconds workTime(1);
    boost::this_thread::sleep(workTime);


    bool bResult = C11.ConnectToWebserver("ws://127.0.0.1:5859"); // dataframes will be handled inside C1_1_Profile::HandleDataframe_Response
    if(bResult)
    {
        /// send a dataframe to yourself - meaning webserver will throw it back here
        dataframe tempframe;
        DED_START_ENCODER(encoder_ptr);
        DED_PUT_STRUCT_START( encoder_ptr, "DFDRequest" );
        //+ fixed area start
        DED_PUT_METHOD	( encoder_ptr, "Method",  "1_1_1_CreateProfile" );
        DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)25);
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)"DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "dest", (std::string)"DFD_1.1" ); // This is HERE!!!
        DED_PUT_STDSTRING	( encoder_ptr, "src", (std::string)"DFD_1.1" );
        //- fixed area end
        //+ Profile record area start
        DED_PUT_STDSTRING	( encoder_ptr, "STARTrequest", (std::string)"EmployeeRequest" );
        DED_PUT_STDSTRING	( encoder_ptr, "STARTrecord", (std::string)"record" );
        DED_PUT_STDSTRING	( encoder_ptr, "ProfileID", (std::string)"22980574" );
        DED_PUT_STDSTRING	( encoder_ptr, "ProfileName", (std::string)"Benny" );
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID",(std::string)"DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "sizeofProfileData", (std::string)"0" );
        DED_PUT_STDSTRING	( encoder_ptr, "Profile_chunk_id", (std::string)"0" );
        DED_PUT_STDSTRING	( encoder_ptr, "ENDrecord", (std::string)"record" );
        DED_PUT_STDSTRING	( encoder_ptr, "ENDrequest", (std::string)"EmployeeRequest" );
        //- Profile request area end
        DED_PUT_STRUCT_END( encoder_ptr, "DFDRequest" );
        DED_GET_ENCODED_DATAFRAME(encoder_ptr, tempframe);

        /// First clean from previous test run

        // Remove Entity file
        boost::filesystem::path my_current_path( boost::filesystem::current_path() );
        std::string EntityFileName = "Benny";
        std::string strFilepath = my_current_path.string() + relativeENTITIES_DATABASE_PLACE + EntityFileName + ".xml"; // database file for entity
        boost::filesystem::remove( strFilepath );

        // Remove TOAST File
        //std::string strFilepathToast = my_current_path.string() + relativeENTITIES_DATABASE_TOAST_PLACE + (std::string)"123456" + ".xml"; // database file for entity TOAST
        //boost::filesystem::remove( strFilepathToast );

        bResult = C11.SendDataframe(tempframe);
        if(bResult==true)
        {
            /// wait for dataframe to come back - meaning request is now inside the DFD1.1.1
            bResult = C11.WaitForReceivingDataframe(8000);
            if(bResult==true) /// wait for creation of Profile in database file - meaning request is parsed
                bResult = C11.WaitForCreateProfile(8000);
            if(bResult==true) {
            /// wait for response of request - meaning a response dataframe is send and received (since dest is same as source - dataframe will be send and received inside C11 object)
            bResult = C11.WaitForReceivingUnknownDataframe(8000);
            BOOST_CHECK(bResult == true);
            if(bResult==true)
            {
                bResult=false;
                /// Profile file has now been written in database area -- now try to retrieve it

                /// Create a fetch Profile request
                dataframe tempframe2;
                DED_START_ENCODER(encoder_ptr2);
                DED_PUT_STRUCT_START( encoder_ptr2, "DFDRequest" );
                //+ fixed area start
                DED_PUT_METHOD	( encoder_ptr2, "Method",  "1_1_8_FetchProfile" );
                DED_PUT_USHORT	( encoder_ptr2, "TransID",	(unsigned short)FetchProfile_iTransID);
                DED_PUT_STDSTRING	( encoder_ptr2, "protocolTypeID", (std::string)"DED1.00.00" );
                DED_PUT_STDSTRING	( encoder_ptr2, "dest", (std::string)"DFD_1.1" ); // This is HERE!!!
                DED_PUT_STDSTRING	( encoder_ptr2, "src", (std::string)"DFD_1.1" );
                //- fixed area end
                //+ request area start
                DED_PUT_STDSTRING	( encoder_ptr2, "STARTrequest", (std::string)"EmployeeRequest" );
                DED_PUT_STDSTRING	( encoder_ptr2, "profileID", (std::string)"22980574" );
                DED_PUT_STDSTRING	( encoder_ptr2, "profileName", (std::string)"Benny" );
                DED_PUT_STDSTRING	( encoder_ptr2, "ENDrequest", (std::string)"EmployeeRequest" );
                //- request area end
                DED_PUT_STRUCT_END( encoder_ptr2, "DFDRequest" );
                DED_GET_ENCODED_DATAFRAME(encoder_ptr, tempframe2);

                /// send a request for retrieving Profile from stored Profile file in database area
                bResult = C11.SendDataframe(tempframe2);
                BOOST_CHECK(bResult == true);

                bResult=false;
                /// wait for request to be received in C11 -- meaning webserver is sending dataframe to attached C11 process
                bResult = C11.WaitForReceivingDataframe(8000);
                BOOST_CHECK(bResult == true);
                bResult=false;
                bResult = C11.WaitForFetchProfile(8000);/// wait for fetch of Profile in database file - meaning request is parsed, and a response will be send
                BOOST_CHECK(bResult == true);

                bResult=false;
                /// wait for response of request - meaning a response dataframe is send and received (since dest is same as source - dataframe will be send and received inside C11 object)
                bResult = C11.WaitForReceivingUnknownDataframe(8000);
                BOOST_CHECK(bResult == true);

                /// check if unknown dataframe in trashbucket is the one we send
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
                if(strStatus == (std::string)"ProfileFoundInDatabase" && iTransID == FetchProfile_iTransID)
                    bResult = true;
                else
                    bResult = false;
                BOOST_CHECK(bResult == true);
            }

            /// stop communication
            C11.StopHandling();
            C11.WaitForCompletion();
            }
        }

        /// cleanup from this test run
        boost::filesystem::remove( strFilepath );
        //boost::filesystem::remove( strFilepathToast );

    }

    /// stop local server
    server.stop();// signal stop to server
    /// wait some time for server to stop
    boost::this_thread::sleep(workTime);

    BOOST_CHECK(bResult == true);
}

BOOST_AUTO_TEST_CASE(CheckForConflicts)
{
    bool bResult = false;

    C1_1_Profile C11;

    BOOST_CHECK(bResult == true);
}

BOOST_AUTO_TEST_CASE(CreateEntity)
{
    bool bResult = false;

    C1_1_Profile C11;

    BOOST_CHECK(bResult == true);
}

BOOST_AUTO_TEST_CASE(SendConfirmationRequest)
{
    bool bResult = false;

    C1_1_Profile C11;

    BOOST_CHECK(bResult == true);
}

BOOST_AUTO_TEST_CASE(Authenticate)
{
    // This is called via handler from outside the Profile class

    bool bResult = false;

    C1_1_Profile C11;

    BOOST_CHECK(bResult == true);
}

BOOST_AUTO_TEST_CASE(CleanupAndEraseProfile)
{
    bool bResult = false;

    C1_1_Profile C11;

    BOOST_CHECK(bResult == true);
}

BOOST_AUTO_TEST_CASE(SendFundingRequest)
{
    bool bResult = false;

    C1_1_Profile C11;

    BOOST_CHECK(bResult == true);
}

BOOST_AUTO_TEST_CASE(ApplyFunding)
{
    bool bResult = false;

    C1_1_Profile C11;

    BOOST_CHECK(bResult == true);
}

BOOST_AUTO_TEST_CASE(CreateAccount)
{
    bool bResult = false;

    C1_1_Profile C11;

    BOOST_CHECK(bResult == true);
}

*/
