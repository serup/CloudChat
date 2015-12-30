import WebSocketEchoTestEndpoints.EchoByteArrayEndpoint;
import ClientEndpoint.JavaWebSocketClientEndpoint;
import WebSocketEchoTestEndpoints.EchoEndpoint;
import WebSocketEchoTestEndpoints.MockServer;
import messaging.simp.ded.DEDDecoder;
import messaging.simp.ded.DEDEncoder;
import org.glassfish.tyrus.server.Server;
import org.glassfish.tyrus.client.ClientManager;
import org.junit.Test;

import javax.websocket.*;
import javax.websocket.Endpoint;
import javax.websocket.server.ServerEndpoint;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URI;
import java.nio.ByteBuffer;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import static junit.framework.TestCase.assertEquals;

/**
 * Created by serup on 22-12-15.
 */
public class WebSocketTests {

    private static CountDownLatch messageLatch;
    private static Thread serverThread;
    String strMessageToSend="";
    String strMsgBridge=""; // used as picky-bag message for echo client server test

    public void runTxtMessageServer() {
        serverThread = new Thread() {
            public void run() {
                Server server = new Server("localhost", 8025, "/websockets", null, EchoEndpoint.class);
                try {
                    server.start();
                    BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
                    System.out.print("Please press a key to stop the server.");
                    reader.readLine();
                } catch (Exception e) {
                    e.printStackTrace();
                } finally {
                    server.stop();
                }
            }
        };
        serverThread.start();
    }

    public String responseToClient(String strmsg)
    {
        try {
            strMessageToSend=strmsg;
            messageLatch = new CountDownLatch(1);

            final ClientEndpointConfig cec = ClientEndpointConfig.Builder.create().build();
            ClientManager client = ClientManager.createClient();
            client.connectToServer(new Endpoint() {

                @Override
                public void onOpen(Session session, EndpointConfig config) {
                    try {
                        session.addMessageHandler(new MessageHandler.Whole<String>() {

                            @Override
                            public void onMessage(String message) {
                                System.out.println("Received message: "+message);
                                strMsgBridge = message;
                                messageLatch.countDown();
                            }
                        });
                        session.getBasicRemote().sendText(strMessageToSend);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }, cec, new URI("ws://localhost:8025/websockets/echo"));
            messageLatch.await(100, TimeUnit.SECONDS);

        } catch (Exception e) {
            e.printStackTrace();
        }
        return strMsgBridge;
    }

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

    class MockTestServer{

        private Thread MockserverThread;
        private JavaWebSocketClientEndpoint clientEndpoint;
        private Session session;
        private Object waitLock = new Object();
        private Server server;

        private void  wait4TerminateSignal()
        {
            synchronized(waitLock)
            {
                try {
                    waitLock.wait();
                }
                catch (InterruptedException e) { }
            }
        }

        public void runMockServer(int port) {
            MockserverThread = new Thread() {
                public void run() {
                    server = new Server("localhost", port, "/websockets", null, MockServer.class);
                    if(server!=null) {
                        try {
                            server.start();
                            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
                            System.out.println("Mock Test server ready to read incoming packets");
                            while (reader.read() != -1) ;
                        } catch (Exception e) {
                            e.printStackTrace();
                        } finally {
                            System.out.print("Mock Test Server will stop !!!");
                            stopServer();
                        }
                    }
                }
            };
            MockserverThread.start();
        }

        public void stopServer()
        {
            if(MockserverThread.isAlive() && server != null)
                server.stop();
        }

        MockTestServer(int ServerPort, String ServerEndpoint)
        {
            /**
             * start the MOCK server
             */
            this.runMockServer(ServerPort);

            /**
             * setup client
             */
            clientEndpoint = new JavaWebSocketClientEndpoint();

            /**
             * connect client to MockServer
             */
            session = clientEndpoint.connectToServer("ws://localhost:" + ServerPort + "/websockets/" + ServerEndpoint);


        }

        public boolean isOpen()
        {
            if(session!=null)
                return session.isOpen();
            else
                return false;
        }

       public int sendToServer(ByteBuffer DEDpacket)
        {
            int iResult=-1;
            if(isOpen())
                iResult = clientEndpoint.sendToServer(DEDpacket);
            return iResult;
        }

        public byte[] receiveFromServer()
        {
            byte[] IncomingPacket=null;
            if(isOpen())
                IncomingPacket = clientEndpoint.receiveFromServer();
            return IncomingPacket;
        }
    }


    @Test
    public void testClientServerEcho() throws Exception {


        /**
         * First start a server
         */
        runTxtMessageServer();

        /**
         * Second connect to it as a java websocket client, and send a message, expecting it to echo back
         */
        String strReceivedMsg = responseToClient("Hello World");

        assertEquals("Hello World",strReceivedMsg);

    }

    @Test
    public void testClientServerBinaryEcho() throws Exception {

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
    public void testLoginToMockServer() throws Exception {

        /**
         * First start and connect to a MOCK server which can act as a Server and receive a DED packet and return a DED packet with result
         */
        MockTestServer mockTestServer = new MockTestServer(8044,"MockServerLogin");

        assertEquals(true,mockTestServer.isOpen());

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
        mockTestServer.sendToServer(data);


        //TODO: create method wait for incoming datapacket

        /**
         * wait for incomming data response, then receive data
         */
        byte[] receivedData = mockTestServer.receiveFromServer();



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
}
