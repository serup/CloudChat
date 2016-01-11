import ClientEndpoint.JavaWebSocketClientEndpoint;
import Mocks.MockTestServer;
import WebSocketEchoTestEndpoints.EchoByteArrayEndpoint;
import messaging.simp.ded.DEDDecoder;
import messaging.simp.ded.DEDEncoder;
import org.glassfish.tyrus.server.Server;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;

import static junit.framework.TestCase.assertEquals;

/**
 * Created by serup on 22-12-15.
 */
public class WebSocketTests {

    private static MockTestServer mockTestServer=null;
    private static Thread serverThread;

    public void runByteArrayServer() {
        serverThread = new Thread() {
            public void run() {
                Server server = new Server("localhost", 8033, "/websockets", null, EchoByteArrayEndpoint.class);
                try {
                    server.start();
                    BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
                    System.out.println("Server ready and waiting for input.");
                    reader.read();
                } catch (Exception e) {
                    e.printStackTrace();
                } finally {
                    server.stop();
                }
            }
        };
        serverThread.start();
    }


    @Before
    public void setupMockServer()
    {
        /**
         * Start and connect to a MOCK server which can act as a Server and receive a DED packet and return a DED packet with result
         */
        if(mockTestServer==null)
            mockTestServer = new MockTestServer(8046,"MockServerEndpoint");
    }

    @Test
    @Ignore // some how it is not possible to run multiple servers even if port is different - hmm !?!?!
    public void testClientServerBinaryEcho() throws Exception {

        // no need for mockTestServer, so close it down if running - there is however issues when localhost has a running server - normally there could be more servers on seperate ports, however in Java there seems to be some issues
        if(mockTestServer!=null)
            mockTestServer.stopServer();

        /**
         * First start a server which can receive and echo back a binary array
         */
        runByteArrayServer();

        /**
         * setup client
         */
        JavaWebSocketClientEndpoint clientEndpoint = new JavaWebSocketClientEndpoint();

        /**
         * connect client
         */
        clientEndpoint.connectToServer("ws://localhost:8033/websockets/echoBinary");

        /**
         * prepare data to be send
         */
        short trans_id = 1;
        boolean action = true;

        DEDEncoder DED = new DEDEncoder();
        DED.PUT_STRUCT_START( "event" );
        DED.PUT_METHOD  ( "name",  "MusicPlayer" );
        DED.PUT_USHORT  ( "trans_id",  trans_id);
        DED.PUT_BOOL    ( "startstop", action );
        DED.PUT_STDSTRING("text", "hello world");
        DED.PUT_STRUCT_END( "event" );

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
        String strValue="";
        short iValue=0;
        boolean bValue=false;
        String strText="";

        // decode data ...
        DEDDecoder DED2 = new DEDDecoder();
        DED2.PUT_DATA_IN_DECODER( receivedData, receivedData.length);
        if( DED2.GET_STRUCT_START( "event" )==1 &&
                (strValue = DED2.GET_METHOD ( "name" )).length()>0 &&
                (iValue   = DED2.GET_USHORT ( "trans_id")) !=-1 &&
                (bValue   = DED2.GET_BOOL   ( "startstop")) &&
                (strText  = DED2.GET_STDSTRING ( "text")).length()>0 &&
                DED2.GET_STRUCT_END( "event" )==1)
        {
            bDecoded=true;
        }
        else
        {
            bDecoded=false;
        }

        assertEquals(true,bDecoded);

    }


    @Test
    public void testCreateProfileInMockServer()
    {
        boolean bCreatedProfile=false;
        assertEquals(true,mockTestServer.isOpen());

        /**
         * prepare data to be send
         */
//TODO: build handling of a create profile event
        assertEquals(true,bCreatedProfile);
    }

    @Test
    public void testLoginToMockServer() throws Exception {
        assertEquals(true,mockTestServer.isOpen());

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
            DED.PUT_METHOD   ( "name",  "JavaConnect" );
            DED.PUT_USHORT   ( "trans_id",  trans_id);
            DED.PUT_STDSTRING( "protocolTypeID", "DED1.00.00");
            DED.PUT_STDSTRING( "functionName", uniqueId );
            DED.PUT_STDSTRING( "username", username );
            DED.PUT_STDSTRING( "password", password );
        DED.PUT_STRUCT_END( "WSRequest" );

        ByteBuffer data = DED.GET_ENCODED_BYTEBUFFER_DATA();

        /**
         * send to server with client current client session connection
         */
        mockTestServer.sendToServer(data);


        /**
         * wait for incomming data response, then receive data - or timeout
         */
        byte[] receivedData = mockTestServer.receiveFromServer();


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
                (strMethod   = DED2.GET_METHOD ( "name" )).length()>0 &&
                (uTrans_id     = DED2.GET_USHORT ( "trans_id")) !=-1 &&
                (strProtocolTypeID  = DED2.GET_STDSTRING ( "protocolTypeID")).length()>0 &&
                (strFunctionName    = DED2.GET_STDSTRING ( "functionName")).length()>0 &&
                (strStatus  = DED2.GET_STDSTRING ( "status")).length()>0 &&
            DED2.GET_STRUCT_END( "WSResponse" )==1)
        {
            bDecoded=true;
            System.out.println("DED packet decoded - now validate");

            if(!strMethod.equals("JavaConnect")) bDecoded=false;
            if(uTrans_id != trans_id) bDecoded=false;
            assertEquals(true,bDecoded);
            if(!strFunctionName.equals(uniqueId)) bDecoded=false;
            assertEquals(true,bDecoded);
            if(!strStatus.equals("ACCEPTED")) bDecoded=false;
            assertEquals(true,bDecoded);
            if(!strProtocolTypeID.equals("DED1.00.00")) bDecoded=false;

            if(bDecoded)
                System.out.println("DED packet validated - OK");

        }
        else
        {
            bDecoded=false;
        }

        assertEquals(true,bDecoded);
    }
}
