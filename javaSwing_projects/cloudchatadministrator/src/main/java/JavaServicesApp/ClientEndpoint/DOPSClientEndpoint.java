package JavaServicesApp.ClientEndpoint;

import JavaServicesApp.ClientMessageHandler.DEDMessageHandler;

import javax.websocket.EndpointConfig;
import javax.websocket.Session;
import java.io.IOException;
import java.net.URI;
import java.nio.ByteBuffer;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

/**
 * Created by serup on 6/15/16.
 */
public class DOPSClientEndpoint extends JavaWebSocketClientEndpoint
{
    public Session clientSession;
    DEDMessageHandler msgHandler;

    public DOPSClientEndpoint()
    {
        msgHandler = new DEDMessageHandler();
        msgHandler.messageLatch = new CountDownLatch(1);
    }

    @Override
    public void onOpen(Session session, EndpointConfig config) {
        try {
            session.addMessageHandler(msgHandler);
            clientSession=session;
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public Session connectToServer(String strURI)
    {
        Session session=null;
        try {
            session = client.connectToServer(this, cec, new URI(strURI));
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        return session;
    }

    public int sendToServer(ByteBuffer data)
    {
        int result=-1;
        if(clientSession.isOpen())
        {
            try {
                clientSession.getBasicRemote().sendBinary(data);
                result = data.array().length;
            }
            catch (IOException e)
            {
                e.printStackTrace();
            }
        }
        return result;
    }

    public byte[] receiveFromServer()
    {
        byte[] data = null;
        try{
            if(msgHandler.messageLatch.await(100, TimeUnit.SECONDS)) // wait for incoming data -- data will arrive in JavaWebSocketClientMessageHandler onMessage and latch will be decreased to zero
            {
                data = msgHandler.receivedData;
            }
            else
            {
                System.out.println("WARNING - Timeout when trying to receive data from server - NO data received");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        msgHandler.messageLatch = new CountDownLatch(1); // prepare for next incoming packet
        return data;
    }
}
