import config.EchoByteArrayEndpoint;
import config.EchoEndpoint;
import junit.framework.Assert;
import org.glassfish.tyrus.server.Server;
import org.glassfish.tyrus.client.ClientManager;
import org.junit.Test;

import javax.validation.constraints.AssertTrue;
import javax.websocket.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URI;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import static junit.framework.TestCase.assertEquals;

/**
 * Created by serup on 22-12-15.
 */
public class WebSocketTests {

    private static CountDownLatch messageLatch;
    private static final String SENT_MESSAGE = "Hello World";
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
}
