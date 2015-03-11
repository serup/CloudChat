/*
 * 1_1_Profile.hpp
 */
#include <string>
#include <boost/algorithm/hex.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/random_generator.hpp> // generator
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <iostream>
#include <fstream>      // std::ifstream
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include "../../websocket_server/wsclient/wsclient.hpp"
#include "../../websocket_server/dataframe.hpp"
#include "../../DataBaseControl/databasecontrol.hpp"
using namespace websocket;

/*!
//////////////////
// COMPILE INFO //
//////////////////
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
*/

/*! \brief How to compile in linux
 * g++ -g -o 1_1_1_CreateProfile 1_1_1_CreateProfile.cpp 1_1_1_CreateProfile.hpp -lboost_system -lboost_signals -lpthread -ldl -std=c++0x
 *
 */

/*! \brief how to compile in cygwin windows
 * g++ -g -o /cygdrive/c/Temp/1_1_1_CreateProfile 1_1_1_CreateProfile.cpp 1_1_1_CreateProfile.hpp -L"/cygdrive/c/Program Files/boost/boost_1_53_0/stage/lib/" -lboost_system -lboost_signals -lboost_thread -lws2_32 -D__USE_W32_SOCKETS -D_WIN32_WINNT -D__CYGWIN__  -D__MSABI_LONG=long -static -std=gnu++11
 *
 */

/*! \brief cygwin boost
 *  ./b2 -j4 cxxflags="-std=gnu++11" -d+2 --prefix="/cygdrive/c/Program Files/boost/boost_1_53_0" --build-type=complete --layout=tagged --with-date_time --with-thread --with-program_options --with-regex --with-test --with-system --with-serialization --with-graph --with-filesystem --disable-filesystem2 stage
 *
 */

/*! \brief Doxygen documentation
 * NB! DOXYBLOCKS DOES NOT WORK IF PATH HAVE SPACES !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *  2013-06-11 ALSO IT DOES NOT PT. GENERATE OUTPUT FROM SOURCE ON WINDOWS CYGWIN CODE::BLOCKS SETUP -- WHY I DO NOT KNOW
 *   http://forums.codeblocks.org/index.php/topic,15701.0.html
 * http://wiki.codeblocks.org/index.php?title=DoxyBlocks_plugin
 * http://sourceforge.net/projects/doxyblocks/
 * http://doxyblocks.sourceforge.net/
 * http://forums.codeblocks.org/index.php?action=search2
 * /usr/bin/doxygen.exe ../1_1_1_CreateProfileTestsuite/doxygen/doxyfile
 */

///////////////////////////
// DATAFLOW DIAGRAM INFO //
///////////////////////////
/*! \brief 1_1_1_CreateProfile
 *
 * This function takes a datastream of info about a new Profile and creates a new file in the database folder
 * that file will be handled using DED (DataEncoderDecoder)
 *
 * There are two main files for Profile, the Profile file and the Profiletoast file
 * The costomer file consists of main attributes (non-changeable) and the TOAST file consists of the bulk of the Profile
 * this bulk of data has a specific protocol in use with the DED protocol
 * Description of Profile toast aka. bulk data will be described in the systemspec under protocoltype for Profile
 *
 */


enum lifeCycleState{
    Temporary,
    Accepted,
    ConfirmationPending,
    Authenticated,
    FundingRequestPending,
    NonValid,
    Valid,
    Deleting
 };

///////////////////////////
// CLASS SECTION         //
///////////////////////////
/** \brief C1_1_1_CreateProfile is main class for CreateProfile function -- see DFD for more detail
 *
 *
 */
class C1_1_Profile
{
public:
    C1_1_Profile(void);
    ~C1_1_Profile(void);
    lifeCycleState state;

    std::string guid()
    {
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        std::string strguid = boost::to_upper_copy(boost::uuids::to_string(uuid));
        boost::algorithm::erase_all(strguid,"-");
        return strguid;
    }

    bool ConnectToWebserver(std::string strurl,void (*pt2Function)(void* pParent));
    bool ConnectToWebserver(std::string strurl);
    bool WaitForReceivingUnknownDataframe(long milliseconds); // will time out after parameter milliseconds
    bool WaitForReceivingDataframe(long milliseconds); // will time out after parameter milliseconds
    bool WaitForCreateProfile(long milliseconds); // will time out after parameter milliseconds
    bool WaitForLoginProfile(long milliseconds); // will time out after parameter milliseconds
    bool WaitForProfileLog(long milliseconds); // will time out after parameter milliseconds
    bool WaitForFetchProfile(long milliseconds); // will time out after parameter milliseconds
    bool WaitForCompletion(); // will join internal thread
    bool StopHandling(); // sends stop dataframe and close down
/// DFD streams
    bool fn116_LoginProfile(LoginProfileRequest datastream, LoginProfileRequestResponse &response);
    bool fn1161_Authentication(LoginProfileRequest &datastream, AuthenticationResponse &response);
    bool fn11611_FetchProfile(LoginProfileRequest &datastream, std::vector<Elements> &record_value, FetchProfileResponse &response);
    bool fn11612_MatchPassword(ProfilePasswordMatchRequest datastream, std::vector<Elements> record_value, MatchPasswordResponse &response);
    bool fn1162_AccountValidation(AccountValidationRequest datastream, AccountValidationResponse &response);
    bool fn1163_UpdateProfileStatus(UpdateProfileStatusRequest datastream, UpdateProfileStatusResponse &response);
    bool fn1164_WriteResponseInLogFile(WriteLogRequest datastream, WriteLogResponse &response);
    bool fn111_CreateProfile(CreateNewProfileInfo datastream, CreateProfileRequestResponse &response);
    bool fn118_FetchProfile(FetchProfileInfo datastream, FetchProfileRequestResponse &response); ///TODO: consider redesign based on fn11611_FetchProfile
    bool fn1111_HandleProfileInfo(CreateNewProfileInfo datastream);
    bool fn1112_CheckForProfile(ProfileInfo datastream);

    bool fn1113_SaveProfile(CreateNewProfileInfo datastream);

 /*   bool fn111x1_CreateTemporaryProfile(CreateNewProfileInfo);
    bool fn111x2_CheckForConflicts(CreateNewProfileInfo);
    bool fn111x3_CreateEntity(CreateNewProfileInfo);
    bool fn111x4_SendConfirmationRequest(CreateNewProfileInfo);
    bool fn111x5_Authenticate(CreateNewProfileInfo);
    bool fn111x6_CleanupAndEraseProfile(CreateNewProfileInfo);
    bool fn111x7_SendFundingRequest(CreateNewProfileInfo);
    bool fn111x8_ApplyFunding(CreateNewProfileInfo);
    bool fn111x9_CreateAccount(CreateNewProfileInfo);
*/
    bool fn119_HandleProfileLog(ProfileLogRequest profileLogReq, ProfileLogResponse &response);
    bool fn1191_AddToLog(unsigned short iTransID, ProfileLogMessage profileLogMsg, ProfileLogResponse &response);

    bool SendBinarydataframe(unsigned char *pCompressedData,unsigned int sizeofCompressedData);
    bool SendDataframe(wsclient::dataframe frame);
    bool SendDataframe(websocket::dataframe frame);

    class wsclient::CDED trashbucketDED;
    std::vector<char> trashdata;

#ifndef __DEBUG__
private:
#endif
////TODO: databasecontrol is taking over so make sure lowlevel methods are moved to that class
//    bool AppendRecordToTOASTXmlFile(bool bFlush, std::ostream & os,unsigned char* pCompressedData, unsigned int sizeofCompressedData ); // bFlush=true will cause file to be written
//    bool AppendElementToToastXmlFile(bool bFlush,unsigned long& aiid, std::ostream & os, std::string Profile_chunk_id, std::string strElementDataInHex );
//    bool AppendElementToToastXmlFile(bool bFlush,unsigned long& aiid, std::ostream & os, std::string Profile_chunk_id, std::vector<unsigned char>& iterator_data_in_buf );
//    bool FetchFileAsHex(std::string strfilepath, std::string &strFileDataInHex);
//    std::ifstream::pos_type filesize(const char* filename);
//    bool FetchFileAsVector(std::string strfilepath, std::vector<unsigned char> &FileDataBytesInVector);
/// Handle incomming dataframes

    bool HandleDataframe_Response(wsclient::CDED* pDED);
    bool HandleDFDRequest_1_1_1_CreateProfile(std::unique_ptr<CDataEncoder> , std::string, unsigned short);
    bool HandleDFDRequest_1_1_8_FetchProfile(std::unique_ptr<CDataEncoder> &);
private:
    boost::mutex mtxWait;
    boost::condition cndSignal;
    boost::mutex mtxReceiveDataframeWait;
    boost::condition cndSignalDataframeReceived;
    boost::mutex mtxReceiveUnknownDataframeWait;
    boost::condition cndSignalUnknownDataframeReceived;
    boost::mutex mtxCreateProfileWait;
    boost::condition cndSignalProfileCreated;
    boost::mutex mtxFetchProfileWait;
    boost::condition cndSignalProfileFetched;
    boost::mutex mtxLoginProfileWait;
    boost::condition cndSignalProfileLoggedin;
    boost::mutex mtxProfileLogWait;
    boost::condition cndSignalLogSaved;
    wsclient::ClientWebSocket::pointer m_ws; // connection to webserver

};

static void outscope_to_inscope_response(void *pParent) /// used for callback function to call back into c++ object (inscope)
{
    if ((wsclient::CDED*)pParent == (wsclient::CDED*)0)
    {
        printf(">>> ERROR null pointer received\n");
        return;
    }

    wsclient::CDED* pDED = (wsclient::CDED*)pParent;
    if ((C1_1_Profile*)pDED->pOwner == (C1_1_Profile*)0)
    {
        printf(">>> ERROR NOT VALID DED object received -- could be a standard text_frame (%s), it should be a binary_frame\n",(char*)pParent);
    }
    else
    {
        /// Call handlind dataframe back inside the caller object
        ((C1_1_Profile*)pDED->pOwner)->HandleDataframe_Response((wsclient::CDED*) pDED);
    }
}

