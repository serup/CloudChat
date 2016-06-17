package dops.mocks;

import dops.protocol.ded.DEDDecoder;
import dops.protocol.ded.DEDEncoder;

import javax.websocket.MessageHandler;
import javax.websocket.OnMessage;
import javax.websocket.OnOpen;
import javax.websocket.Session;
import javax.websocket.server.ServerEndpoint;

/**
 * Created by serup on 30-12-15.
 *
 * this class simulates a DOPS system login - login to pro actor server, which logs in to DFD backend database profile and then response
 */
@ServerEndpoint(value = "/MockServerEndpoint")
public class MockServer {

    boolean bDecoded=false;
    String strMethod="";
    String strProtocolTypeID="";
    String strFunctionName="";
    String strUsername="";
    String strPassword="";
    String strStatus="UNKNOWN USER - LOGIN NOT ACCEPTED";

    short uTrans_id=0;


    @OnMessage
    public byte[] onMessage(byte[] dedpacket, Session session)
    {
        System.out.println("- MockServerLogin endpoint received dedpacket - now handle it, and send a response dedpacket ");

        // 1. decode login packet
        DEDDecoder DED = new DEDDecoder();
        DED.PUT_DATA_IN_DECODER( dedpacket, dedpacket.length);
        if( DED.GET_STRUCT_START( "WSRequest" )==1 &&
                (strMethod          = DED.GET_METHOD ( "Method" )).length()>0 &&
                (uTrans_id          = DED.GET_USHORT ( "TransID")) !=-1 &&
                (strProtocolTypeID  = DED.GET_STDSTRING ( "protocolTypeID")).length()>0 &&
                (strFunctionName    = DED.GET_STDSTRING ( "functionName")).length()>0 &&
                (strUsername        = DED.GET_STDSTRING ( "username")).length()>0 &&
                (strPassword        = DED.GET_STDSTRING ( "password")).length()>0 &&
            DED.GET_STRUCT_END( "WSRequest" )==1)
        {
            bDecoded=true;

            // expected values
            short trans_id = 69;
            String uniqueId = "985998707DF048B2A796B44C89345494";
            String username = "johndoe@email.com";
            String password = "12345";

            if(!strMethod.equals("JavaConnect")) bDecoded=false;
            if(uTrans_id != trans_id) bDecoded=false;
            if(!strFunctionName.equals(uniqueId)) bDecoded=false;
            if(!strProtocolTypeID.equals("DED1.00.00")) bDecoded=false;
            if(!strUsername.equals(username)) bDecoded=false;
            if(!strPassword.equals(password)) bDecoded=false;
        }
        else
        {
            if( DED.GET_STRUCT_START( "DFDRequest" )==1 &&
                (strMethod          = DED.GET_METHOD ( "Method" )).length()>0 &&
                (uTrans_id          = DED.GET_USHORT ( "TransID")) !=-1 &&
                (strProtocolTypeID  = DED.GET_STDSTRING ( "protocolTypeID")).length()>0 )
            {
                System.out.println("- now determine DED");
                String strClientSrc="<unknown>";
                if((DED.GET_STDSTRING("dest")).contains("DFD_1.1")) {
                    System.out.println("DFDRequest - received - now parse");

                    if ((strClientSrc = DED.GET_STDSTRING("src")).length() > 0 &&
                            (DED.GET_STDSTRING("STARTrequest")).contains("EmployeeRequest") &&
                            (DED.GET_STDSTRING("STARTrecord")).contains("record")) {
                        System.out.println("- EmployeeRequest - received - now parse");
                        String strProfileID = "", strProfileName = "", strSizeOfProfileData = "", strProfile_chunk_id = "", strAccountStatus = "", strExpireDate = "", strProfileStatus = "";
                        if ((strProfileID = DED.GET_STDSTRING("profileID")).length() > 0 &&
                                (strProfileName = DED.GET_STDSTRING("profileName")).length() > 0 &&
                                (strProtocolTypeID = DED.GET_STDSTRING("protocolTypeID")).length() > 0 &&
                                (strSizeOfProfileData = DED.GET_STDSTRING("sizeofProfileData")).length() > 0 &&
                                (strProfile_chunk_id = DED.GET_STDSTRING("profile_chunk_id")).length() > 0 &&
                                (strAccountStatus = DED.GET_STDSTRING("AccountStatus")).length() > 0 &&
                                (strExpireDate = DED.GET_STDSTRING("ExpireDate")).length() > 0 &&
                                (strProfileStatus = DED.GET_STDSTRING("ProfileStatus")).length() > 0) {
                            System.out.println("-- Employee record received - now validate TOAST ");
                            if ((DED.GET_STDSTRING("STARTtoast")).length() > 0) {
                                // TOAST area found, now iterate thru all elements
                                System.out.println("--- TOAST area found, now iterate thru all elements");
                                DEDDecoder._Elements elementvalue = null;
                                while ((elementvalue = DED.GET_ELEMENT("profile")) != null) {
                                    System.out.println("--- TOAST element : " + elementvalue.strElementID);
                                }
                                if ((DED.GET_STDSTRING("elements-ignore").isEmpty()) &&
                                        (DED.GET_STDSTRING("ENDrecord")).contains("record") &&
                                        (DED.GET_STDSTRING("ENDrequest")).contains("EmployeeRequest") &&
                                        (DED.GET_STDSTRING("DFDRequest")).isEmpty()) {
                                    System.out.println("-- END Employee record");
                                    System.out.println("- END EmployeeRequest");
                                    System.out.println("END DFDRequest");
                                    System.out.println("DFDRequest parsed correct");
                                    bDecoded = true;
                                }

                                // 2. determine what to respond
                                if (bDecoded)
                                    strStatus = "Profile Saved in database";
                                else
                                    strStatus = "Error in creating profile";

                                // 3. create response packet
                                DEDEncoder DED2 = new DEDEncoder();
                                DED2.PUT_STRUCT_START("DFDResponse");
                                DED2.PUT_METHOD("Method", "CreateProfile");
                                DED2.PUT_USHORT("TransID", uTrans_id);
                                DED2.PUT_STDSTRING("protocolTypeID", "DED1.00.00");
                                DED2.PUT_STDSTRING("dest", strClientSrc);
                                DED2.PUT_STDSTRING("src", "DFD_1.1");
                                DED2.PUT_STDSTRING("status", strStatus);
                                DED2.PUT_STRUCT_END("DFDResponse");

                                byte[] dedResponsePacket = DED2.GET_ENCODED_BYTEARRAY_DATA();

                                // 4. send response packet
                                if (dedResponsePacket == null) {
                                    dedResponsePacket = dedpacket; // echo back original packet, since creation of response packet went wrong!!
                                    System.out.println("Internal ERROR [MockServer] - was not capable of creating a DED response packet, thus echoing received back");
                                }
                                return dedResponsePacket;
                            } else {
                                // NO TOAST area found
                                System.out.println("No TOAST area found in request, meaning NO elements added to profile info");
                            }
                        }
                    }
                    else
                    {
                        System.out.println("- MockServer; Warning - unknown DED - accepting basic parsing - header of packet was correct");
                        bDecoded=true;
                    }
                }
                else
                {
                    //TODO: refactor to be more general in its handling of DEDs
                    System.out.println("- WARNING: Unknown DED type");
                    System.out.println("- TODO: [MockServer] refactor to be more general in its handling of DEDs");
                }
            }
            if(!bDecoded)
                System.out.println("WARNING [MockServer] - was not capable of decoding incoming DED datapacket - could be unknown DED method");
        }

        // 2. determine what to respond
        if(bDecoded)
            strStatus="ACCEPTED";
        else
            strStatus="NOT ACCEPTED USER";

        // 3. create response packet
        DEDEncoder DED2 = new DEDEncoder();
        DED2.PUT_STRUCT_START( "WSResponse" );
            DED2.PUT_METHOD   ( "Method", strMethod );
            DED2.PUT_USHORT   ( "TransID", uTrans_id);
            DED2.PUT_STDSTRING( "protocolTypeID", "DED1.00.00");
            DED2.PUT_STDSTRING( "functionName", strFunctionName );
            DED2.PUT_STDSTRING( "status", strStatus );
        DED2.PUT_STRUCT_END( "WSResponse" );

        byte[] dedResponsePacket = DED2.GET_ENCODED_BYTEARRAY_DATA();

        // 4. send response packet
        if(dedResponsePacket==null) {
            dedResponsePacket = dedpacket; // echo back original packet, since creation of response packet went wrong!!
            System.out.println("Internal ERROR [MockServer] - was not capable of creating a DED response packet, thus echoing received back");
        }
        return dedResponsePacket;
    }

    @OnOpen
    public void onOpen(Session session)
    {
        // for test purposes - if a pure text is send from client
        session.addMessageHandler(new MessageHandler.Whole<String>() {

            @Override
            public void onMessage(String message) {
                System.out.println("WARNING TEXT on a Binary socket - Received message: "+message);
            }
        });

    }

}

