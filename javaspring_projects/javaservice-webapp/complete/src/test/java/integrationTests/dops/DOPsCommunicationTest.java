package integrationTests.dops;

import JavaServicesApp.ClientEndpoint.JavaWebSocketClientEndpoint;
import JavaServicesApp.ProtocolHandlings.DOPsCommunication;
import dops.protocol.DOPS;
import dops.protocol.ded.DEDDecoder;
import dops.protocol.ded.DEDEncoder;
import integrationTests.IntegrationEnvironmentSetup;
import junit.framework.TestCase;
import org.junit.Before;
import org.junit.Test;

import java.nio.ByteBuffer;

import static org.junit.Assert.assertEquals;

/**
 * INTEGRATION TESTS
 *
 * To run these tests, then the VM's needed must be running
 *
 * how to manually start VM's :
 *
 * vagrant up <name of VM>
 *
 * Created by serup on 05-01-16.
 */
public class DOPsCommunicationTest {

    IntegrationEnvironmentSetup env = new IntegrationEnvironmentSetup();

    @Before
    public void setup() throws Exception {
        assertEquals(true,env.setupIntegrationEnvironment());
    }

    @Test
    public void testLoginToServer() throws Exception {

        DOPS dops = new DOPS();
        String url = "backend.scanva.com";
        String port = "7777";
        String uniqueId = "985998707DF048B2A796B44C89345494";
        String username = "johndoe@email.com"; // TODO: find a way to safely handle retrieval of username,password - should NOT be stored in source code
        String password = "12345";

        assertEquals(false,dops.loginToServer(url,port,uniqueId,username,password)); // expect false, since user is NOT registered in DFD
        assertEquals(true,dops.isOpen()); // even though login to profile failed, then connection to DOPS main part should have been established

        System.out.println("done - success");
    }

    @Test
    public void testConnectToDOPs() throws Exception {
        DOPsCommunication dopsCommunications = new DOPsCommunication();
            String uniqueId = "HadoopJavaServiceApp";
            //String uniqueId = "985998707DF048B2A796B44C89345494";
            String username = "johndoe@email.com";
            String password = "12345";

            if(dopsCommunications.connectToDOPs(uniqueId, username, password)) {
                System.out.println("connected - success");
            }
            else
                throw new Exception("Connection with REAL DOPS server - FAILED");
    }

    @Test
    public void testConnectToDFD()
    {
        /**
         * setup client
         */
        JavaWebSocketClientEndpoint clientEndpoint = new JavaWebSocketClientEndpoint();

        /**
         * connect client -- Make sure the backend is running -- example start: vagrant up backend
         */
//        clientEndpoint.connectToServer("ws://localhost:7778");  // NB! make sure backend VM is NOT running or start local server with port 7778 - then you can debug server :-)
        clientEndpoint.connectToServer("ws://backend.scanva.com:7777");

        /**
         * prepare data to be send
         */
        short trans_id = 69;
        boolean action = true;
        String uniqueId = "985998707DF048B2A796B44C89345494";
        String username = "johndoe@email.com"; // TODO: find a way to safely handle retrieval of username,password - should NOT be stored in source code
        String password = "12345";

        /**
         * create DED connect datapacket for DOPS for java clients
         */
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
                    if(uTrans_id != trans_id) bDecoded=false;
                    TestCase.assertEquals(true,bDecoded);
                    if(!strFunctionName.equals(uniqueId)) bDecoded=false;
                    TestCase.assertEquals(true,bDecoded);
                    if(!strStatus.equals("ACCEPTED")) bDecoded=false;
                    TestCase.assertEquals(true,bDecoded);
                    if(!strProtocolTypeID.equals("DED1.00.00")) bDecoded=false;
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
        TestCase.assertEquals(true,bDecoded);
    }

}