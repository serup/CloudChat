package Mocks;

import ClientEndpoint.JavaWebSocketClientEndpoint;
import WebSocketEchoTestEndpoints.MockServer;
import org.glassfish.tyrus.server.Server;

import javax.websocket.MessageHandler;
import javax.websocket.Session;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;

/**
 * Created by serup on 08-01-16.
 */
public class MockTestServer{

    public boolean bIsRunning=false;
    private Thread MockserverThread;
    public JavaWebSocketClientEndpoint clientEndpoint;
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
                    bIsRunning=false;
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

    public MockTestServer(int ServerPort, String ServerEndpoint)
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



        bIsRunning=true;

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

