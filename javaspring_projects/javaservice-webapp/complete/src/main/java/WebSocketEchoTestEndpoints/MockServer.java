package WebSocketEchoTestEndpoints;

import messaging.simp.ded.DEDDecoder;
import messaging.simp.ded.DEDEncoder;

import javax.websocket.OnMessage;
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
        System.out.println("MockServerLogin endpoint received dedpacket - now handle it, and send a response dedpacket ");

        // 1. decode login packet
        DEDDecoder DED = new DEDDecoder();
        DED.PUT_DATA_IN_DECODER( dedpacket, dedpacket.length);
        if( DED.GET_STRUCT_START( "WSRequest" )==1 &&
                (strMethod          = DED.GET_METHOD ( "name" )).length()>0 &&
                (uTrans_id          = DED.GET_USHORT ( "trans_id")) !=-1 &&
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
                System.out.println("DFDRequest - received - now parse");
            }
            bDecoded=false;
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
            DED2.PUT_METHOD   ( "name", strMethod );
            DED2.PUT_USHORT   ( "trans_id", uTrans_id);
            DED2.PUT_STDSTRING( "protocolTypeID", "DED1.00.00");
            DED2.PUT_STDSTRING( "functionName", strFunctionName );
            DED2.PUT_STDSTRING( "status", strStatus );
        DED2.PUT_STRUCT_END( "WSResponse" );

        byte[] dedResponsePacket = DED2.GET_ENCODED_BYTEARRAY_DATA();

        // 4. send response packet
        if(dedResponsePacket==null) {
            dedResponsePacket = dedpacket; // echo back original packet, since encoding of response packet went wrong!!
            System.out.println("Internal ERROR [MockServer] - was not capable of creating a DED response packet, thus echoing received back");
        }
        return dedResponsePacket;
    }
}

