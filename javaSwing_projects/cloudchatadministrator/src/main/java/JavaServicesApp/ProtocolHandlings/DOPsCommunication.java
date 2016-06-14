package JavaServicesApp.ProtocolHandlings;

import JavaServicesApp.ClientEndpoint.JavaWebSocketClientEndpoint;
import dops.protocol.ded.DEDDecoder;
import dops.protocol.ded.DEDEncoder;

import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Created by serup on 09-05-16.
 */
public class DOPsCommunication {
    private JavaWebSocketClientEndpoint clientEndpoint=null;

    public boolean connectToDOPs(String uniqueId, String username, String password){
        boolean bResult;
        clientEndpoint = new JavaWebSocketClientEndpoint();
        clientEndpoint.connectToServer("ws://backend.scanva.com:7777");

        ByteBuffer data = prepareDataToSend(uniqueId, username, password);
        clientEndpoint.sendToServer(data);
        bResult = decodeIncomingData(clientEndpoint.receiveFromServer()).contains("dops.connected.status.ok");

        return bResult;
    }

    public boolean disconnectFromDOPs() throws IOException {
        boolean bResult=false;
        if(clientEndpoint!=null)
        {
            clientEndpoint.clientSession.close();
            bResult=true;
        }
        return bResult;
    }

    private static ByteBuffer prepareDataToSend(String uniqueId, String username, String password) {
        short trans_id = 69;
/*
        String uniqueId = "985998707DF048B2A796B44C89345494";
        String username = "johndoe@email.com"; // TODO: find a way to safely handle retrieval of username,password - should NOT be stored in source code
        String password = "12345";
*/
        ByteBuffer data = createDEDforDOPSJavaConnect(trans_id, uniqueId, username, password);

        return data;
    }


    private static String decodeIncomingData(byte[] receivedData)
    {
        String Result="dops.decode.status.error";
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
                        bDecoded=true;
                        System.out.println("1_1_6_LoginProfile response packet decoded; src: "+strSource+" ; dest: "+strDestination+" ; Status: ACCEPTED");
                    }
                    else
                    {
                        bDecoded=true; // set to true since login failure is actually a correct response, since client is not registered
                        System.out.println("1_1_6_LoginProfile response packet decoded; however login failed ;  Status: "+strStatus);
                    }
                }
            }
            else
            { // When DFD is offline, then this type of packet is received
                if (
                        (strFunctionName    = DED2.GET_STDSTRING ( "functionName")).length()>0 &&
                                (strStatus  = DED2.GET_STDSTRING ( "status")).length()>0 &&
                                DED2.GET_STRUCT_END( "WSResponse" )==1)
                {
                    bDecoded=true;
                    System.out.println("DED packet decoded - now validate");

                    if(!strMethod.equals("JavaConnect")) bDecoded=false;
                }
                else
                {
                    // unknown method
                    System.out.println("DED packet could NOT be decoded - unknown Method: "+strMethod);
                }
            }
        }
        else
        {
            bDecoded=false;
        }

        if(bDecoded)
            Result="dops.connected.status.ok";
        return Result;
    }

    private static ByteBuffer createDEDforDOPSJavaConnect(short trans_id, String uniqueId, String username, String password)
    {
        DEDEncoder DED = new DEDEncoder();
        DED.PUT_STRUCT_START ( "WSRequest" );
        DED.PUT_METHOD   ( "Method",  "JavaConnect" );
        DED.PUT_USHORT   ( "TransID",  trans_id);
        DED.PUT_STDSTRING( "protocolTypeID", "DED1.00.00");
        DED.PUT_STDSTRING( "functionName", uniqueId );
        DED.PUT_STDSTRING( "username", username );
        DED.PUT_STDSTRING( "password", password );
        DED.PUT_STRUCT_END( "WSRequest" );

        ByteBuffer data = DED.GET_ENCODED_BYTEBUFFER_DATA();
        return data;
    }

}
