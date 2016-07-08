package dops.mocks;

import ClientEndpoint.JavaWebSocketClientEndpoint;
import org.glassfish.tyrus.server.Server;

import javax.websocket.Session;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;

/**
 * Created by serup on 08-01-16.
 */
public class setupMockServer {

    public boolean bIsRunning=false;
    private Thread MockserverThread;
    public JavaWebSocketClientEndpoint clientEndpoint;
    private Session session;
    private Object waitLock = new Object();
    private Server server;
    private int ServerPort;

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
                        while (reader.read() != -1) ;
                    } catch (Exception e) {
                    } finally {
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
        if(MockserverThread.isAlive() && server != null) {
            server.stop();
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public Session connectToMockServer()
    {
        /**
         * connect client to MockServer
         */
        session = clientEndpoint.connectToServer(getMockServerURI());
        return session;
    }

    // constructor
    public setupMockServer(int ServerPort)
    {
        this.ServerPort = ServerPort;

        /**
         * start the MOCK server
         */
        this.runMockServer(ServerPort);

        /**
         * setup client
         */
        clientEndpoint = new JavaWebSocketClientEndpoint();


    }

    public String getMockServerURI() {
        return "ws://localhost:" + ServerPort + "/websockets/" + "MockServerEndpoint";
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
            IncomingPacket = clientEndpoint.receiveMessageFromServer();
        return IncomingPacket;
    }
}

