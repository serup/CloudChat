#include "room.hpp"
#include "../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h"
#include "../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <boost/bind.hpp>
#include <boost/foreach.hpp> ///used for test dataframes received

/// HOWTO REMOVE UNWANTED CRLF
//sed -i 's/\r\n/\n/' websocket_server/room.cpp

namespace websocket {
    namespace applications {
        namespace chat {

            void room::join(participant_ptr participant)
            {
                participants_.insert(participant);

                //deprecated - not necessary to tell other client that a new client is connecting
                //update_participants();
                //std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
                //    boost::bind(&participant::deliver, participant, _1));

            }

            void room::leave(participant_ptr participant)
            {
                removevipparticipant(participant);
                participants_.erase(participant);
                // update_participants(); // temporary disabled to find out why a RST comes when using javascript connection with nokia mobile N700
            }

            void room::deliver(const dataframe& msg, participant_ptr source)
            {
                std::string header = "log:" + source->get_id() + ": ";

                dataframe frm;
                std::copy(header.begin(), header.end(), std::back_inserter(frm.payload));
                std::copy(msg.payload.begin(), msg.payload.end(), std::back_inserter(frm.payload));

                deliver(frm);
            }

            void room::deliverTo(const dataframe& msg, participant_ptr participant)
            {
                if (std::find(participants_.begin(), participants_.end(), participant) != participants_.end()) {
                    std::cout << "[webserver] found participant - now deliver dataframe\n";
                    message_queue individual_msgs_;
                    // transmit dataframe to participant
                    individual_msgs_.push_back(msg);
                    std::for_each(individual_msgs_.begin(), individual_msgs_.end(),
                            boost::bind(&participant::deliver, participant, _1));

                }
            }

            void room::deliver(const dataframe& msg)
            {
                recent_msgs_.push_back(msg);
                while (recent_msgs_.size() > max_recent_msgs)
                    recent_msgs_.pop_front();

                std::for_each(participants_.begin(), participants_.end(),
                    boost::bind(&participant::deliver, _1, boost::ref(msg)));
            }

            void room::update_participants()
            {
                std::string header = "connected:" + boost::lexical_cast<std::string>(participants_.size());

                dataframe frm;
                std::copy(header.begin(), header.end(), std::back_inserter(frm.payload));

                std::for_each(participants_.begin(), participants_.end(),
                    boost::bind(&participant::deliver, _1, boost::ref(frm)));
            }

            participant_ptr room::findvipparticipant(std::string& strFunctionName)
            {
                std::vector<vipparticipants*>::iterator it = std::find_if(vipparticipants_.begin(),vipparticipants_.end(),
                                                           boost::bind(&vipparticipants::dfdid_, _1) == strFunctionName); // http://stackoverflow.com/questions/1378867/using-find-if-on-stdvectorstdstring-with-bind2nd-and-stringcompare
                if(it != vipparticipants_.end())
                {
                    std::cout << "found : " << (std::string)(**it).dfdid_.c_str() << "\n";
                    return (participant_ptr)(**it).source;
                }
                return (participant_ptr)0;
            }


            bool room::removevipparticipant(std::string& strFunctionName)
            {
                std::vector<vipparticipants*>::iterator it = std::find_if(vipparticipants_.begin(),vipparticipants_.end(),
                                                           boost::bind(&vipparticipants::dfdid_, _1) == strFunctionName); // http://stackoverflow.com/questions/1378867/using-find-if-on-stdvectorstdstring-with-bind2nd-and-stringcompare
                if(it != vipparticipants_.end())
                {
                    std::cout << "removed : " << (std::string)(**it).dfdid_.c_str() << "\n";

                    vipparticipants_.erase(it);

//TODO: find a better way, if needed
//not working - trying to make it go thru all and erase all occurrencies - should only be one, however if somehow due to error there is multiple, then they should all be removed
                        // taken from Item 32 of Scott Myers' Effective STL
//                        std::vector<int> v;
//                        v.erase(remove(v.begin(), v.end(), 99), v.end());
//not working with pointer only with above integers
                    //vipparticipants_.erase(std::remove(vipparticipants_.begin(), vipparticipants_.end(), it),vipparticipants_.end()); // remove ALL occurrencies of functionname
                    return true;
                }
                return false; // could not remove from list, it does not exist in the list
            }

            bool room::removevipparticipant(participant_ptr source)
            {
                std::vector<vipparticipants*>::iterator it = std::find_if(vipparticipants_.begin(),vipparticipants_.end(),
                                                           boost::bind(&vipparticipants::source, _1) == source); // http://stackoverflow.com/questions/1378867/using-find-if-on-stdvectorstdstring-with-bind2nd-and-stringcompare
                if(it != vipparticipants_.end())
                {
                    std::cout << "removed : " << (std::string)(**it).dfdid_.c_str() << "\n";
                    vipparticipants_.erase(it);
                    return true;
                }
                return false; // could not remove from list, it does not exist in the list
            }

            void room::handleIncomming(const dataframe& msg, participant_ptr source)
            {
                bool bDecoded;
                bDecoded=false;
                std::string strMethod;
                std::string strProtocolTypeID;
                std::string strUniqueId;
                std::string strFunctionName;
                std::string strUserName;
                std::string strPassword;
                unsigned short iTransID;
                participant_ptr destination;
                dataframe frmResponse;

                /// Fetch payload from dataframe and decode the DED inside
                DED_PUT_DATA_IN_DECODER(decoder_ptr,(unsigned char*)&msg.payload[0],msg.payload.size());

                /// decode data ...
                /// Handle DFDRequests
                if( DED_GET_STRUCT_START( decoder_ptr, "WSRequest" ) )
                {
                    /// Handle connect requests from DFDs
                    if ( DED_GET_METHOD( decoder_ptr, "Method", strMethod ) && strMethod==(std::string)"DFDconnect")
                    {
                        /// Handle protocol DED1.00.00
                        if( DED_GET_USHORT( decoder_ptr, "TransID", iTransID) &&
                        DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID ) && strProtocolTypeID == (std::string)"DED1.00.00" )
                        {
                            if( DED_GET_STDSTRING( decoder_ptr, "functionName", strFunctionName ) &&
                            DED_GET_STRUCT_END( decoder_ptr, "WSRequest" ))
                            {
                                /// A connect request following protocol version DED1.00.00 has been received
                                std::cout << "[webserver] A connect request following protocol version DED1.00.00 has been received\n";

                                /// Add participant to vip list of DFD functions online
                                if(findvipparticipant(strFunctionName) == 0)
                                {
                                    vipparticipants_.push_back(new vipparticipants(source,strFunctionName));

                                    /// Create a dataframe for response to new participant
                                    dataframe wsresponse;
                                    {
                                        DED_START_ENCODER(encoder_ptr);
                                        DED_PUT_STRUCT_START( encoder_ptr, "WSResponse" );
                                        DED_PUT_METHOD	( encoder_ptr, "Method",  "DFDconnect" );
                                        DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)iTransID);
                                        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)strProtocolTypeID );
                                        DED_PUT_STDSTRING	( encoder_ptr, "functionName", (std::string)strFunctionName );
                                        DED_PUT_STDSTRING	( encoder_ptr, "status", (std::string)"ACCEPTED" );
                                        DED_PUT_STRUCT_END( encoder_ptr, "WSResponse" );
                                        DED_GET_ENCODED_DATAFRAME(encoder_ptr, wsresponse);
                                    }
                                    /*
                                    //                                        DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);
                                    //                                        /// Create a binary dataframe with above DED inside payload
                                    //                                        dataframe frame;
                                    //                                        frame.opcode = dataframe::binary_frame;
                                    //
                                    //                                        if(sizeofCompressedData==0) sizeofCompressedData = iLengthOfTotalData; // if sizeofcompresseddata is 0 then compression was not possible and size is the same as for uncompressed
                                    //                                        frame.putBinaryInPayload(pCompressedData,sizeofCompressedData); // Put DED structure in dataframe payload
                                    //
                                    //                                        /// set dataframe as response frame
                                    //                                        frmResponse=frame;
                                    */
                                    frmResponse=wsresponse;
                                    destination=source; // since reply have to go back to source, then destination becomes source
                                    bDecoded=true;
                                    std::cout << "[webserver] send WSResponse to : " << strFunctionName << "\n";
                                }
                            }
                        }
                        else
                        {
                            if(strProtocolTypeID == (std::string)"DED1.00.01")
                            {
                                /// Handling future protocol
                                /// A connect request following protocol version DED1.00.01 has been received
                            }
                            //else-if(strProtocolTypeID == (std::string)"DED1.00.02")
                            //TODO: consider sending a dataframe back with info about unknown DED version!!
                        }
                    }
                    else
                    { /// Handle JSC dataframes
                        enum _eMethod { null, JSCconnect, JSCGetProfile } eMethod;

                        if( strMethod ==(std::string)"JSCconnect" ) eMethod = JSCconnect;
                        if( strMethod ==(std::string)"JSCGetProfile" ) eMethod = JSCGetProfile;

                        switch(eMethod)
                        {
                        case JSCconnect:
                        {
                            /// Handle protocol DED1.00.00
                            if( DED_GET_USHORT( decoder_ptr, "TransID", iTransID) &&
                                    DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID ) && strProtocolTypeID == (std::string)"DED1.00.00" )
                            {
                                if( DED_GET_STDSTRING( decoder_ptr, "functionName", strFunctionName ) )
                                {
                                    /// A connect request following protocol version DED1.00.00 has been received
                                    std::cout << "[webserver] A javaclient connect request following protocol version DED1.00.00 has been received\n";

                                    /// fetch username and password
                                    if( DED_GET_STDSTRING( decoder_ptr, "username", strUserName ) && DED_GET_STDSTRING( decoder_ptr, "password", strPassword ))
                                    {
                                        /// Add participant to vip list of DFD functions online
                                        removevipparticipant(strFunctionName); /// First remove previous connection info
                                        if(findvipparticipant(strFunctionName) == 0)
                                        {
                                            vipparticipants_.push_back(new vipparticipants(source,strFunctionName));

                                            /// Send Request to DFD_1.1 and send a response if not online, if online then DFD will send a response
                                            std::string strDFD = "DFD_1.1";
                                            participant_ptr dfddest = findvipparticipant(strDFD);
                                            if(dfddest==0)
                                            {
                                                /// TODO: if DFD is NOT online then do something smart - perhaps restart it !?
                                                std::cout << "[webserver] ERROR DFD_1.1 is NOT online " << "\n";

 /*
 //TODO: MAKE WSRESPONSE PROTOCOL FOLLOW BASIC PART OF DATAFRAME PROTOCOL -- TOP FIXED PART !!!!
 */                                               /// Create a dataframe for response to new participant
                                                DED_START_ENCODER(encoder_ptr);
                                                DED_PUT_STRUCT_START( encoder_ptr, "WSResponse" );
                                                DED_PUT_METHOD	( encoder_ptr, "Method",  "JSCconnect" );
                                                DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)iTransID);
                                                DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)strProtocolTypeID );
                                                DED_PUT_STDSTRING	( encoder_ptr, "functionName", (std::string)strFunctionName );
                                                DED_PUT_STDSTRING	( encoder_ptr, "status", (std::string)"ACCEPTED" );
                                                DED_PUT_BOOL	    ( encoder_ptr, "testbool_false", false );
                                                DED_PUT_BOOL	    ( encoder_ptr, "testbool_true", true );
                                                DED_PUT_STRUCT_END( encoder_ptr, "WSResponse" );
                                                /// Create a binary dataframe with above DED inside payload
                                                dataframe tempframe;
                                                DED_GET_ENCODED_DATAFRAME(encoder_ptr, tempframe);
/*
                                                DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,ptmpCompressedData,sizeoftmpCompressedData);
                                                /// Create a binary dataframe with above DED inside payload
                                                dataframe tempframe;
                                                //tempframe.opcode = dataframe::text_frame; // javascript has issues with converting from blob to arraybuffer
                                                tempframe.opcode = dataframe::binary_frame;
                                                if(sizeoftmpCompressedData==0) sizeoftmpCompressedData = iLengthOfTotalData; // if sizeofcompresseddata is 0 then compression was not possible and size is the same as for uncompressed
                                                tempframe.putBinaryInPayload(ptmpCompressedData,sizeoftmpCompressedData); // Put DED structure in dataframe payload
*/
                                                /// set dataframe as response frame
                                                frmResponse=tempframe;
                                                destination=source; // since reply have to go back to source, then destination becomes source
                                                bDecoded=true;
                                                std::cout << "[webserver] send WSResponse to : " << strFunctionName << "\n";
                                            }
                                            else
                                            {
                                                /// JavaScript client is now connected and accepted, however now it needs to login to its profile

                                                /// Send Request Login to profile - profile will send response dataframe to JSClient with status
                                                dataframe dfRequest;
                                                DED_START_ENCODER(encoder_ptr2);
                                                DED_PUT_STRUCT_START( encoder_ptr2, "DFDRequest" );
                                                //+ fixed area start
                                                DED_PUT_METHOD	( encoder_ptr2, "Method",  "1_1_6_LoginProfile" );
                                                DED_PUT_USHORT	( encoder_ptr2, "TransID",	(unsigned short)iTransID);
                                                DED_PUT_STDSTRING	( encoder_ptr2, "protocolTypeID", (std::string)"DED1.00.00" );
                                                DED_PUT_STDSTRING	( encoder_ptr2, "dest", (std::string)"DFD_1.1" ); // destination is profile DFD
                                                DED_PUT_STDSTRING	( encoder_ptr2, "src", (std::string)strFunctionName ); /// destination is a JavaScript client
                                                //+ Profile request area start
                                                DED_PUT_STDSTRING	( encoder_ptr2, "STARTrequest", (std::string)"LoginProfileRequest" );
                                                DED_PUT_STDSTRING	( encoder_ptr2, "STARTDATAstream", (std::string)"116" ); // TODO: add datadictionary id for the datastream
                                                DED_PUT_STDSTRING	( encoder_ptr2, "profileID", (std::string)strFunctionName ); // unique id for registered profile - this is actually the name of the main profile entityfile
                                                DED_PUT_STDSTRING	( encoder_ptr2, "username", (std::string)strUserName ); // TODO: find a way to use fx. hashing of user and password
                                                DED_PUT_STDSTRING	( encoder_ptr2, "password", (std::string)strPassword );
                                                DED_PUT_STDSTRING	( encoder_ptr2, "ENDDATAstream", (std::string)"116" ); // TODO: add datadictionary id for the datastream
                                                DED_PUT_STDSTRING	( encoder_ptr2, "ENDrequest", (std::string)"LoginProfileRequest" );
                                                //- Profile request area end
                                                DED_PUT_STRUCT_END( encoder_ptr2, "DFDRequest" );
                                                DED_GET_ENCODED_DATAFRAME(encoder_ptr2, dfRequest);

                                                /// set dataframe as response frame
                                                frmResponse=dfRequest; /// this is actually a request to DFD however it will just be treated as a normal response dataframe transfer
                                                destination=dfddest;
                                                //destination=source; // since reply have to go back to source, then destination becomes source
                                                bDecoded=true;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        /// Create a dataframe for response to unknown new participant
                                        DED_START_ENCODER(encoder_ptr);
                                        DED_PUT_STRUCT_START( encoder_ptr, "WSResponse" );
                                        DED_PUT_METHOD	( encoder_ptr, "Method",  "JSCconnect" );
                                        DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)iTransID);
                                        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)strProtocolTypeID );
                                        DED_PUT_STDSTRING	( encoder_ptr, "functionName", (std::string)strFunctionName );
                                        DED_PUT_STDSTRING	( encoder_ptr, "status", (std::string)"NOT ACCEPTED" );
                                        DED_PUT_STRUCT_END( encoder_ptr, "WSResponse" );
                                        DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);

                                        /// Create a binary dataframe with above DED inside payload
                                        dataframe frame;
                                        frame.opcode = dataframe::binary_frame;
                                        if(sizeofCompressedData==0) sizeofCompressedData = iLengthOfTotalData; // if sizeofcompresseddata is 0 then compression was not possible and size is the same as for uncompressed
                                        frame.putBinaryInPayload(pCompressedData,sizeofCompressedData); // Put DED structure in dataframe payload

                                        /// set dataframe as response frame
                                        frmResponse=frame;
                                        destination=source; // since reply have to go back to source, then destination becomes source
                                        bDecoded=true;
                                        std::cout << "[webserver] send NOT ACCEPTED WSResponse to : " << strFunctionName << "\n";
                                    }
                                }
                            }
                            else
                            {
                                if(strProtocolTypeID == (std::string)"DED1.00.01")
                                {
                                    /// Handling future protocol
                                    /// A connect request following protocol version DED1.00.01 has been received
                                }
                                //else-if(strProtocolTypeID == (std::string)"DED1.00.02")
                                //TODO: consider sending a dataframe back with info about unknown DED version!!
                            }
                        }
                        break;
                        case JSCGetProfile:
                        {
                            /// Handle protocol DED1.00.00
                            if( DED_GET_USHORT( decoder_ptr, "TransID", iTransID) &&
                                    DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID ) && strProtocolTypeID == (std::string)"DED1.00.00" )
                            {
                                    if( DED_GET_STDSTRING( decoder_ptr, "uniqueId", strUniqueId ) &&
                                        DED_GET_STDSTRING( decoder_ptr, "username", strUserName ) &&
                                        DED_GET_STDSTRING( decoder_ptr, "password", strPassword ))
                                    {
                                        /// Send Request to DFD_1.1 and send a response if not online, if online then DFD will send a response
                                        std::string strDFD = "DFD_1.1";
                                        participant_ptr dfddest = findvipparticipant(strDFD);
                                        if(dfddest==0)
                                        {
                                            /// TODO: if DFD is NOT online then do something smart
                                            std::cout << "[webserver] ERROR DFD_1.1 is NOT online " << "\n";

                                            /// Create a dataframe for response to new participant
                                            dataframe dfResponse;
                                            DED_START_ENCODER(encoder_ptr);
                                            DED_PUT_STRUCT_START( encoder_ptr, "WSResponse" );
                                            DED_PUT_METHOD	( encoder_ptr, "Method",  "JSCGetProfile" );
                                            DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)iTransID);
                                            DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)strProtocolTypeID );
                                            DED_PUT_STDSTRING	( encoder_ptr, "dest", (std::string)strUniqueId ); // destination is javascript client
                                            DED_PUT_STDSTRING	( encoder_ptr, "src", (std::string)"scanva_cloud_1" ); //TODO: add this scanvaservers unique id - perhaps in future more clouds are running - source is scanvaserver
                                            DED_PUT_STDSTRING	( encoder_ptr, "status", (std::string)"DFD_1.1 OFFLINE" );
                                            DED_PUT_STRUCT_END( encoder_ptr, "WSResponse" );
                                            DED_GET_ENCODED_DATAFRAME(encoder_ptr, dfResponse);

                                            /// set dataframe as response frame
                                            frmResponse=dfResponse;
                                            destination=source; // since reply have to go back to source, then destination becomes source
                                            bDecoded=true;
                                            std::cout << "[webserver] send WSResponse to : " << source << "\n";
                                        }
                                        else
                                        {
                                            /// setup fetch profile request from info provided in JSCGetProfile
                                            /// Create a fetch Profile request
                                            dataframe dfRequest;
                                            DED_START_ENCODER(encoder_ptr2);
                                            DED_PUT_STRUCT_START( encoder_ptr2, "DFDRequest" );
                                            //+ fixed area start
                                            DED_PUT_METHOD	( encoder_ptr2, "Method",  "1_1_8_FetchProfile" );
                                            DED_PUT_USHORT	( encoder_ptr2, "TransID",	(unsigned short)iTransID);
                                            DED_PUT_STDSTRING	( encoder_ptr2, "protocolTypeID", (std::string)"DED1.00.00" );
                                            DED_PUT_STDSTRING	( encoder_ptr2, "dest", (std::string)strDFD ); // destination is profile DFD
                                            DED_PUT_STDSTRING	( encoder_ptr2, "src", (std::string)strUniqueId ); /// its forward destination is a JavaScript client
                                            //+ Profile request area start
                                            DED_PUT_STDSTRING	( encoder_ptr2, "STARTrequest", (std::string)"FetchProfileRequest" );
                                            DED_PUT_STDSTRING	( encoder_ptr2, "STARTDATAstream", (std::string)"118" ); // TODO: add datadictionary id for the datastream
                                            DED_PUT_STDSTRING	( encoder_ptr2, "profileID", (std::string)strUniqueId ); // unique id for registered profile - this is actually the name of the main profile entityfile
                                            DED_PUT_STDSTRING	( encoder_ptr2, "profileName", (std::string)strUserName ); // add this in JSCGetProfile dataframe;   username
                                            DED_PUT_STDSTRING	( encoder_ptr2, "password", (std::string)strPassword );
                                            DED_PUT_STDSTRING	( encoder_ptr2, "ENDDATAstream", (std::string)"118" ); // TODO: add datadictionary id for the datastream
                                            DED_PUT_STDSTRING	( encoder_ptr2, "ENDrequest", (std::string)"FetchProfileRequest" );
                                            //- Profile request area end
                                            DED_PUT_STRUCT_END( encoder_ptr2, "DFDRequest" );
                                            DED_GET_ENCODED_DATAFRAME(encoder_ptr2, dfRequest);

                                            /// set dataframe as response frame
                                            frmResponse=dfRequest; /// this is actually a request to DFD however it will just be treated as a normal response dataframe transfer
                                            destination=dfddest;
                                            bDecoded=true;
                                            std::cout << "[webserver] send DFDRequest to : " << strDFD << "\n";
                                        }
                                    }
                            }
                            else
                            {
                                if(strProtocolTypeID == (std::string)"DED1.00.01")
                                {
                                    /// Handling future protocol
                                    /// A connect request following protocol version DED1.00.01 has been received
                                }
                                //else-if(strProtocolTypeID == (std::string)"DED1.00.02")
                                //TODO: consider sending a dataframe back with info about unknown DED version!!
                            }

                        }
                        break;
                        default:
                        {
                            /// set received dataframe as response frame
                            frmResponse=msg;
                            destination=source; // send frame back
                            bDecoded=false; // unknown DED
                        }
                        break;
                        }

                        /*
                        // deprecated
                        //                        /// Handle connect requests from JavaClients
                        //                        if( strMethod ==(std::string)"JSCconnect" )
                        //                        {
                        //                            /// Handle protocol DED1.00.00
                        //                            if( DED_GET_USHORT( decoder_ptr, "TransID", iTransID) &&
                        //                                    DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID ) && strProtocolTypeID == (std::string)"DED1.00.00" )
                        //                            {
                        ////                                if( DED_GET_STDSTRING( decoder_ptr, "functionName", strFunctionName ) &&
                        ////                                    DED_GET_STRUCT_END( decoder_ptr, "WSRequest" ))
                        //                                if( DED_GET_STDSTRING( decoder_ptr, "functionName", strFunctionName ) )
                        //                                {
                        //                                    /// A connect request following protocol version DED1.00.00 has been received
                        //                                    std::cout << "[webserver] A javaclient connect request following protocol version DED1.00.00 has been received\n";
                        //
                        //                                    /// fetch username and password
                        //                                    if( DED_GET_STDSTRING( decoder_ptr, "username", strUserName ) && DED_GET_STDSTRING( decoder_ptr, "password", strPassword ))
                        //                                    {
                        //                                        /// Add participant to vip list of DFD functions online
                        //                                        removevipparticipant(strFunctionName); /// First remove previous connection info
                        //                                        if(findvipparticipant(strFunctionName) == 0)
                        //                                        {
                        //                                            vipparticipants_.push_back(new vipparticipants(source,strFunctionName));
                        //
                        //                                            /// Send Request to DFD_1.1 and send a response if not online, if online then DFD will send a response
                        //                                            std::string strDFD = "DFD_1.1";
                        //                                            participant_ptr dfddest = findvipparticipant(strDFD);
                        //                                            if(dfddest==0)
                        //                                            {
                        //                                                /// TODO: if DFD is NOT online then do something smart
                        //                                                std::cout << "[webserver] ERROR DFD_1.1 is NOT online " << "\n";
                        //
                        //                                                /// Create a dataframe for response to new participant
                        //                                                DED_START_ENCODER(encoder_ptr);
                        //                                                DED_PUT_STRUCT_START( encoder_ptr, "WSResponse" );
                        //                                                DED_PUT_METHOD	( encoder_ptr, "Method",  "JSCconnect" );
                        //                                                DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)iTransID);
                        //                                                DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)strProtocolTypeID );
                        //                                                DED_PUT_STDSTRING	( encoder_ptr, "functionName", (std::string)strFunctionName );
                        //                                                DED_PUT_STDSTRING	( encoder_ptr, "status", (std::string)"ACCEPTED" );
                        //                                                DED_PUT_BOOL	    ( encoder_ptr, "testbool_false", false );
                        //                                                DED_PUT_BOOL	    ( encoder_ptr, "testbool_true", true );
                        //                                                DED_PUT_STRUCT_END( encoder_ptr, "WSResponse" );
                        //                                                DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,ptmpCompressedData,sizeoftmpCompressedData);
                        //
                        //                                                /// Create a binary dataframe with above DED inside payload
                        //                                                dataframe tempframe;
                        //                                                //tempframe.opcode = dataframe::text_frame; // javascript has issues with converting from blob to arraybuffer
                        //                                                tempframe.opcode = dataframe::binary_frame;
                        //                                                if(sizeoftmpCompressedData==0) sizeoftmpCompressedData = iLengthOfTotalData; // if sizeofcompresseddata is 0 then compression was not possible and size is the same as for uncompressed
                        //                                                tempframe.putBinaryInPayload(ptmpCompressedData,sizeoftmpCompressedData); // Put DED structure in dataframe payload
                        //
                        //                                                /// set dataframe as response frame
                        //                                                frmResponse=tempframe;
                        //                                                destination=source; // since reply have to go back to source, then destination becomes source
                        //                                                bDecoded=true;
                        //                                                std::cout << "[webserver] send WSResponse to : " << strFunctionName << "\n";
                        //                                            }
                        //                                            else
                        //                                            {
                        //                                                /// JavaScript client is now connected and accepted, however now it needs to login to its profile
                        //
                        //                                                /// Send Request Login to profile - profile will send response dataframe to JSClient with status
                        //                                                dataframe dfRequest;
                        //                                                DED_START_ENCODER(encoder_ptr2);
                        //                                                DED_PUT_STRUCT_START( encoder_ptr2, "DFDRequest" );
                        //                                                //+ fixed area start
                        //                                                DED_PUT_METHOD	( encoder_ptr2, "Method",  "1_1_6_LoginProfile" );
                        //                                                DED_PUT_USHORT	( encoder_ptr2, "TransID",	(unsigned short)iTransID);
                        //                                                DED_PUT_STDSTRING	( encoder_ptr2, "protocolTypeID", (std::string)"DED1.00.00" );
                        //                                                DED_PUT_STDSTRING	( encoder_ptr2, "dest", (std::string)"DFD_1.1" ); // destination is profile DFD
                        //                                                DED_PUT_STDSTRING	( encoder_ptr2, "src", (std::string)strFunctionName ); /// destination is a JavaScript client
                        //                                                //+ Profile request area start
                        //                                                DED_PUT_STDSTRING	( encoder_ptr2, "STARTrequest", (std::string)"LoginProfileRequest" );
                        //                                                DED_PUT_STDSTRING	( encoder_ptr2, "STARTDATAstream", (std::string)"116" ); // TODO: add datadictionary id for the datastream
                        //                                                DED_PUT_STDSTRING	( encoder_ptr2, "profileID", (std::string)strFunctionName ); // unique id for registered profile - this is actually the name of the main profile entityfile
                        //                                                DED_PUT_STDSTRING	( encoder_ptr2, "username", (std::string)strUserName );
                        //                                                DED_PUT_STDSTRING	( encoder_ptr2, "password", (std::string)strPassword );
                        //                                                DED_PUT_STDSTRING	( encoder_ptr2, "ENDDATAstream", (std::string)"116" ); // TODO: add datadictionary id for the datastream
                        //                                                DED_PUT_STDSTRING	( encoder_ptr2, "ENDrequest", (std::string)"LoginProfileRequest" );
                        //                                                //- Profile request area end
                        //                                                DED_PUT_STRUCT_END( encoder_ptr2, "DFDRequest" );
                        //                                                DED_GET_ENCODED_DATAFRAME(encoder_ptr2, dfRequest);
                        //
                        //                                                /// set dataframe as response frame
                        //                                                frmResponse=dfRequest; /// this is actually a request to DFD however it will just be treated as a normal response dataframe transfer
                        //                                                destination=dfddest;
                        //                                                //destination=source; // since reply have to go back to source, then destination becomes source
                        //                                                bDecoded=true;
                        //                                            }
                        //                                        }
                        //                                        //                                        else
                        //                                        //                                        {
                        //                                        //                                            /// deprecated - somehow it created problems
                        //                                        //                                            /// Create a dataframe for response to ALLREADY loggedin participant
                        //                                        //                                            dataframe tempframe;
                        //                                        //                                            DED_START_ENCODER(encoder_ptr);
                        //                                        //                                            DED_PUT_STRUCT_START( encoder_ptr, "WSResponse" );
                        //                                        //                                                DED_PUT_METHOD	( encoder_ptr, "Method",  "JSCconnect" );
                        //                                        //                                                DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)iTransID);
                        //                                        //                                                DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)strProtocolTypeID );
                        //                                        //                                                DED_PUT_STDSTRING	( encoder_ptr, "functionName", (std::string)strFunctionName );
                        //                                        //                                                DED_PUT_STDSTRING	( encoder_ptr, "status", (std::string)"LOGGEDIN" );
                        //                                        //                                                DED_PUT_BOOL	    ( encoder_ptr, "testbool_false", false );
                        //                                        //                                                DED_PUT_BOOL	    ( encoder_ptr, "testbool_true", true );
                        //                                        //                                            DED_PUT_STRUCT_END( encoder_ptr, "WSResponse" );
                        //                                        //                                            DED_GET_ENCODED_DATAFRAME(encoder_ptr, tempframe);
                        //                                        //
                        //                                        //                                            /// set dataframe as response frame
                        //                                        //                                            frmResponse=tempframe;
                        //                                        //                                            destination=source; // since reply have to go back to source, then destination becomes source
                        //                                        //                                            bDecoded=true;
                        //                                        //                                            std::cout << "[webserver] send Allready loggedin WSResponse to : " << strFunctionName << "\n";
                        //                                        //                                        }
                        //
                        //                                    }
                        //                                    else
                        //                                    {
                        //                                        /// Create a dataframe for response to unknown new participant
                        //                                        DED_START_ENCODER(encoder_ptr);
                        //                                        DED_PUT_STRUCT_START( encoder_ptr, "WSResponse" );
                        //                                        DED_PUT_METHOD	( encoder_ptr, "Method",  "JSCconnect" );
                        //                                        DED_PUT_USHORT	( encoder_ptr, "TransID",	(unsigned short)iTransID);
                        //                                        DED_PUT_STDSTRING	( encoder_ptr, "protocolTypeID", (std::string)strProtocolTypeID );
                        //                                        DED_PUT_STDSTRING	( encoder_ptr, "functionName", (std::string)strFunctionName );
                        //                                        DED_PUT_STDSTRING	( encoder_ptr, "status", (std::string)"NOT ACCEPTED" );
                        //                                        DED_PUT_STRUCT_END( encoder_ptr, "WSResponse" );
                        //                                        DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);
                        //
                        //                                        /// Create a binary dataframe with above DED inside payload
                        //                                        dataframe frame;
                        //                                        frame.opcode = dataframe::binary_frame;
                        //                                        if(sizeofCompressedData==0) sizeofCompressedData = iLengthOfTotalData; // if sizeofcompresseddata is 0 then compression was not possible and size is the same as for uncompressed
                        //                                        frame.putBinaryInPayload(pCompressedData,sizeofCompressedData); // Put DED structure in dataframe payload
                        //
                        //                                        /// set dataframe as response frame
                        //                                        frmResponse=frame;
                        //                                        destination=source; // since reply have to go back to source, then destination becomes source
                        //                                        bDecoded=true;
                        //                                        std::cout << "[webserver] send NOT ACCEPTED WSResponse to : " << strFunctionName << "\n";
                        //                                    }
                        //                                }
                        //                            }
                        //                            else
                        //                            {
                        //                                if(strProtocolTypeID == (std::string)"DED1.00.01")
                        //                                {
                        //                                    /// Handling future protocol
                        //                                    /// A connect request following protocol version DED1.00.01 has been received
                        //                                }
                        //                                //else-if(strProtocolTypeID == (std::string)"DED1.00.02")
                        //                                //TODO: consider sending a dataframe back with info about unknown DED version!!
                        //                            }
                        //                        }
                        //                        else
                        //                        {
                        //                            /// set received dataframe as response frame
                        //                            frmResponse=msg;
                        //                            destination=source; // send frame back
                        //                            bDecoded=false; // unknown DED
                        //                        }
                        //                    }*/
                    }
                }
                else
                {
                    /// Forward the dataframe -- below is fixed part of a DED packet - it should not be changed
                    std::string strDestination=(std::string)"";
                    std::string strSource=(std::string)"";
                    DED_GET_METHOD( decoder_ptr, "Method", strMethod );
                    DED_GET_USHORT( decoder_ptr, "TransID", iTransID);
                    DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID );
                    DED_GET_STDSTRING( decoder_ptr, "dest", strDestination );
                    DED_GET_STDSTRING( decoder_ptr, "src", strSource );

/////+tst - large image serup.png was correct -- somehow issue is further - also fin was true
//            if (strMethod==(std::string)"1_1_2_EditProfile")
//            { /// modify elements in profile
////                std::cout << "[test][DFDRequest:1_1_2_EditProfile] " << std::endl;
////                if(  DED_GET_USHORT( decoder_ptr, "TransID", iTransID) && /// iTransID should be used in a dataframe reply to this received dataframe
////                        DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID ) && strProtocolTypeID == (std::string)"DED1.00.00" )
////                {
////                    std::string strDestination=(std::string)"";
////                    std::string strSource=(std::string)"";
////                    if(  DED_GET_STDSTRING( decoder_ptr, "dest", strDestination ) && strDestination == (std::string)"DFD_1.1" &&
////                            DED_GET_STDSTRING( decoder_ptr, "src", strSource ) )
////                    {
//                        std::string strProfileID = (std::string)"";
//                        std::string strUsername  = (std::string)"";
//                        std::string strPassword  = (std::string)"";
//                        if(  DED_GET_STDSTRING( decoder_ptr, "profileID", strProfileID ) &&
//                            DED_GET_STDSTRING( decoder_ptr, "username", strUsername ) && DED_GET_STDSTRING( decoder_ptr, "password", strPassword ) )
//                        {
//                                std::string strStartupdate = (std::string)"";
//                                std::string strEndupdate  = (std::string)"";
//                                std::vector<Elements> vecElements;
//                                std::cout << "[test][DFDRequest:1_1_2_EditProfile] validate profile - SUCCESS " << std::endl;
//                                if(  DED_GET_STDSTRING( decoder_ptr, "STARTupdate", strStartupdate ) )
//                                {
//                                    DED_GET_TOAST( decoder_ptr, "profile", vecElements);
//                                    if(vecElements.size() <= 0)
//                                    {
//                                            std::cout << "[test][DFDRequest:1_1_2_EditProfile] ERROR - NO elements in toast area in dataframe" << std::endl;
//                                            bDecoded = false;
//                                    }
//                                    else
//                                    {
//                                        if( DED_GET_STDSTRING( decoder_ptr, "ENDupdate", strEndupdate ) )
//                                        {
//
//                                            std::cout << "[test][DFDRequest:1_1_2_EditProfile] STARTupdate -> toast elements -> ENDupdate - read from received dataframe " << std::endl;
//                                                BOOST_FOREACH(Elements f, vecElements)
//                                                {
//                                                    ///found element
//                                                    std::string strValue(f.ElementData.begin(),f.ElementData.end());
//                                                    std::cout << "[test][1_1_2_EditProfile] element  : " << f.strElementID << " Element value: " << strValue << std::endl;
//                                                }
//
//
//
//                                            /*
//                                            /// now take each new value and store in database file
//                                            std::string EntityName     = "Profile";
//                                            std::string EntityFileName = strProfileID;
//                                            CDatabaseControl CDbCtrl;
//                                            std::vector<Elements> record_value;
//                                            /// use ftgt to fetch the entity file
//                                            bDecoded = CDbCtrl.ftgt( (std::string)EntityName, (std::string)EntityFileName, record_value );
//                                            if(bDecoded==false)
//                                                std::cout << "[test][1_1_2_EditProfile] ERROR - could not ftgt entityfile : " << EntityFileName << std::endl;
//                                            else
//                                            {
//                                                bDecoded=false;
//                                                /// update entity attribut in entity file
//                                                BOOST_FOREACH(Elements f, vecElements)
//                                                {
//                                                    ///found element to update
//                                                    bDecoded = CDbCtrl.update_element_value(record_value, f.strElementID, f.ElementData);
//                                                    if(bDecoded==false)
//                                                        std::cout << "[test][1_1_2_EditProfile] ERROR - could NOT update_element_value : " << f.strElementID << std::endl;
//                                                }
//                                                ///update the entity file
//                                                if(bDecoded==true)
//                                                    bDecoded = CDbCtrl.put((std::string)EntityName,(std::string)EntityFileName,record_value);
//                                                if(bDecoded==false)
//                                                    std::cout << "[test][1_1_2_EditProfile] ERROR - could NOT update entityfile after change : " << EntityFileName << std::endl;
//                                            }
//                                            */
//                                        }
//                                        else
//                                        {
//                                            std::cout << "[test][DFDRequest:1_1_2_EditProfile] ERROR ENDtoast / ENDupdate - MISSING  " << std::endl;
//                                            bDecoded = false;
//                                        }
//                                    }
//                                }
//                            }
//                            else
//                            {
//                                std::cout << "[test][DFDRequest:1_1_2_EditProfile] validate profile - ACCESS DENIED " << std::endl;
//
//                            }
// //                       }
// //                   }
//           }
/////-tst

/*/////+tst large image in createprofile dataframe somehow gets destroyed before or during transfer to DFD  - investigate if dataframe received is ok
///// test showed that data in payload was ok - picture was intact - issue is elsewhere
//                    if( strMethod==(std::string)"1_1_1_CreateProfile")
//                    {
//
//                        if(  DED_GET_USHORT( decoder_ptr, "TransID", iTransID) && /// iTransID should be used in a dataframe reply to this received dataframe
//                                DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", strProtocolTypeID ) && strProtocolTypeID == (std::string)"DED1.00.00" )
//                        {
//                            /// Handle version DED1.00.00 for CreateProfile
//                            std::string strDestination=(std::string)"";
//                            std::string strSource=(std::string)"";
//                            std::string strStartRequest=(std::string)"";
//                            std::string strStartRecord=(std::string)"";
//                            std::string strEndRecord=(std::string)"";
//                            std::string strEndRequest=(std::string)"";
//                            std::string strStartToast=(std::string)"";
//                            std::string strEndToast=(std::string)"";
//
//                            if(  DED_GET_STDSTRING( decoder_ptr, "dest", strDestination ) && strDestination == (std::string)"DFD_1.1" &&
//                                    DED_GET_STDSTRING( decoder_ptr, "src", strSource ) &&
//                                    DED_GET_STDSTRING( decoder_ptr, "STARTrequest", strStartRequest ) && strStartRequest == (std::string)"EmployeeRequest" &&
//                                    DED_GET_STDSTRING( decoder_ptr, "STARTrecord", strStartRecord ) && strStartRecord == (std::string)"record" )
//                            {
//                                /// Take Profile info and transfer to 1.1.1 Create Profile
//                                /// DFD stream "Create New Profile Info" -> "Create Profile 1.1.1"
//
//                                struct CreateNewProfileInfo
//                                {
//                                    /// Helpful script to cut attributes out of DD_PROFILE file
//                                    /// $cat DD_PROFILE.xml | grep -e '<PhysicalDataElementName>' | sed -e 's/<PhysicalDataElementName>/\"/' | sed -e 's/<\/PhysicalDataElementName>/\",/'
//                                    unsigned short iTransID; // trans id is used for as handshaking between client/"server", to make sure communication is aligned
//                                    std::string strSource; // source is name of "client/function" which is making a request for creating a new Profile -- this is needed to beable to send a response back
//                                    std::string strProfileID;
//                                    std::string strProfileName;
//                                    std::string strProtocolTypeID;
//                                    std::string strSizeofProfileData;
//                                    std::string strProfile_chunk_id;
//                                    std::string strAccountStatus;
//                                    std::string strSubscriptionExpireDate;
//                                    std::string strProfileStatus;
//                                    std::vector<Elements> vecElements; // here is TOAST data, in the form of elements
//                                };
//                                CreateNewProfileInfo newProfileInfo;
//                                newProfileInfo.iTransID = iTransID;
//                                newProfileInfo.strSource = strSource;
//
//                                if (DED_GET_STDSTRING( decoder_ptr, "profileID", newProfileInfo.strProfileID ) &&
//                                DED_GET_STDSTRING( decoder_ptr, "profileName", newProfileInfo.strProfileName ) &&
//                                DED_GET_STDSTRING( decoder_ptr, "protocolTypeID", newProfileInfo.strProtocolTypeID ) &&
//                                DED_GET_STDSTRING( decoder_ptr, "sizeofProfileData", newProfileInfo.strSizeofProfileData ) &&
//                                DED_GET_STDSTRING( decoder_ptr, "profile_chunk_id", newProfileInfo.strProfile_chunk_id ) &&
//                                DED_GET_STDSTRING( decoder_ptr, "AccountStatus", newProfileInfo.strAccountStatus ) &&
//        //                        DED_GET_STDSTRING( decoder_ptr, "SubscriptionExpireDate", newProfileInfo.strSubscriptionExpireDate ) &&
//                                DED_GET_STDSTRING( decoder_ptr, "ExpireDate", newProfileInfo.strSubscriptionExpireDate ) &&
//                                DED_GET_STDSTRING( decoder_ptr, "ProfileStatus", newProfileInfo.strProfileStatus ))
//                                {
//                                    bDecoded=true;
//                                    ///+ put toast part in structure, if it exists
//                                    DED_GET_TOAST( decoder_ptr, "profile", newProfileInfo.vecElements);
//                                    ///-
//                                    BOOST_FOREACH( Elements f, newProfileInfo.vecElements )
//                                    {
//                                        if(f.strElementID == "foto")
//                                        {
//                                            std::cout << "TEST : dataframe with foto embedded has been received in server - DFD was NOT online" << "\n";
//                                            break;
//                                        }
//                                    }
//                                }
//                                else
//                                {
//                                    std::cout << "[test_1_1_1_CreateProfile] Error: dataframe was NOT valid "<< "\n";
//                                }
//                            }
//                        }
//                    }
/////-tst*/

                    if(strMethod==(std::string)"")
                        std::cout << "[WebServer][WARNING] dataframe was not parsed correctly -- no method in DED structure" << "\n";

                    /// set received dataframe as response frame
                    frmResponse=msg;
                    participant_ptr dest = findvipparticipant(strDestination);
                    if(dest == 0)
                    {
                        destination=source; // send frame back
                        bDecoded=false; // Dataframe does not have a known DED type, inside its payload
                        std::cout << "[WebServer][WARNING] dataframe was not send to recipient, because recipient was NOT registered : [ " << strDestination << " ] " << "\n";
                    }
                    else
                    {
                        destination=dest; // send to destination
                        ///TODO: check if source is allowed / registered - if NOT then do NOT deliver dataframe to entities on server
                        ///NB! if an entity is not registered, then fx. a DFD will NOT be-able to send back a dataframe and the DFD will get its own dataframe bounced back - this is unfortunate !!!!
                        bDecoded=true; // dataframe is legal and have a legal DED in its payload, it will be passed further on
                    }
                }

                /// deliver dataframe to receipient
                deliverTo(frmResponse, destination);
            }

        } // chat
    } // namespace applications
} // namespace websocket
