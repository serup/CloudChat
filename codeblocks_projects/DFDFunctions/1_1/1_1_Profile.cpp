/*
 * 1_1_Profile.cpp
 */


/*
 * THIS CLASS HANDLES ALL FUNCTIONS OF LEVEL 1.1 :
    1_1_1 CreateProfile
    1_1_2 EditProfile
    1_1_3 DeleteProfile
    1_1_4 AddMembership
    1_1_5_RemoveMembership
  ? 1_1_5 ModifyMembership
    1_1_6 LoginProfile
    1_1_7 LogoutProfile
    1_1_8 FetchProfile
    1_1_9_HandleProfileLog
 *
 */

#include "1_1_Profile.hpp"

#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include "md5.h"
#include "base64.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
using namespace std;
using namespace boost;

//////////////////////////////////////////
// howto compile :
// use makefile_cygwin.mak from inside cygwin terminal :
// use makefile_ubuntu.mak from linux prompt
//
// ex.
// make -f makefile_cygwin.mak
// make -f makefile_ubuntu.mak
//////////////////////////////////////////


C1_1_Profile::C1_1_Profile(void)
{
    m_ws = 0;
}

C1_1_Profile::~C1_1_Profile(void)
{
    if(m_ws)
    {
        m_ws->close();
        m_ws->join();
        delete m_ws;
        m_ws = 0;
    }
}


bool C1_1_Profile::ConnectToWebserver(std::string strurl)
{
    return ConnectToWebserver(strurl,&outscope_to_inscope_response);
}

/** \brief Connect to webserver
 *
 * \param strurl = "ws://<hostip>:<hostport>"
 * \return true/false
 *
 */
bool C1_1_Profile::ConnectToWebserver(std::string strurl,void (*pt2Function)(void* pHandlerResponseFunction))
{

    /// use new webclient class to communicate with local webserver
    using wsclient::ClientWebSocket;

    boost::mutex::scoped_lock mtxWaitLock(mtxWait);
    long milliseconds = 8000; // 8 seconds wait should be enough
    boost::posix_time::time_duration wait_duration =  boost::posix_time::milliseconds(milliseconds); // http://www.boost.org/doc/libs/1_34_0/doc/html/date_time/posix_time.html
    boost::system_time const timeout=boost::get_system_time()+wait_duration; // http://www.justsoftwaresolutions.co.uk/threading/condition-variable-spurious-wakes.html

    ClientWebSocket::pointer ws = ClientWebSocket::from_url(strurl,this);

    if(ws)
    {
        m_ws = ws;
        /// Simulate dataframe with DED protocol handling connection request to server from client DFDs function
        DED_START_ENCODER(encoder_ptr);
        DED_PUT_STRUCT_START( encoder_ptr, "WSRequest" );
        DED_PUT_METHOD	( encoder_ptr, "Method",  "DFDconnect" );
        DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)88);
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)"DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "functionName", (std::string)"DFD_1.1" );
        DED_PUT_STRUCT_END( encoder_ptr, "WSRequest" );
        /// Create a binary dataframe
        wsclient::dataframe frame;
        DED_GET_WSCLIENT_DATAFRAME(encoder_ptr,frame)

        /// start thread handling response from webserver
        ws->start(pt2Function); // HandleDataframe_Response -- unless user is providing another

/*old way - DED_GET_WSCLIENT_DATAFRAME should do this
        DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData); // -----------
        /// Create a binary dataframe
        wsclient::dataframe frame;
        // //+ ---------------------------
        frame.opcode = wsclient::dataframe::operation_code::binary_frame;
        if(sizeofCompressedData==0) sizeofCompressedData = iLengthOfTotalData; // if sizeofcompresseddata is 0 then compression was not possible and size is the same as for uncompressed
        if(data_ptr == 0) return false;
        if(iLengthOfTotalData == 0) return false;
        if(pCompressedData == 0) return false;
        frame.putBinaryInPayload(pCompressedData,sizeofCompressedData); // Put DED structure in dataframe payload
        // //- ------------------------
*/
        /// put dataframe in outgoing buffer -- first test with txbuf, then with ringbuffer
        ws->insertDataframeIn_txbuf(frame);

        /// wait for a response from server to be received
        return cndSignal.timed_wait(mtxWait,timeout); // wait until response from webserver has been received and approved
    }
    return false;
}


/* ********************* */


bool C1_1_Profile::WaitForReceivingDataframe(long milliseconds)
{
    boost::mutex::scoped_lock mtxWaitLock(mtxReceiveDataframeWait);
    boost::posix_time::time_duration wait_duration =  boost::posix_time::milliseconds(milliseconds); // http://www.boost.org/doc/libs/1_34_0/doc/html/date_time/posix_time.html
    boost::system_time const timeout=boost::get_system_time()+wait_duration; // http://www.justsoftwaresolutions.co.uk/threading/condition-variable-spurious-wakes.html
    return cndSignalDataframeReceived.timed_wait(mtxReceiveDataframeWait,timeout); // wait until response from webserver has been received and approved
}

bool C1_1_Profile::WaitForReceivingUnknownDataframe(long milliseconds)
{
    boost::mutex::scoped_lock mtxWaitLock(mtxReceiveUnknownDataframeWait);
    boost::posix_time::time_duration wait_duration =  boost::posix_time::milliseconds(milliseconds); // http://www.boost.org/doc/libs/1_34_0/doc/html/date_time/posix_time.html
    boost::system_time const timeout=boost::get_system_time()+wait_duration; // http://www.justsoftwaresolutions.co.uk/threading/condition-variable-spurious-wakes.html
    return cndSignalUnknownDataframeReceived.timed_wait(mtxReceiveUnknownDataframeWait,timeout); // wait until response from webserver has been received and approved
}

bool C1_1_Profile::WaitForCompletion()
{
    if((wsclient::ClientWebSocket::pointer)m_ws == 0) return false;
    else
    {
        ((wsclient::ClientWebSocket::pointer)m_ws)->join();
    }
    return true;
}

bool C1_1_Profile::WaitForCreateProfile(long milliseconds)
{
    boost::mutex::scoped_lock mtxWaitLock(mtxCreateProfileWait);
    boost::posix_time::time_duration wait_duration =  boost::posix_time::milliseconds(milliseconds); // http://www.boost.org/doc/libs/1_34_0/doc/html/date_time/posix_time.html
    boost::system_time const timeout=boost::get_system_time()+wait_duration; // http://www.justsoftwaresolutions.co.uk/threading/condition-variable-spurious-wakes.html
    return cndSignalProfileCreated.timed_wait(mtxCreateProfileWait,timeout); // wait until response from webserver has been received and approved
}

bool C1_1_Profile::WaitForFetchProfile(long milliseconds)
{
    boost::mutex::scoped_lock mtxWaitLock(mtxFetchProfileWait);
    boost::posix_time::time_duration wait_duration =  boost::posix_time::milliseconds(milliseconds); // http://www.boost.org/doc/libs/1_34_0/doc/html/date_time/posix_time.html
    boost::system_time const timeout=boost::get_system_time()+wait_duration; // http://www.justsoftwaresolutions.co.uk/threading/condition-variable-spurious-wakes.html
    return cndSignalProfileFetched.timed_wait(mtxFetchProfileWait,timeout); // wait until response from webserver has been received and approved
}

bool C1_1_Profile::WaitForLoginProfile(long milliseconds)
{
    boost::mutex::scoped_lock mtxWaitLock(mtxLoginProfileWait);
    boost::posix_time::time_duration wait_duration =  boost::posix_time::milliseconds(milliseconds); // http://www.boost.org/doc/libs/1_34_0/doc/html/date_time/posix_time.html
    boost::system_time const timeout=boost::get_system_time()+wait_duration; // http://www.justsoftwaresolutions.co.uk/threading/condition-variable-spurious-wakes.html
    return cndSignalProfileLoggedin.timed_wait(mtxLoginProfileWait,timeout); // wait until response from webserver has been received and approved
}

bool C1_1_Profile::WaitForProfileLog(long milliseconds)
{
    boost::mutex::scoped_lock mtxWaitLock(mtxProfileLogWait);
    boost::posix_time::time_duration wait_duration =  boost::posix_time::milliseconds(milliseconds); // http://www.boost.org/doc/libs/1_34_0/doc/html/date_time/posix_time.html
    boost::system_time const timeout=boost::get_system_time()+wait_duration; // http://www.justsoftwaresolutions.co.uk/threading/condition-variable-spurious-wakes.html
    return cndSignalLogSaved.timed_wait(mtxProfileLogWait,timeout); // wait until response from webserver has been received and approved
}
/* ********************* */


bool C1_1_Profile::StopHandling()
{
    if(m_ws)
    {
        m_ws->close(); // send close dataframe, stops internal threads and socket handling, when the stop dataframe is being handled
        return true;
    }
    return false;
}

bool C1_1_Profile::SendBinarydataframe(unsigned char *pCompressedData,unsigned int sizeofCompressedData)
{
    bool bResult=false;

    /// Create a binary dataframe
    wsclient::dataframe frame;
    frame.opcode = wsclient::dataframe::operation_code::binary_frame;

    assert(sizeofCompressedData != 0);
    assert(pCompressedData != 0);

    frame.putBinaryInPayload(pCompressedData,sizeofCompressedData); // Put DED structure in dataframe payload

    /// put dataframe in outgoing buffer -- first test with txbuf, then with ringbuffer
    if(m_ws)
        bResult = m_ws->insertDataframeIn_txbuf(frame);
    else
        return bResult; // no socket connection with server, thus not capable of sending dataframe

    return bResult;
}

bool C1_1_Profile::SendDataframe(wsclient::dataframe frame)
{
    bool bResult=false;

    /// put dataframe in outgoing buffer -- first test with txbuf, then with ringbuffer
    if(m_ws)
        bResult = m_ws->insertDataframeIn_txbuf(frame);
    else
        return bResult; // no socket connection with server, thus not capable of sending dataframe

    return bResult;
}

bool C1_1_Profile::SendDataframe(websocket::dataframe frame)
{
    bool bResult=false;

    /// Create a binary dataframe
    wsclient::dataframe _frame;
    _frame.opcode = wsclient::dataframe::operation_code::binary_frame;

    assert(frame.payload.size() != 0);
    assert((unsigned char*)&frame.payload[0] != 0);

    _frame.putBinaryInPayload((unsigned char*)&frame.payload[0],frame.payload.size()); // Put DED structure in dataframe payload
    /// put dataframe in outgoing buffer -- first test with txbuf, then with ringbuffer
    if(m_ws)
        bResult = m_ws->insertDataframeIn_txbuf(_frame);
    else
        return bResult; // no socket connection with server, thus not capable of sending dataframe

    return bResult;
}

/* ********************* */


/** \brief HandleDataframe_Response - it is called by internal callback function outscope_to_inscope_response
 *
 * \param wsclient::CDED* pDED
 * \return true/false
 *
 */
bool C1_1_Profile::HandleDataframe_Response(wsclient::CDED* pDED)
{
    bool bDecoded=false;
    std::string strMethod="";
    std::string strProtocolTypeID="";
    std::string strFunctionName="";
    std::string strStatus="";
    unsigned short iTransID=0;
//   bool bValue=false;

    std::cout << "[DFD_1.1][HandleDataframe_Response] dataframe received " << std::endl;

    if (pDED == 0)
    {
        printf(">>> ERROR null pointer received\n");
        return false;
    }

    //wsclient::CDED* pDED = (wsclient::CDED*)pParent;
    if (pDED->pOwner == 0)
    {
        printf(">>> ERROR NOT VALID DED object received -- could be a standard text_frame (%s), it should be a binary_frame\n",(char*)pDED);
    }
    else
    {
        pDED->status = "DED not yet inside";

        DED_PUT_DATA_IN_DECODER(decoder_ptr,(unsigned char*)pDED->pDEDarray,pDED->sizeofDED);
        //assert(decoder_ptr != 0);
        if(decoder_ptr == 0)
        {
            std::cout << "[DFD_1.1][HandleDataframe_Response] ERROR - DECODER - dataframe can not be decoded" << std::endl;
            return false;
        }

        /// decode data ...
        /// Handle WSResponse
        if( DED_GET_STRUCT_START( decoder_ptr, "WSResponse" ) )
        {
            /// Handle Connect to webserver response
            if( DED_GET_METHOD( decoder_ptr, "Method", strMethod ) && strMethod==(std::string)"DFDconnect")
            {
                if( DED_GET_USHORT( decoder_ptr, "TransID", iTransID) && iTransID == (unsigned short)88 &&
                        DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID ) && strProtocolTypeID == (std::string)"DED1.00.00" )
                {
                    if( DED_GET_STDSTRING( decoder_ptr, "functionName", strFunctionName ) && strFunctionName == (std::string)"DFD_1.1" &&
                            DED_GET_STDSTRING( decoder_ptr, "status", strStatus ) && strStatus == (std::string)"ACCEPTED" &&
                            DED_GET_STRUCT_END( decoder_ptr, "WSResponse" ))
                    {
                        bDecoded=true;
                        std::string tmp("WebServer connection response = ");
                        pDED->status = tmp + strStatus;

                        cndSignal.notify_one(); // signal that connection is made
                    }
                    else
                    {
                        pDED->status = (std::string)"WARNING: NO legal DED in dataframe payload";
                        printf("%s\n",pDED->status.c_str());
                        bDecoded=false;
                    }
                }
            }
        }
        else if (DED_GET_STRUCT_START( decoder_ptr, "DFDRequest" ))
        {
            /// incomming requests to this DFD function
            std::cout << "[DFD_1.1][DFDRequest] " << std::endl;

            /// CreateProfile request
            if(  DED_GET_METHOD( decoder_ptr, "Method", strMethod ) && strMethod==(std::string)"1_1_1_CreateProfile")
            {

                if(  DED_GET_USHORT( decoder_ptr, "TransID", iTransID) && /// iTransID should be used in a dataframe reply to this received dataframe
                        DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID ) && strProtocolTypeID == (std::string)"DED1.00.00" )
                {
                    /// Handle version DED1.00.00 for CreateProfile
                    std::string strDestination=(std::string)"";
                    std::string strSource=(std::string)"";
                    std::string strStartRequest=(std::string)"";
                    std::string strStartRecord=(std::string)"";
                    std::string strEndRecord=(std::string)"";
                    std::string strEndRequest=(std::string)"";
                    std::string strStartToast=(std::string)"";
                    std::string strEndToast=(std::string)"";

                    if(  DED_GET_STDSTRING( decoder_ptr, "dest", strDestination ) && strDestination == (std::string)"DFD_1.1" &&
                            DED_GET_STDSTRING( decoder_ptr, "src", strSource ) &&
                            DED_GET_STDSTRING( decoder_ptr, "STARTrequest", strStartRequest ) && strStartRequest == (std::string)"EmployeeRequest" &&
                            DED_GET_STDSTRING( decoder_ptr, "STARTrecord", strStartRecord ) && strStartRecord == (std::string)"record" )
                    {
                        /// Take Profile info and transfer to 1.1.1 Create Profile
                        /// DFD stream "Create New Profile Info" -> "Create Profile 1.1.1"

                        CreateNewProfileInfo newProfileInfo;
                        newProfileInfo.iTransID = iTransID;
                        newProfileInfo.strSource = strSource;

                        if (DED_GET_STDSTRING( decoder_ptr, "profileID", newProfileInfo.strProfileID ) &&
                        DED_GET_STDSTRING( decoder_ptr, "profileName", newProfileInfo.strProfileName ) &&
                        DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", newProfileInfo.strProtocolTypeID ) &&
                        DED_GET_STDSTRING( decoder_ptr, "sizeofProfileData", newProfileInfo.strSizeofProfileData ) &&
                        DED_GET_STDSTRING( decoder_ptr, "profile_chunk_id", newProfileInfo.strProfile_chunk_id ) &&
                        DED_GET_STDSTRING( decoder_ptr, "AccountStatus", newProfileInfo.strAccountStatus ) &&
                        DED_GET_STDSTRING( decoder_ptr, "ExpireDate", newProfileInfo.strSubscriptionExpireDate ) &&
                        DED_GET_STDSTRING( decoder_ptr, "ProfileStatus", newProfileInfo.strProfileStatus ))
                        {
                            bDecoded=true;
                            ///+ put toast part in structure, if it exists
                            DED_GET_TOAST( decoder_ptr, "profile", newProfileInfo.vecElements);
                            ///-
                            cndSignalDataframeReceived.notify_one();
                            CreateProfileRequestResponse response;
                            /// transfer to bool fn111_CreateProfile(CreateNewProfileInfo datastream); as a datastream
                            bDecoded=fn111_CreateProfile(newProfileInfo, response);
                            if(bDecoded && response.enumresp != CreateProfileRequestResponse::enumResponse::error){
                                cndSignalProfileCreated.notify_one();
                                std::cout << "[1_1_1_CreateProfile] Request was parsed valid "<< "\n";
                            }
                            /// send response dataframe back
                            bDecoded = SendDataframe(response.frame);
                        }
                        else
                        {
                            std::cout << "[1_1_1_CreateProfile] Error: dataframe was NOT valid "<< "\n";
                        }
                    }
                    else
                    {
                        bDecoded=false;
                    }
                }
            }
            else if (strMethod==(std::string)"1_1_2_EditProfile")
            { /// modify elements in profile
                std::cout << "[DFD_1.1][DFDRequest:1_1_2_EditProfile] " << std::endl;
                if(  DED_GET_USHORT( decoder_ptr, "TransID", iTransID) && /// iTransID should be used in a dataframe reply to this received dataframe
                        DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID ) && strProtocolTypeID == (std::string)"DED1.00.00" )
                {
                    std::string strDestination=(std::string)"";
                    std::string strSource=(std::string)"";
                    if(  DED_GET_STDSTRING( decoder_ptr, "dest", strDestination ) && strDestination == (std::string)"DFD_1.1" &&
                            DED_GET_STDSTRING( decoder_ptr, "src", strSource ) )
                    {
                        std::string strProfileID = (std::string)"";
                        std::string strUsername  = (std::string)"";
                        std::string strPassword  = (std::string)"";
                        if(  DED_GET_STDSTRING( decoder_ptr, "profileID", strProfileID ) &&
                            DED_GET_STDSTRING( decoder_ptr, "username", strUsername ) && DED_GET_STDSTRING( decoder_ptr, "password", strPassword ) )
                        {
                            std::cout << "[DFD_1.1][DFDRequest:1_1_2_EditProfile] validate profile... " << std::endl;
                            LoginProfileRequest newLoginProfileRequest;
                            newLoginProfileRequest.iTransID = iTransID;
                            newLoginProfileRequest.strSource = strSource;
                            newLoginProfileRequest.strProfileUsername = strUsername;
                            newLoginProfileRequest.strProfilePassword = strPassword;
                            AuthenticationResponse response;

                            bDecoded = fn1161_Authentication(newLoginProfileRequest,response);
                            if(bDecoded == true)
                            {
                                std::string strStartupdate = (std::string)"";
                                std::string strEndupdate  = (std::string)"";
                                std::vector<Elements> vecElements;
                                std::cout << "[DFD_1.1][DFDRequest:1_1_2_EditProfile] validate profile - SUCCESS " << std::endl;
                                if(  DED_GET_STDSTRING( decoder_ptr, "STARTupdate", strStartupdate ) )
                                {
                                    DED_GET_TOAST( decoder_ptr, "profile", vecElements);
                                    if(vecElements.size() <= 0)
                                    {
                                            std::cout << "[DFD_1.1][DFDRequest:1_1_2_EditProfile] ERROR - NO elements in toast area in dataframe" << std::endl;
                                            bDecoded = false;
                                    }
                                    else
                                    {
                                        if( DED_GET_STDSTRING( decoder_ptr, "ENDupdate", strEndupdate ) )
                                        {
                                            std::cout << "[DFD_1.1][DFDRequest:1_1_2_EditProfile] STARTupdate -> toast elements -> ENDupdate - read from received dataframe " << std::endl;

                                            /// now take each new value and store in database file
                                            std::string EntityName     = "Profile";
                                            std::string EntityFileName = strProfileID;
                                            CDatabaseControl CDbCtrl;
                                            std::vector<Elements> record_value;
                                            /// use ftgt to fetch the entity file
                                            bDecoded = CDbCtrl.ftgt( (std::string)EntityName, (std::string)EntityFileName, record_value );
                                            if(bDecoded==false)
                                                std::cout << "[1_1_2_EditProfile] ERROR - could not ftgt entityfile : " << EntityFileName << std::endl;
                                            else
                                            {
                                                bDecoded=false;
                                                /// update entity attribut in entity file
                                                BOOST_FOREACH(Elements f, vecElements)
                                                {
                                                    ///found element to update
                                                    std::string strValue(f.ElementData.begin(),f.ElementData.end()); // test
                                                    bDecoded = CDbCtrl.update_element_value(record_value, f.strElementID, f.ElementData);
                                                    if(bDecoded==false)
                                                        std::cout << "[1_1_2_EditProfile] ERROR - could NOT update_element_value : " << f.strElementID << std::endl;
                                                }
                                                ///update the entity file
                                                if(bDecoded==true)
                                                    bDecoded = CDbCtrl.put((std::string)EntityName,(std::string)EntityFileName,record_value);
                                                if(bDecoded==false)
                                                    std::cout << "[1_1_2_EditProfile] ERROR - could NOT update entityfile after change : " << EntityFileName << std::endl;
                                            }
                                        }
                                        else
                                        {
                                            std::cout << "[DFD_1.1][DFDRequest:1_1_2_EditProfile] ERROR ENDtoast / ENDupdate - MISSING  " << std::endl;
                                            bDecoded = false;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                std::cout << "[DFD_1.1][DFDRequest:1_1_2_EditProfile] validate profile - ACCESS DENIED " << std::endl;

                            }
                        }
                    }
                }
            }
            else if (strMethod==(std::string)"1_1_6_LoginProfile")
            {
                std::cout << "[DFD_1.1][DFDRequest:1_1_6_LoginProfile] " << std::endl;
                /// LoginProfile request
                if(  DED_GET_USHORT( decoder_ptr, "TransID", iTransID) && /// iTransID should be used in a dataframe reply to this received dataframe
                        DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID ) && strProtocolTypeID == (std::string)"DED1.00.00" )
                {
                    /// Handle version DED1.00.00 for LoginProfile
                    std::string strDestination=(std::string)"";
                    std::string strSource=(std::string)"";
                    std::string strStartRequest=(std::string)"";
                    std::string strStartDatastream=(std::string)"";
                    std::string strEndDatastream=(std::string)"";
                    std::string strEndRequest=(std::string)"";

                    if(  DED_GET_STDSTRING( decoder_ptr, "dest", strDestination ) && strDestination == (std::string)"DFD_1.1" &&
                            DED_GET_STDSTRING( decoder_ptr, "src", strSource ) &&
                            DED_GET_STDSTRING( decoder_ptr, "STARTrequest", strStartRequest ) && strStartRequest == (std::string)"LoginProfileRequest" &&
                            DED_GET_STDSTRING( decoder_ptr, "STARTDATAstream", strStartDatastream ) && strStartDatastream == (std::string)"116" ) /// TODO: add datadictionary datasteam unique id
                    {
                        /// Take LoginProfile request and transfer to 1.1.6 Login Profile
                        /// DFD stream "Login Profile Request" -> "Login Profile 1.1.6"

                        LoginProfileRequest newLoginProfileRequest;
                        newLoginProfileRequest.iTransID = iTransID;
                        newLoginProfileRequest.strSource = strSource;
                        if (DED_GET_STDSTRING( decoder_ptr, "profileID", newLoginProfileRequest.strProfileID ) &&
                                DED_GET_STDSTRING( decoder_ptr, "username", newLoginProfileRequest.strProfileUsername ) &&
                                DED_GET_STDSTRING( decoder_ptr, "password", newLoginProfileRequest.strProfilePassword ))
                        {
                            if (DED_GET_STDSTRING( decoder_ptr, "ENDDATAstream", strEndDatastream ) && strEndDatastream == (std::string)"116" &&
                                    DED_GET_STDSTRING( decoder_ptr, "ENDrequest", strEndRequest ) && strEndRequest == (std::string)"LoginProfileRequest"  )
                                bDecoded=true;

                            cndSignalDataframeReceived.notify_one();
                            LoginProfileRequestResponse response;
                            /// transfer to bool fn116_LoginProfile(NewLoginProfileInfo datastream); as a datastream
                            bDecoded=fn116_LoginProfile(newLoginProfileRequest, response);
                            if(bDecoded && response.enumresp != LoginProfileRequestResponse::enumResponse::error)
                                cndSignalProfileLoggedin.notify_one();
                            /// send response dataframe back
                            bDecoded = SendDataframe(response.frame);

                        }
                    }
                }
            }
            else if (strMethod==(std::string)"1_1_8_FetchProfile")
            {
                  bDecoded = HandleDFDRequest_1_1_8_FetchProfile(decoder_ptr);
            }
            else if (strMethod==(std::string)"1_1_9_HandleProfileLog")
            {
                /// Handle Profile Log request
                if(  DED_GET_USHORT( decoder_ptr, "TransID", iTransID) && /// iTransID should be used in a dataframe reply to this received dataframe
                        DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID ) && strProtocolTypeID == (std::string)"DED1.00.00" )
                {
                    /// Handle version DED1.00.00
                    std::string strDestination=(std::string)"";
                    std::string strSource=(std::string)"";
                    std::string strStartRequest=(std::string)"";

                    if(  DED_GET_STDSTRING( decoder_ptr, "dest", strDestination ) && strDestination == (std::string)"DFD_1.1" &&
                            DED_GET_STDSTRING( decoder_ptr, "src", strSource ) &&
                            DED_GET_STDSTRING( decoder_ptr, "STARTrequest", strStartRequest ) && strStartRequest == (std::string)"ProfileLogRequest" )
                    {
                        /// Transfer to 1.1.9 Handle Profile Log
                        /// DFD stream "Profile Log Request" -> "Handle Profile Log 1.1.9"

///TODO: parse dataframe and put values in below structure
                        ProfileLogRequest plogreq;
                        plogreq.eLogRequest = ProfileLogRequest::enumLogRequest::AddToLog;
                        plogreq.iTransID = 88;
                        plogreq.strProfileID = "";
                        plogreq.profileLogMsg.LogEntryCounter = ""; ///TODO: use datetime count as counter
                        plogreq.profileLogMsg.protocolTypeID = "";
                        plogreq.profileLogMsg.profileID = "";
                        plogreq.profileLogMsg.src = "serup"; // unique id for connection of source
                        plogreq.profileLogMsg.dest = "clientapp"; // unique id for connection of client
                        plogreq.profileLogMsg.srcUsername = "serup";
                        plogreq.profileLogMsg.destUsername = "clientapp";
                        plogreq.profileLogMsg.showdelivery = "true";
                        plogreq.profileLogMsg.timeReceived = "";
                        plogreq.profileLogMsg.timeSend = "";
                        plogreq.profileLogMsg.delivered = "Tue, 15 Apr 2014 10:28:09 GMT";
                        plogreq.profileLogMsg.message = "Hello world, this is a test of writing into the history log";
                        plogreq.profileLogMsg.acceptedIndex = "2";
                        plogreq.profileLogMsg.history_chunk_id = "";

                        bDecoded=true;
                        cndSignalDataframeReceived.notify_one();

                        ProfileLogResponse response;
                        bDecoded = fn119_HandleProfileLog(plogreq, response);
                        if(bDecoded && response.enumresp != ProfileLogResponse::enumResponse::error)
                            cndSignalLogSaved.notify_one();
                        /// send response dataframe back
                        bDecoded = SendDataframe(response.frame);

                    }
                }
            }
        }
        else
        {
            bDecoded=true; // since data was received nothing is wrong with connection, thus stays true until fx. an exception

            /// unknown dataframe
            std::string strDestination=(std::string)"";
            std::string strSource=(std::string)"";
            DED_GET_METHOD( decoder_ptr, "Method", strMethod );
            DED_GET_USHORT( decoder_ptr, "TransID", iTransID);
            DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID );
            DED_GET_STDSTRING( decoder_ptr, "dest", strDestination );
            DED_GET_STDSTRING( decoder_ptr, "src", strSource );

            if(strDestination == (std::string)"DFD_1.1")
            {
                /// put unknown dataframe in trashbucket
                unsigned char* pBinaryData=(unsigned char*)pDED->pDEDarray;
                int sizeofBinaryData=pDED->sizeofDED;
                trashdata.erase(trashdata.begin(),trashdata.end());  // make sure only one dataframe is in the trash at any time
                std::vector<char> vec(pBinaryData, pBinaryData+sizeofBinaryData);
                std::copy(vec.begin(), vec.end(), std::back_inserter(trashdata));
                trashbucketDED.pDEDarray = &trashdata[0];
                trashbucketDED.sizeofDED = trashdata.size()-1;
                trashbucketDED.status = (std::string)"dataframe is unknown";
                /// Signal that an unknown dataframe was received
                cndSignalUnknownDataframeReceived.notify_one();
                //TODO: add a check for iTransID to see if dataframe could have been send from here -- nice to have feature
                /// dataframe was a correct dataframe and send to correct receipient, however this receipient does NOT KNOW the dataframe type
                std::cout << "[DFD_1.1] dataframe was a correct dataframe and send to correct receipient, however this receipient does NOT KNOW the dataframe type!" << std::endl;
            }
            else
                std::cout << "[DFD_1.1] dataframe was NOT a correct dataframe and was NOT send to correct receipient - infact dataframe can NOT be validated at all!" << std::endl;
        }
    }

    if(m_ws == 0) return false;
    if(bDecoded==false)
    {///TODO: find a better way to handle error dataframes - this way the DFD will disconnect itself - NOT SMART !!!!
        ///somehow server is also not working proper after such a close -- hmmm!
//        cndSignal.notify_one(); // signal to avoid deadlock
//        m_ws->close(); // send close dataframe, stops internal threads and socket handling, when the stop dataframe is being handled
//        std::cout << "closing webserver" << "\n";
        std::cout << "[webserver: HandleDataframe_Response] WARNING ! dataframe received was NOT decoded correct" << "\n";
    }
    return bDecoded;
}


/* ********************* */


bool C1_1_Profile::HandleDFDRequest_1_1_1_CreateProfile(std::unique_ptr<CDataEncoder> decoder_ptr, std::string strProtocolTypeID, unsigned short iTransID)
{
    bool bDecoded=false;

    if(  DED_GET_USHORT( decoder_ptr, "TransID", iTransID) && /// iTransID should be used in a dataframe reply to this received dataframe
            DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID ) && strProtocolTypeID == (std::string)"DED1.00.00" )
    {
        /// Handle version DED1.00.00 for CreateProfile
        std::string strDestination=(std::string)"";
        std::string strSource=(std::string)"";
        std::string strStartRequest=(std::string)"";
        std::string strStartRecord=(std::string)"";
        std::string strEndRecord=(std::string)"";
        std::string strEndRequest=(std::string)"";
        std::string strStartToast=(std::string)"";
        std::string strEndToast=(std::string)"";

        if(  DED_GET_STDSTRING( decoder_ptr, "dest", strDestination ) && strDestination == (std::string)"DFD_1.1" &&
                DED_GET_STDSTRING( decoder_ptr, "src", strSource ) &&
                DED_GET_STDSTRING( decoder_ptr, "STARTrequest", strStartRequest ) && strStartRequest == (std::string)"EmployeeRequest" &&
                DED_GET_STDSTRING( decoder_ptr, "STARTrecord", strStartRecord ) && strStartRecord == (std::string)"record" )
        {
            /// Take Profile info and transfer to 1.1.1 Create Profile
            /// DFD stream "Create New Profile Info" -> "Create Profile 1.1.1"

            CreateNewProfileInfo newProfileInfo;
            newProfileInfo.iTransID = iTransID;
            newProfileInfo.strSource = strSource;
            if (DED_GET_STDSTRING( decoder_ptr, "ProfileID", newProfileInfo.strProfileID ) &&
                    DED_GET_STDSTRING( decoder_ptr, "ProfileName", newProfileInfo.strProfileName ) &&
                    DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", newProfileInfo.strProtocolTypeID ) &&
                    DED_GET_STDSTRING( decoder_ptr, "sizeofProfileData", newProfileInfo.strSizeofProfileData ) &&
                    DED_GET_STDSTRING( decoder_ptr, "Profile_chunk_id", newProfileInfo.strProfile_chunk_id ) )
                // && DED_GET_STDSTRING( decoder_ptr, "organizationID", newProfileInfo.strOrganizationID )
            {
                bDecoded=true;
                ///+ put toast part in structure, if it exists
                if(DED_GET_STDSTRING( decoder_ptr, "STARTtoast", strStartToast ))
                {
                    std::string strtmp = (std::string) " ";
                    std::vector<unsigned char> vectmp;
                    Elements Element;
                    bool bStillVectors=true;
                    /// Toast area exists now put elements in structure
                    while(bStillVectors)
                    {
                        if(DED_GET_STDSTRING( decoder_ptr, "Profile_chunk_id", strtmp ) &&
                                DED_GET_STDVECTOR( decoder_ptr, "Profile_chunk_data", vectmp ))
                        {

                            Element.strElementID = strtmp;
                            Element.ElementData = vectmp;
                            newProfileInfo.vecElements.push_back(Element);
                        }
                        else   //DED_GET_STDSTRING( decoder_ptr, "ENDtoast", strEndToast )
                            bStillVectors=false;
                    }
                }

                cndSignalDataframeReceived.notify_one();
                CreateProfileRequestResponse response;
                /// transfer to bool fn111_CreateProfile(CreateNewProfileInfo datastream); as a datastream
                bDecoded=fn111_CreateProfile(newProfileInfo, response);
                if(bDecoded && response.enumresp != CreateProfileRequestResponse::enumResponse::error)
                    cndSignalProfileCreated.notify_one();
                /// send response dataframe back
                bDecoded = SendDataframe(response.frame);
            }
        }
        else
        {
            bDecoded=false;
        }
    }
    return bDecoded;
}


bool C1_1_Profile::HandleDFDRequest_1_1_8_FetchProfile(std::unique_ptr<CDataEncoder> &decoder_ptr)
{
    bool bDecoded=false;
    std::string strProtocolTypeID="";
    unsigned short iTransID=0;

/*//    /// Fetchprofile request
//    if(  DED_GET_USHORT( decoder_ptr, "TransID", iTransID) && /// iTransID should be used in a dataframe reply to this received dataframe
//            DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID ) && strProtocolTypeID == (std::string)"DED1.00.00" )
//    {
//        /// Handle version DED1.00.00 for FetchProfile
//        std::string strDestination=(std::string)"";
//        std::string strSource=(std::string)"";
//        std::string strStartRequest=(std::string)"";
//
//        if(  DED_GET_STDSTRING( decoder_ptr, "dest", strDestination ) && strDestination == (std::string)"DFD_1.1" &&
//                DED_GET_STDSTRING( decoder_ptr, "src", strSource ) &&
//                DED_GET_STDSTRING( decoder_ptr, "STARTrequest", strStartRequest ) && strStartRequest == (std::string)"EmployeeRequest" )
//        {
//            /// Take profile info and transfer to 1.1.8 Fetch Profile
//            /// DFD stream "Fetch Profile Info" -> "Fetch Profile 1.1.8"
//
//            FetchProfileInfo foundProfileInfo;
//            foundProfileInfo.iTransID = iTransID;
//            foundProfileInfo.strSource = strSource;
//            if (DED_GET_STDSTRING( decoder_ptr, "profileID", foundProfileInfo.strProfileID ) &&
//                DED_GET_STDSTRING( decoder_ptr, "profileName", foundProfileInfo.strProfileName ) &&
//                DED_GET_STDSTRING( decoder_ptr, "password", foundProfileInfo.strPassword) )
//            {
//                bDecoded=true;
//                cndSignalDataframeReceived.notify_one();
//
//                FetchProfileRequestResponse response;
//                /// transfer to bool fn118_FetchProfile(FetchProfileInfo datastream); as a datastream
//                bDecoded=fn118_FetchProfile(foundProfileInfo, response);
//                if(bDecoded && response.enumresp != FetchProfileRequestResponse::enumResponse::error)
//                    cndSignalProfileFetched.notify_one();
//                /// send response dataframe back
//                bDecoded = SendDataframe(response.frame);
//
//            }
//
//        }
//    }
*/

    /// Fetchprofile request
    if(  DED_GET_USHORT( decoder_ptr, "TransID", iTransID) && /// iTransID should be used in a dataframe reply to this received dataframe
            DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID ) && strProtocolTypeID == (std::string)"DED1.00.00" )
    {
        /// Handle version DED1.00.00 for FetchProfile
        std::string strDestination=(std::string)"";
        std::string strSource=(std::string)"";
        std::string strStartRequest=(std::string)"";
        std::string strStartdatastream=(std::string)"";

        if(  DED_GET_STDSTRING( decoder_ptr, "dest", strDestination ) && strDestination == (std::string)"DFD_1.1" &&
                DED_GET_STDSTRING( decoder_ptr, "src", strSource ) &&
                DED_GET_STDSTRING( decoder_ptr, "STARTrequest", strStartRequest ) && strStartRequest == (std::string)"FetchProfileRequest" &&
                DED_GET_STDSTRING( decoder_ptr, "STARTDATAstream", strStartdatastream ) && strStartdatastream == (std::string)"118" )
        {
            /// Take profile info and transfer to 1.1.8 Fetch Profile
            /// DFD stream "Fetch Profile Info" -> "Fetch Profile 1.1.8"

            FetchProfileInfo foundProfileInfo;
            foundProfileInfo.iTransID = iTransID;
            foundProfileInfo.strSource = strSource;
            if (DED_GET_STDSTRING( decoder_ptr, "profileID", foundProfileInfo.strProfileID ) &&
                DED_GET_STDSTRING( decoder_ptr, "profileName", foundProfileInfo.strProfileName ) &&
                DED_GET_STDSTRING( decoder_ptr, "password", foundProfileInfo.strPassword ))
            {
                bDecoded=true;
                cndSignalDataframeReceived.notify_one();

                FetchProfileRequestResponse response;
                /// transfer to bool fn118_FetchProfile(FetchProfileInfo datastream); as a datastream
                bDecoded=fn118_FetchProfile(foundProfileInfo, response);
                if(bDecoded && response.enumresp != FetchProfileRequestResponse::enumResponse::error)
                    cndSignalProfileFetched.notify_one();
                /// send response dataframe back
                bDecoded = SendDataframe(response.frame);

            }
        }
    }

    return bDecoded;
}


/* ********************* */

bool C1_1_Profile::fn118_FetchProfile(FetchProfileInfo datastream, FetchProfileRequestResponse &response)
{
    bool bResult=false;
    response.enumresp = FetchProfileRequestResponse::enumResponse::error;

    CDatabaseControl CDbCtrl;
    std::string realm_name = "Profile";
    std::string index_name = datastream.strProfileID;
    std::string username   = datastream.strProfileName;
    std::string password   = datastream.strPassword;
    std::vector<Elements> record_value;

    bResult = CDbCtrl.ftgt( realm_name, index_name, record_value );
    if(bResult==false)
    {
        std::cout << "[fn118_FetchProfile] WARNING: ftgt failed" << "\n";
        response.enumresp = FetchProfileRequestResponse::enumResponse::ProfileNotFoundInDatabase;

///TODO: consider making a function to fetch the profile based on username and password -- perhaps make CDbCtrl.ftgt( realmname, indexname, username, password, &recordvalue )
        std::cout << "[fn118_FetchProfile] WARNING: ftgt could not find profile -- now try to search using username and password" << "\n";
        std::vector<std::string> Entities;
        bResult = CDbCtrl.list_realm_entities( realm_name, Entities );
        /// search thru all files to find a mathing profile
        BOOST_FOREACH( std::string f_index_name, Entities )
        {
            //TODO: make a mapping file and search thru that first and if not found there, then do this and add to map file if file is not existing there, thus updating the mapping file

            record_value.clear(); // clear previous use
            bResult = CDbCtrl.ftgt( realm_name, f_index_name, record_value );
            if(bResult==true)
            {
                /// fetch the username
                Elements element;
                bResult = CDbCtrl.fetch_element(record_value,(std::string)"username", element);
                std::string _username(element.ElementData.begin(),element.ElementData.end());
                if(bResult!=false)
                {
                    /// fetch the password
                    bResult = CDbCtrl.fetch_element(record_value,(std::string)"password", element);
                    std::string _password(element.ElementData.begin(),element.ElementData.end());
                    if(bResult==true)
                    {
                        if(username == _username && password == _password)
                        {
                            std::cout << "[fn118_FetchProfile] SUCCESS: profile with correct username and password was found" << "\n";
                            bResult=true;
                            bResult = CDbCtrl.fetch_element(record_value,(std::string)"profileID", element);
                            std::string _ProfileID(element.ElementData.begin(),element.ElementData.end());
                            datastream.strProfileID = _ProfileID;
                            break; // no need to iterate further
                        }
                        else
                            bResult=false;
                    }
                }
            }
        }
    }
    if(bResult==false)
    {
        /// Fetch profile from the Hadoop system - a profile file should ONLY exist locally when active, otherwise it should be closed and stored in hadoop hdfs
        std::cout << "[fn118_FetchProfile] Profile was NOT found - now looking for it in hadoop system" << "\n";
        std::string profile_index_name = requestProfileFromHadoop("username", username);
        if(profile_index_name != "<not_found>") {
            record_value.clear();
            bResult = CDbCtrl.ftgt( realm_name, profile_index_name, record_value );
        }
    }
    if(bResult==true)
    {
        response.enumresp = FetchProfileRequestResponse::enumResponse::ProfileFoundInDatabase;
        /// fetch the account status
        Elements element;
        bResult = CDbCtrl.fetch_element(record_value,(std::string)"AccountStatus", element);
        int iAccount = boost::lexical_cast<int>( element.ElementData[0] );
        response.eAccountStatus = (FetchProfileRequestResponse::enumAccountStatus) iAccount;
        /// fetch the expire date
        if(bResult!=false)
        {
            bResult = CDbCtrl.fetch_element(record_value,(std::string)"SubscriptionExpireDate", element);
            std::string tmp(element.ElementData.begin(),element.ElementData.end());
            response.strSubscriptionExpireDate = tmp;
        }
    }
    if(bResult==true && response.eAccountStatus == FetchProfileRequestResponse::AccountValid)
    {

        bool bUpdated = extractUpdateImageUrl(datastream, record_value);
        if(!bUpdated)
            std::cout << "[fn118_FetchProfile] FAILURE when trying to handle embedded foto element" << "\n";

        /// Profile was read, now make a response "ProfileFoundInDatabase"
        response.enumresp = FetchProfileRequestResponse::enumResponse::ProfileFoundInDatabase;

        DED_START_ENCODER(encoder_ptr);
        DED_PUT_STRUCT_START( encoder_ptr, "DFDResponse" );
            DED_PUT_METHOD	        ( encoder_ptr, "Method",            "JSCGetProfile" );
            DED_PUT_USHORT	        ( encoder_ptr, "TransID",	        (unsigned short)datastream.iTransID);
            DED_PUT_STDSTRING	    ( encoder_ptr, "protocolTypeID",    (std::string)"DED1.00.00" );
            DED_PUT_STDSTRING	    ( encoder_ptr, "dest",              (std::string)datastream.strSource); // Send back to requester
            DED_PUT_STDSTRING	    ( encoder_ptr, "src",               (std::string)"DFD_1.1" );// This is HERE!!!
            DED_PUT_STDSTRING	    ( encoder_ptr, "status",            (std::string)"ProfileFoundInDatabase" );
            DED_PUT_TOAST_ELEMENTS  ( encoder_ptr, "profile",           record_value );
        DED_PUT_STRUCT_END  ( encoder_ptr, "DFDResponse" );
        DED_GET_WSCLIENT_DATAFRAME(encoder_ptr, response.frame)
        bResult=true;
    }
    else
    {
        /// Profile was NOT read, now make a response "Profile NOT Found in database"
        response.enumresp = FetchProfileRequestResponse::enumResponse::ProfileNotFoundInDatabase;

        DED_START_ENCODER(encoder_ptr);
        DED_PUT_STRUCT_START( encoder_ptr, "DFDResponse" );
            DED_PUT_METHOD	    ( encoder_ptr, "Method",            "JSCGetProfile" );
            DED_PUT_USHORT	    ( encoder_ptr, "TransID",	        (unsigned short)datastream.iTransID);
            DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID",    (std::string)"DED1.00.00" );
            DED_PUT_STDSTRING	( encoder_ptr, "dest",              (std::string)datastream.strSource); // Send back to requester
            DED_PUT_STDSTRING	( encoder_ptr, "src",               (std::string)"DFD_1.1" );// This is HERE!!!
            DED_PUT_STDSTRING	( encoder_ptr, "status",            (std::string)"ProfileNotFoundInDatabase" );
        DED_PUT_STRUCT_END  ( encoder_ptr, "DFDResponse" );
        DED_GET_WSCLIENT_DATAFRAME(encoder_ptr, response.frame)
        bResult=false;
    }

    return bResult;
}

std::string C1_1_Profile::requestProfileFromHadoop(std::string elementOfInterest, std::string elementOfInterestValue)
{
    std::string result="<not_found>";

    if(isHadoopJavaServiceAppOnline())
    {

    }

    return result;
}

bool C1_1_Profile::isHadoopJavaServiceAppOnline()
{
    bool bOnline=false;

    return bOnline;
}

/** \brief Update Image URL

* Take the embedded foto from record_value and write to image file in /var/www/img/xxxxx.jpg, then give URI to this extracted file
* instead of base64 embedded image - this is due to performance issues - it takes too long in JavaScript to receive a large base64 image inside DED
*
*  \param FetchProfileInfo datastream
*  \param std::vector<Elements> record_value
*  \return true/false
*/
bool C1_1_Profile::extractUpdateImageUrl(FetchProfileInfo datastream, std::vector<Elements> record_value)
{
    bool bResult=false;
    //+ Due to performance issues, the embedded foto is extracted and a new URI is given to client
    Elements fotoelement;
    CDatabaseControl CDbCtrl;
    bool bFound = CDbCtrl.fetch_element(record_value,(std::string)"foto", fotoelement);
    if(bFound==true) {
        std::string _strfoto(fotoelement.ElementData.begin(),fotoelement.ElementData.end());

        boost::system_time const systime=boost::get_system_time();
        std::stringstream sstream;
        sstream << systime;

        std::string filenamepath = "/var/www/img/" + datastream.strProfileID + "_" + sstream.str() + ".jpg"; // TODO: add a guid filename to newly created image -- NB! filepath should be inside /var/www/img/   -- to make sure client can access it
        bool bExtracted = extractBase64TojpgImagefile(filenamepath,_strfoto);
        if(bExtracted == true)
        {
            std::vector<unsigned char> ElementData;
            std::string imageURI = "img/" + datastream.strProfileID + "_" + sstream.str() + ".jpg";
            std::copy( imageURI.begin(), imageURI.end(), std::back_inserter(ElementData));
            CDbCtrl.update_element_value(record_value,"foto",ElementData);
            // Wait a moment, so background automatic replication can transfer image
            // INFO: fx. scp the foto to cloudchatmanager machine - it should reside in relative img/<profileid>.jpg - NB! This is necessary since no extracted data is allowed on backend.scanva.com server
            // sudo scp -r img/ vagrant@cloudchatmanager.com:/home/vagrant/.
            sleep(1); // wait seconds
            bResult = bExtracted;
        }
        else
        {
            std::cout << "[C1_1_Profile::extractUpdateImageUrl] ERROR: it was not possible to change embedded image to png file url " << "\n";
        }
    }
    //-
    return bResult;
}

/** \brief Create Profile DFD 1.1.1
 *
 * Described in systemBoundariesLevel_1_1_1.vse
 *
 * \param CreateNewProfileInfo ["Create new Profile info"]
 * \param CreateProfileRequestResponse ["Profile Already in database"|"Profile Saved in database"]
 * \return true/false
 *
 */
bool C1_1_Profile::fn111_CreateProfile(CreateNewProfileInfo datastream, CreateProfileRequestResponse &response)
{
    bool bResult=false;
    std::vector<Elements> DEDElements;
    response.enumresp = CreateProfileRequestResponse::enumResponse::error;
    CDatabaseControl DbCtrl;

    std::string EntityName = "Profile";

    DbCtrl.readDDEntityRealm(EntityName, DEDElements); /// Fetch current datadictionary specs for this entity
    DbCtrl.readDDTOASTEntityRealm(EntityName, DEDElements); /// Fetch current datadictionary specs for this entity TOAST and add it to list

    /// Helpful script to cut attributes out of DD_PROFILE and DD_PROFILE_TOAST_ATTRIBUTES files
    /// $cat DD_PROFILE.xml | grep -e '<PhysicalDataElementName>' | sed -e 's/<PhysicalDataElementName>/\"/' | sed -e 's/<\/PhysicalDataElementName>/\",/';cat DD_PROFILE_TOAST_ATTRIBUTES.xml | grep -e '<PhysicalDataElementName>' | sed -e 's/<PhysicalDataElementName>/\"/' | sed -e 's/<\/PhysicalDataElementName>/\",/'
    ///
    /// put values to elements -- NB if datadictionary changes, then this one should be changed too (use above script to help get element names) - reason for having it is to avoid data corruption when change occur
    std::vector<std::string> strAttr = { "profileID",/// this is the unique id for the file in database
                                         "profileName",
                                         "protocolTypeID",
                                         "sizeofProfileData",
                                         "profile_chunk_id",
                                         "AccountStatus",
                                         "SubscriptionExpireDate",
                                         "ProfileStatus", /// After this the TOAST area start
                                         "lifecyclestate",
                                         "username",
                                         "password",
                                         "devicelist",
                                         "firstname",
                                         "lastname",
                                         "streetname",
                                         "streetno",
                                         "postalcode",
                                         "city",
                                         "state",
                                         "country",
                                         "email",
                                         "mobilephone",
                                         "title",
                                         "about",
                                         "foto",
                                         "type",
                                         "status",
                                         "expiredate",
                                         "creditor",
                                         "terms",
                                         "subscriptions",
                                         "paymentdetails",
                                         "notificationlevel",
                                         "bankaccount",
                                         "memberships",
                                         "friendships",
                                         "presence",
                                         "location",
                                         "context",
                                         "schedule",
                                         "deviceID",
                                         "experience",
                                         "skills",
                                         "messagesboard",
                                         "eventlog",
                                         "supervisor"
                                       };  /// if elements belongs in TOAST area ; WriteEntityFile - should find out by itself and update value inside TOAST file

    /// NB! MUST follow above for every element, otherwise datacorruption will occur
    std::vector<std::string> strVecAttrValue = {datastream.strProfileID,
                                                datastream.strProfileName,
                                                datastream.strProtocolTypeID,
                                                datastream.strSizeofProfileData,
                                                datastream.strProfile_chunk_id,
                                                datastream.strAccountStatus,
                                                datastream.strSubscriptionExpireDate,
                                                datastream.strProfileStatus
                                                /// TOAST area is in datastream.vecElements and will be stored in file named in datastream.strProfile_chunk_id
                                               };


    unsigned int n=0;
    unsigned int nn = strAttr.size();
    bool bNotTOAST = true; /// set to false when toast area is reached

    BOOST_FOREACH( Elements f, DEDElements )
    {
        if( f.strElementID == datastream.vecElements[0].strElementID)
            bNotTOAST = false;
        /// iterate thru elements and add value to current specs of above given entity
        if( bNotTOAST && f.strElementID == (std::string)strAttr[n])
        {
            std::copy(strVecAttrValue[n].begin(), strVecAttrValue[n].end(), std::back_inserter(DEDElements[n].ElementData));
        }
        else
        {
            BOOST_FOREACH( Elements ff, datastream.vecElements )
            {
                /// search in TOAST area
                if(f.strElementID == ff.strElementID )
                {
                    std::copy(ff.ElementData.begin(), ff.ElementData.end(), std::back_inserter(DEDElements[n].ElementData));
                    break; /// found, so no need to proceed with this element - onto the next
                }
            }
        }
        n++;
        if(n>=nn)
            break;/// No need to continue past elements, thus exit loop
    }

    /// Write values in entity file
    bResult = DbCtrl.put(EntityName, datastream.strProfileID, DEDElements);

    if(bResult==true)
    {
        /// Profile was created, now make a response "Profile Saved in database"
        response.enumresp = CreateProfileRequestResponse::enumResponse::ProfileSavedInDatabase;
        std::string strStatus = "Profile Saved in database";
        DED_START_ENCODER(encoder_ptr);
        DED_PUT_STRUCT_START( encoder_ptr, "DFDResponse" );
        DED_PUT_METHOD	( encoder_ptr, "Method",  "CreateProfile" );
        DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)datastream.iTransID);
        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)"DED1.00.00" );
        DED_PUT_STDSTRING	( encoder_ptr, "dest", (std::string)datastream.strSource); // Send back to requester
        DED_PUT_STDSTRING	( encoder_ptr, "src", (std::string)"DFD_1.1" );// This is HERE!!!
        DED_PUT_STDSTRING	( encoder_ptr, "status", (std::string)strStatus );
        DED_PUT_STRUCT_END( encoder_ptr, "DFDResponse" );
        DED_GET_WSCLIENT_DATAFRAME(encoder_ptr, response.frame);
    }
    else
    {
        /// Profile was already in database, now make a response "Profile Already in database"
        response.enumresp = CreateProfileRequestResponse::enumResponse::ProfileAlreadyInDatabase;
    }
    return bResult;
}


bool C1_1_Profile::fn1111_HandleProfileInfo(CreateNewProfileInfo datastream)
{
    bool bResult=false;
    ProfileInfo ci;
    ci.strProfileID = datastream.strProfileID;
    ci.strProfileName = datastream.strProfileName;
    bResult = fn1112_CheckForProfile(ci);

    return bResult;
}




/** \brief Login Profile DFD 1.1.6
 *
 * Described in systemBoundariesLevel_1_1_6.vse
 *
 * \param LoginProfileRequest ["Login Profile Request"]
 * \param LoginProfileRequestResponse ["error" | "ProfileNotFoundInDatabase" | "ProfileLoggingIn"]
 * \return true/false
 *
 */
bool C1_1_Profile::fn116_LoginProfile(LoginProfileRequest datastream, LoginProfileRequestResponse &response)
{
    bool bResult=false;
    AuthenticationResponse aresp;
    AccountValidationRequest avreq;
    AccountValidationResponse avresp;
    UpdateProfileStatusRequest upreq;
    UpdateProfileStatusResponse upresp;
    //WriteLogRequest wlreq;
    //WriteLogResponse wlresp;

    bResult = fn1161_Authentication(datastream,aresp);
    if(bResult==false)
    {
        //std::cout << "[fn116_LoginProfile] WARNING: fn1161_Authentication failed" << "\n";
        response.enumresp = LoginProfileRequestResponse::enumResponse::error;
    }
    else
    {
        std::cout << "[fn116_LoginProfile] SUCCESS: Authentication was correct" << "\n";
        response.enumresp = LoginProfileRequestResponse::enumResponse::ProfileLoggingIn;
        avreq.eAccountStatus = (AccountValidationRequest::enumAccountStatus)aresp.eAccountStatus;
        avreq.strSubscriptionExpireDate = aresp.strSubscriptionExpireDate;
        bResult = fn1162_AccountValidation(avreq,avresp);
        if(bResult==false)
        {
            //std::cout << "[fn116_LoginProfile] WARNING: fn1162_AccountValidation failed" << "\n";
            response.enumresp = LoginProfileRequestResponse::enumResponse::error;
        }
        else
        {
            std::cout << "[fn116_LoginProfile] SUCCESS: Account validation was correct" << "\n";
            upreq.iTransID       = datastream.iTransID;
            upreq.strProfileID   = datastream.strProfileID;
            upreq.strProfileUsername = datastream.strProfileUsername;
            upreq.eAccountStatus = (UpdateProfileStatusRequest::enumAccountStatus) avreq.eAccountStatus;
            bResult = fn1163_UpdateProfileStatus(upreq,upresp);
            if(bResult==false)
            {
                std::cout << "[fn116_LoginProfile] WARNING: fn1163_UpdateProfileStatus failed" << "\n";
                response.enumresp = LoginProfileRequestResponse::enumResponse::error;
            }
            else
                response.enumresp = LoginProfileRequestResponse::enumResponse::ProfileLoggingIn;
        }
    }
//    bResult = fn1164_WriteResponseInLogFile(wlreq,wlresp);
//    if(bResult==false)
//    {
//        std::cout << "[fn116_LoginProfile] WARNING: fn1164_WriteResponseInLogFile failed" << "\n";
//    }


    /// Create an appropriate dataframe with response info
    /// now make a response "ProfileFoundInDatabase"
    std::string strStatus = " ";
    if(response.enumresp != LoginProfileRequestResponse::enumResponse::ProfileLoggingIn)
        strStatus = "FAILED";
    else
        strStatus = "ACCEPTED";

    DED_START_ENCODER(encoder_ptr);
    DED_PUT_STRUCT_START( encoder_ptr, "DFDResponse" );
    //+ fixed area start
    DED_PUT_METHOD	( encoder_ptr, "Method",  "1_1_6_LoginProfile" );
    DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)datastream.iTransID);
    DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)"DED1.00.00" );
    DED_PUT_STDSTRING	( encoder_ptr, "dest", (std::string)datastream.strSource); // Send back to requester
    DED_PUT_STDSTRING	( encoder_ptr, "src", (std::string)"DFD_1.1" );// This is HERE!!!
    //- fixed area end
    //+ local method dependend area start
    DED_PUT_STDSTRING	( encoder_ptr, "status", (std::string)strStatus ); // login to profile success
    //- local method dependend area end
    DED_PUT_STRUCT_END( encoder_ptr, "DFDResponse" );
    DED_GET_WSCLIENT_DATAFRAME(encoder_ptr, response.frame);

    return bResult;
}


bool C1_1_Profile::fn1161_Authentication(LoginProfileRequest &datastream, AuthenticationResponse &response)
{
    bool bResult=false;
    FetchProfileResponse fpresp;
    ProfilePasswordMatchRequest ppreq;
    MatchPasswordResponse ppresp;
    std::vector<Elements> record_value;

    bResult = fn11611_FetchProfile(datastream,record_value,fpresp);
    if(bResult==false)
    {
        //std::cout << "[fn1161_Authentication] WARNING: fn11611_FetchProfile failed" << "\n";
        response.enumresp = AuthenticationResponse::enumResponse::AuthentificationFailed;
    }
    else
    {
        std::cout << "[fn1161_Authentication] SUCCESS: profile was read " << "\n";
        ppreq.strProfileID = datastream.strProfileID;
        ppreq.strProfileUsername = datastream.strProfileUsername;
        ppreq.strProfilePassword = datastream.strProfilePassword;
        ppreq.strSource = datastream.strSource;
        bResult = fn11612_MatchPassword(ppreq, record_value, ppresp);
        if(bResult==false)
        {
            response.enumresp = AuthenticationResponse::enumResponse::AuthentificationFailed;
            std::cout << "[fn1161_Authentication] WARNING: fn11612_MatchPassword failed -- perhaps profile is the wrong profile - if you want to search for the profile, then make sure profileId is NOT existing" << "\n";
        }
        else
        {
            response.enumresp = AuthenticationResponse::enumResponse::AuthentificationSuccess;
            response.strSubscriptionExpireDate = fpresp.strSubscriptionExpireDate;
            response.eAccountStatus = (AuthenticationResponse::enumAccountStatus) fpresp.eAccountStatus;
        }
    }

    return bResult;
}


bool C1_1_Profile::fn11611_FetchProfile(LoginProfileRequest &datastream, std::vector<Elements> &record_value, FetchProfileResponse &response)
{
    bool bResult=false;
    CDatabaseControl CDbCtrl;
    std::string realm_name = "Profile";
    std::string index_name = datastream.strProfileID;
    std::string username = datastream.strProfileUsername;
    std::string password = datastream.strProfilePassword;

    bResult = CDbCtrl.ftgt( realm_name, index_name, record_value );

    if(bResult == false)
    { // search through existing profile files in database and find one that has a matching username and password
        //std::cout << "[fn11611_FetchProfile] WARNING: ftgt could not find profile -- now try to search using username and password" << "\n";
        std::vector<std::string> Entities;
        bResult = CDbCtrl.list_realm_entities( realm_name, Entities );
        /// search thru all files to find a mathing profile
        BOOST_FOREACH( std::string f_index_name, Entities )
        {
            //TODO: make a mapping file and search thru that first and if not found there, then do this and add to map file if file is not existing there, thus updating the mapping file

            // fetch profile
            record_value.clear(); // clear previous use
            bResult = CDbCtrl.ftgt( realm_name, f_index_name, record_value );

            // fetch the username and password -- if match, then take breakout of foreach
            if(bResult==true)
            {
                response.enumresp = FetchProfileResponse::enumResponse::ProfileExists;
                /// fetch the username
                Elements element;
                bResult = CDbCtrl.fetch_element(record_value,(std::string)"username", element);
                std::string _username(element.ElementData.begin(),element.ElementData.end());
                /// fetch the password
                if(bResult!=false)
                {
                    bResult = CDbCtrl.fetch_element(record_value,(std::string)"password", element);
                    std::string _password(element.ElementData.begin(),element.ElementData.end());

                    /// compare username and password
                    if(bResult==true)
                    {
                        if(username == _username && password == _password)
                        {
                            std::cout << "[fn11611_FetchProfile] SUCCESS: profile with correct username and password was found" << "\n";
                            bResult=true;
                            bResult = CDbCtrl.fetch_element(record_value,(std::string)"profileID", element);
                            std::string _ProfileID(element.ElementData.begin(),element.ElementData.end());
                            datastream.strProfileID = _ProfileID;
                            break; // no need to iterate further
                        }
                        else{
                            std::cout << "[fn11611_FetchProfile] FAILED: profile with wrong username or password !!!" << "\n";
                            bResult=false;
                        }
                    }
                }
            }
            //std::cout << " realm filename: " << f << "\n";
        }

    }
    if(bResult == false)
    {
 //       std::cout << "[fn11611_FetchProfile] WARNING: ftgt failed" << "\n";
        response.enumresp = FetchProfileResponse::enumResponse::ProfileDoNotExists;
    }
    else
    {
        response.enumresp = FetchProfileResponse::enumResponse::ProfileExists;
        /// fetch the account status
        Elements element;
        bResult = CDbCtrl.fetch_element(record_value,(std::string)"AccountStatus", element);
        int iAccount = boost::lexical_cast<int>( element.ElementData[0] );
        response.eAccountStatus = (FetchProfileResponse::enumAccountStatus) iAccount;
        /// fetch the expire date
        if(bResult!=false)
        {
            bResult = CDbCtrl.fetch_element(record_value,(std::string)"SubscriptionExpireDate", element);
            std::string tmp(element.ElementData.begin(),element.ElementData.end());
            response.strSubscriptionExpireDate = tmp;
        }
    }
    return bResult;
}

bool C1_1_Profile::fn11612_MatchPassword(ProfilePasswordMatchRequest datastream, std::vector<Elements> record_value, MatchPasswordResponse &response)
{
    bool bResult=false;
    std::vector<unsigned char> usernameElementData, passwordElementData;
    std::copy(datastream.strProfileUsername.begin(), datastream.strProfileUsername.end(), std::back_inserter(usernameElementData));
    std::copy(datastream.strProfilePassword.begin(), datastream.strProfilePassword.end(), std::back_inserter(passwordElementData));
    std::string strDataDictionaryEntry_username = "username";

    CDatabaseControl CDbCtrl;
    if(CDbCtrl.compare_element(record_value, strDataDictionaryEntry_username, usernameElementData) && CDbCtrl.compare_element(record_value, (std::string)"password", passwordElementData))
    {
        response.enumresp = MatchPasswordResponse::enumResponse::AuthentificationSuccess;
        bResult=true;
    }
    else
        response.enumresp = MatchPasswordResponse::enumResponse::PasswordDoNotMatch;

    return bResult;
}

bool C1_1_Profile::fn1162_AccountValidation(AccountValidationRequest datastream, AccountValidationResponse &response)
{
    bool bResult=false;

    if(datastream.eAccountStatus == AccountValidationRequest::enumAccountStatus::AccountValid)
    {
        /*    char* date1 = "9\\12\\2012";
            char* date2 = "6\\11\\2013";

            int day1,month1,year1;
            int day2,month2,year2;

            sscanf(date1,"%d\\%d\\%d",&day1,&month1,&year1); //reads the numbers
            sscanf(date2,"%d\\%d\\%d",&day2,&month2,&year2); //from the string

            if (year1<year2 || month1<month2 || day1<day2) //compares 2 dates
            {
                printf("date1 < date2\n");
            }
            else
            {
                printf("date1 >= date2\n");
            }

            char newdate[15];

            sprintf(newdate,"%d\\%d\\%d",13,2,1998); //make a date string from numbers
            printf("%s\n",newdate);
        */

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
        ptime timeAccount;
        ss << t;
        cout << ss.str() << endl; // ex. 2012-08-03 23:46:38

//      ss.str("2000-01-31 12:34:56");
        ss.str(datastream.strSubscriptionExpireDate);
        ss >> timeAccount;

        /*   assert(timeAccount.date().year() == 2000);
           assert(timeAccount.date().month() == 1);
           assert(timeAccount.date().day() == 31);
           assert(timeAccount.time_of_day().hours() == 12);
           assert(timeAccount.time_of_day().minutes() == 34);
           assert(timeAccount.time_of_day().seconds() == 56);
           */

        if(timeAccount>t)
            bResult=true; /// expire date is in the future

//        delete output_facet;
//        delete input_facet;
    }
    return bResult;
}
bool C1_1_Profile::fn1163_UpdateProfileStatus(UpdateProfileStatusRequest datastream, UpdateProfileStatusResponse &response)
{
    // http://stackoverflow.com/questions/11314545/casting-enum-definition-to-unsigned-int
    // http://stackoverflow.com/questions/1297385/c-enum-from-char
    bool bResult=false;
    std::string EntityName     = "Profile";
    std::string strElementID   = "ProfileStatus";
    std::string EntityFileName = datastream.strProfileID;
    std::vector<unsigned char> newElementData;

    /// update entity attribut in entity file
    response.eProfileStatus = UpdateProfileStatusResponse::enumProfileStatus::ProfileLoggedIn;
    int nValue = boost::lexical_cast<int>(response.eProfileStatus);
    unsigned char newValue = boost::lexical_cast<unsigned char>(nValue);
    newElementData.push_back(newValue);

    CDatabaseControl CDbCtrl;
    bResult = CDbCtrl.updateEntityAttribut(EntityName, EntityFileName, strElementID, newElementData );

    return bResult;
}

bool C1_1_Profile::fn1164_WriteResponseInLogFile(WriteLogRequest datastream, WriteLogResponse &response)
{
    bool bResult=false;

    return bResult;
}

//DEPRECATED -- make it use databasecontrol class
/** \brief Read Profile DFD 1.1.8.1
 *
 * \param
 * \param
 * \return
 *
 */
//bool C1_1_Profile::fn1181_ReadProfileFile(FetchProfileInfo &datastream)
//{
//    bool bResult=false;
//
//    return bResult;
//
//    /*
//       std::string strFilepath = "../../../../DataDictionary/Database/" + datastream.strProfileID + "_" + datastream.strProfileName + ".xml";
//       if ( !boost::filesystem::exists( strFilepath ) )
//       {
//           //std::cout << "Can't find my file!" << std::endl;
//           bResult=false;
//       }
//       else
//       {
//           /// Read data from file
//            std::vector<Elements> DEDElements;
//           ProfileRecord ProfileRecordResult;
//           {
//               CDatabaseControl CDbCtrl;
//               std::ifstream is (strFilepath);
//               //bResult = CDbCtrl.ReadXmlFile(is,ProfileRecordResult,"Profile");
//               bResult = CDbCtrl.ReadEntityFile((std::string)"Profile",(std::string)datastream.strProfileName,DEDElements);
//
//               //assert(bResult == true);
//           }
//           if(bResult==false)
//           {
//               std::cout << "[fn1181_ReadProfileFile] ERROR : File can not be read : " << strFilepath << std::endl;    /// no need to go further, something is wrong with the file
//               return bResult;
//           }
//
//           bResult=false;
//           BOOST_FOREACH( ProfileRecordEntry f, ProfileRecordResult )
//           {
//               if(f.DataSize > (unsigned int) 0)
//               {
//                   // take the hex converted data and unhex it before DED will decode it
//                   unsigned int sizeofCompressedDataInHex = (unsigned int)f.DataSize;
//                   unsigned char* data_in_unhexed_buf = (unsigned char*) malloc (4*sizeofCompressedDataInHex + 1);
//                   ZeroMemory(data_in_unhexed_buf,4*sizeofCompressedDataInHex+1); // make sure no garbage is inside the newly allocated space
//                   const std::vector<unsigned char> iterator_data_in_unhexed_buf(&f.Data[0],&f.Data[sizeofCompressedDataInHex]);
//                   boost::algorithm::unhex(iterator_data_in_unhexed_buf.begin(),iterator_data_in_unhexed_buf.end(), data_in_unhexed_buf);// convert the hex array to an array containing byte values
//
//                   // fetch the data area and unpack it with DED to check it
//                   DED_PUT_DATA_IN_DECODER(decoder_ptr,data_in_unhexed_buf,sizeofCompressedDataInHex);
//                   assert(decoder_ptr != 0);
//
//                   bool bDecoded=false;
//
//                   // decode data ...
//                   if( DED_GET_STRUCT_START( decoder_ptr, "record" ) &&
//                           DED_GET_STDSTRING	( decoder_ptr, "ProfileID", datastream.strProfileID ) &&
//                           DED_GET_STDSTRING	( decoder_ptr, "ProfileName", datastream.strProfileName ) &&
//                           DED_GET_STDSTRING	( decoder_ptr, "protocolTypeID", datastream.strProtocolTypeID) &&
//                           DED_GET_STDSTRING	( decoder_ptr, "sizeofProfileData", datastream.strSizeofProfileData ) &&
//                           DED_GET_STDSTRING	( decoder_ptr, "Profile_chunk_id", datastream.strProfile_chunk_id ) &&
//                           DED_GET_STDSTRING	( decoder_ptr, "organizationID", datastream.strOrganizationID ) &&
//                           DED_GET_STRUCT_END( decoder_ptr, "record" ))
//                   {
//                       bDecoded=true;
//                       //TODO: read the toast data and put it on datastream -- use ReadTOASTXmlFile
//                       //TODO: ProfileToastData structure is focused wrong -- CHANGE it, it should not know about elements - it should be neutral!!!
//                       ProfileToastData ProfileTOASTDataResult;
//                       {
//                           //    std::ifstream is ("../../../../DataDictionary/Database/TOASTs/22980574.xml");
//                           std::string strTOASTFilepath = "../../../../DataDictionary/Database/TOASTs/" + datastream.strProfile_chunk_id + ".xml";
//                           if ( boost::filesystem::exists( strTOASTFilepath ) )
//                           {
//                               std::ifstream istoast (strTOASTFilepath);
//                               bDecoded = ReadTOASTXmlFile(istoast,ProfileTOASTDataResult);
//                               Elements element;
//                               BOOST_FOREACH( Elements f, ProfileTOASTDataResult.vecElements )
//                               {
//                                   element.strElementID = f.strElementID;
//                                   element.ElementData = f.ElementData;
//                                   datastream.vecElements.push_back(element);
//                               }
//                           }
//                           else
//                               std::cout << "[DFD1.1.1: fn1181_ReadProfileFile ] Can't find TOAST file!" << std::endl;
//                       }
//                   }
//                   else
//                   {
//                       bDecoded=false;
//                   }
//                   bResult = bDecoded;
//    //                delete[] data_in_unhexed_buf;
//                   free(data_in_unhexed_buf);
//               }
//           }
//       }
//
//       return bResult;
//    */
//}


//bool C1_1_Profile::fn111x2_CheckForConflicts(CreateNewProfileInfo datastream)
//{
//    bool bResult=false;
//
//    // TODO
//    //
//    // Validate that the content of a new Profile description instance,
//    // in temporary state, does not conflict whit existing system data.
//    //
//    // for now, in this version of the, we will just chk if the username is unique.
//    // If the temporary Profile is able to pass validation, state is changed to
//    // 'Accepted' and 'CreateEntity' is called.
//    // If the temporary Profile does not pass validation, then inform the visitor.
//
//    return bResult;
//}

bool C1_1_Profile::fn1112_CheckForProfile(ProfileInfo datastream)
{
    boost::filesystem::path my_current_path( boost::filesystem::current_path() );
    std::string strFilepath = my_current_path.string() + relativeENTITIES_DATABASE_PLACE + datastream.strProfileName + ".xml"; // database file for entity
    bool bResult=true;

    if ( !boost::filesystem::exists( strFilepath ) )
    {
        //std::cout << "Can't find my file!" << std::endl;
        bResult=false;
    }

    return bResult;
}

/*
bool C1_1_Profile::fn111x3_CreateEntity(CreateNewProfileInfo datastream)
{
    bool bResult=false;

    // TODO
    //
    // Create a new Profile record in the system Database, and call 'SendConfirmationRequest'
    //
    //

    return bResult;
}

bool C1_1_Profile::fn111x4_SendConfirmationRequest(CreateNewProfileInfo datastream)
{
    bool bResult=false;

    // TODO
    //
    // Use the contact information details mobile phone or email address, and send a
    // formal confirmation request message including a unique code or a unique web-link url.
    // This will enable the Visitor to confirm his/her identity relative to this data.
    // The message is send through a SMS or Email gateway.
    // If message is dispatched succesfully, state is set to 'ConfirmationPending'
    // If message is not dispatched succesfully, secondary contact details (also mobile
    // phone or email address) will be tried. If this also fails, the state is set to 'Deleting'
    // and fn111x6_CleanupAndEraseProfile is called.

    return bResult;
}

bool C1_1_Profile::fn111x5_Authenticate(CreateNewProfileInfo datastream)
{
    bool bResult=false;

    // TODO
    //
    // This method is called through the DFD function handler from the web server or directly
    // through the socket connection via the Visitors client javascript.
    //

    return bResult;
}


bool C1_1_Profile::fn111x6_CleanupAndEraseProfile(CreateNewProfileInfo datastream)
{
    bool bResult=false;

    // TODO
    //
    // If the specific Profile instance is in 'Deleting' State, then proceed and
    // handle all external dependencies before the database entries are finally deleted.
    // (PS: Deleting might mean, transfering the Profile into our non-active Archive database)

    return bResult;
}

bool C1_1_Profile::fn111x7_SendFundingRequest(CreateNewProfileInfo datastream)
{
    bool bResult=false;

    // TODO
    //
    // Request funding from an existing Account held by another Profile, by creating a new
    // Request-message and placing it inside the specific Profile's MessageBoard (inbox)
    //

    return bResult;
}

bool C1_1_Profile::fn111x8_ApplyFunding(CreateNewProfileInfo datastream)
{
    bool bResult=false;

    // TODO
    //
    // this is called by any Account (Profile) willing to pay for the subscription.
    // The Subscription fee is drawn from the "external" or "internal" Account,
    // and state is set to Valid

    return bResult;
}

bool C1_1_Profile::fn111x9_CreateAccount(CreateNewProfileInfo datastream)
{
    bool bResult=false;

    // TODO
    //
    // The Profile is extended with its own account, and fn111x8_ApplyFunding is called
    //

    return bResult;
}
*/
bool C1_1_Profile::fn119_HandleProfileLog(ProfileLogRequest profileLogReq, ProfileLogResponse &response)
{
    bool bResult=false;
    response.enumresp = ProfileLogResponse::enumResponse::error;

    // TODO: add some validation
    //
    //

    bResult = fn1191_AddToLog(profileLogReq.iTransID,profileLogReq.profileLogMsg, response);

    return bResult;
}

bool C1_1_Profile::fn1191_AddToLog(unsigned short iTransID, ProfileLogMessage profileLogMsg, ProfileLogResponse &response)
{
    bool bResult=false;


    return bResult;
}



