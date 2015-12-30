package ClientEndpoint;

import ClientMessageHandler.JavaWebSocketClientMessageHandler;
import org.glassfish.tyrus.client.ClientManager;

import javax.websocket.ClientEndpointConfig;
import javax.websocket.Endpoint;
import javax.websocket.EndpointConfig;
import javax.websocket.Session;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.ByteBuffer;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

/**
 * Created by serup on 23-12-15.
 */
public final class JavaWebSocketClientEndpoint extends Endpoint
{
    public Session clientSession;
    JavaWebSocketClientMessageHandler msgHandler;
    final ClientEndpointConfig cec;
    ClientManager client;

    public JavaWebSocketClientEndpoint()
    {
        cec = ClientEndpointConfig.Builder.create().build();
        client = ClientManager.createClient();
        msgHandler = new JavaWebSocketClientMessageHandler();
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
            msgHandler.messageLatch.await(100, TimeUnit.SECONDS); // wait for incomming data
            data = msgHandler.receivedData;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return data;
    }
}
