package ClientEndpoint;

import ClientMessageHandler.DEDMessageHandler;
import ProtocolHandlings.DOPsCommunication;

import javax.websocket.EndpointConfig;
import javax.websocket.Session;
import java.io.IOException;
import java.net.URI;
import java.nio.ByteBuffer;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.function.BiFunction;

/**
 * Created by serup on 6/15/16.
 */
public class DOPSClientEndpoint extends JavaWebSocketClientEndpoint
{
    public Session clientSession;
    DEDMessageHandler msgHandler;

    public DOPSClientEndpoint()
    {
        msgHandler = new DEDMessageHandler(true);
        msgHandler.messageLatch = new CountDownLatch(1);
        msgHandler.dedLatch = new CountDownLatch(1);
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

    public void stopMessageHandler()
    {
        msgHandler.stopDEDdistributerThread();
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

    public byte[] receiveMessageFromServer()
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

    public boolean addHandlerFunction(BiFunction<String, DOPsCommunication.dedAnalyzed, String> customHandlerFunction)
    {
        boolean bResult;
        try {
            bResult = msgHandler.addHandlerFunction(customHandlerFunction);
        }
        catch (Exception e)
        {
            e.printStackTrace();
            bResult = false;
        }
        return bResult;
    }

}
