import WebSocketEchoTestEndpoints.EchoByteArrayEndpoint;
import ClientEndpoint.JavaWebSocketClientEndpoint;
import WebSocketEchoTestEndpoints.EchoEndpoint;
import messaging.simp.ded.DEDEncoder;
import org.glassfish.tyrus.server.Server;
import org.glassfish.tyrus.client.ClientManager;
import org.junit.Test;

import javax.websocket.*;
import javax.websocket.Endpoint;
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
                    System.out.print("Please press a key to stop the server.");
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
        final ClientEndpointConfig cec = ClientEndpointConfig.Builder.create().build();
        ClientManager client = ClientManager.createClient();
        JavaWebSocketClientEndpoint clientEndpoint = new JavaWebSocketClientEndpoint();

        /**
         * connect client
         */
        client.connectToServer(clientEndpoint,cec, new URI("ws://localhost:8033/websockets/echoBinary"));

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
         * wait for response from server
         */

    }

}
