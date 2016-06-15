package dops.protocol;

import JavaServicesApp.ClientEndpoint.JavaWebSocketClientEndpoint;
import dops.protocol.ded.DEDDecoder;
import dops.protocol.ded.DEDEncoder;

import java.nio.ByteBuffer;

/**  -- will be DEPRECATED - instead use DOPsCommunication class
 * This class handles DED packet data communication between client and DOPS server
 * It has methods for login to DOPS DFD server and it handles basic communication
 * it is also a wrapper class for JavaWebSocketClientEndpoint
 *
 * Created by serup on 05-01-16.
 */
public class DOPS extends JavaWebSocketClientEndpoint {

    JavaWebSocketClientEndpoint clientEndpoint = new JavaWebSocketClientEndpoint();

    public boolean isOpen()
    {
        return clientEndpoint.clientSession.isOpen();
    }

    private ByteBuffer createDEDLogin(short trans_id, String uniqueId, String username, String password)
    {
        ByteBuffer data = null;
        /**
         * create DED connect datapacket for DOPS for java clients
         */
        DEDEncoder DED = new DEDEncoder();
        DED.PUT_STRUCT_START ( "WSRequest" );
            DED.PUT_METHOD   ( "Method",  "JavaConnect" );
            DED.PUT_USHORT   ( "TransID",  trans_id);
            DED.PUT_STDSTRING( "protocolTypeID", "DED1.00.00");
            DED.PUT_STDSTRING( "functionName", uniqueId );
            DED.PUT_STDSTRING( "username", username ); //TODO: SUGGESTION: use username to lookup in secure file, then use content in file as username and password
            DED.PUT_STDSTRING( "password", password );
        DED.PUT_STRUCT_END( "WSRequest" );
        data = DED.GET_ENCODED_BYTEBUFFER_DATA();

        return data;
    }

    public boolean loginToServer(String strURL, String port, String uniqueId, String username, String password)
    {
        boolean bResult=false;

        /**
         * connect client
         */
        //clientEndpoint.connectToServer("ws://backend.scanva.com:7777");
        if(port.isEmpty()) port = "7777";
        clientEndpoint.connectToServer("ws://"+strURL+":"+port);

        /**
         * prepare data to be send
         */
        short trans_id = 69; // trans id for java clients
        String _uniqueId = uniqueId; // example : "985998707DF048B2A796B44C89345494";
        String _username = username; //           "johndoe@email.com";
        String _password = password; //           "12345";

        /**
         * create DED connect datapacket for DOPS for java clients
         */
        ByteBuffer data = createDEDLogin(trans_id,_uniqueId,_username,_password);

        /**
         * send to server with client current client session connection
         */
        clientEndpoint.sendToServer(data);

        /**
         * wait for incomming data response, then receive data
         */
        byte[] receivedData = clientEndpoint.receiveFromServer();

        /**
         * decode incomming data
         */
        boolean bDecoded=false;
        String strMethod="";
        String strProtocolTypeID="";
        String strFunctionName="";
        String strStatus="";
        short uTrans_id=0;

        // decode data ...
        DEDDecoder DED2 = new DEDDecoder();
        DED2.PUT_DATA_IN_DECODER( receivedData, receivedData.length);
        if( DED2.GET_STRUCT_START( "WSResponse" )==1 &&
                (strMethod   = DED2.GET_METHOD ( "Method" )).length()>0 &&
                (uTrans_id     = DED2.GET_USHORT ( "TransID")) !=-1 &&
                (strProtocolTypeID  = DED2.GET_STDSTRING ( "protocolTypeID")).length()>0 )
        {
            if(strMethod.equals("1_1_6_LoginProfile"))
            {
                String strDestination="";
                String strSource="";
                // login response from DFD was received, now decode response
                if ( (strDestination = DED2.GET_STDSTRING ( "dest")).length()>0 &&
                        (strSource      = DED2.GET_STDSTRING ( "src")).length()>0 &&
                        (strStatus      = DED2.GET_STDSTRING ( "status")).length()>0 &&
                        DED2.GET_STRUCT_END( "WSResponse" )==1)
                {
                    // response from login to profile in DFD was received - now validate status
                    if(strStatus.equals("ACCEPTED")) {
                        // login success
                        bDecoded=true;
                        System.out.println("1_1_6_LoginProfile response packet decoded; src: "+strSource+" ; dest: "+strDestination+" ; Status: ACCEPTED");
                    }
                    else
                    {
                        // login failure is actually a correct response, since client is not registered
                        System.out.println("1_1_6_LoginProfile response packet decoded; however login failed ;  Status: "+strStatus);
                    }
                }
            }
            else
            { // When DFD is offline, then this type of packet is received
                if ( (strFunctionName    = DED2.GET_STDSTRING ( "functionName")).length()>0 &&
                                (strStatus  = DED2.GET_STDSTRING ( "status")).length()>0 &&
                     DED2.GET_STRUCT_END( "WSResponse" )==1)
                {
                    System.out.println("[loginToServer] Profile on server is OFF line: ");
                }
                else
                {
                    // unknown response
                    System.out.println("[loginToServer] unknown response from server");
                }
            }
        }
        bResult = bDecoded;
        return bResult;
    }
}
