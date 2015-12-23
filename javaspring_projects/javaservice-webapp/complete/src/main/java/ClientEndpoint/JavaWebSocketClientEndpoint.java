package ClientEndpoint;

import ClientMessageHandler.JavaWebSocketClientMessageHandler;

import javax.websocket.Endpoint;
import javax.websocket.EndpointConfig;
import javax.websocket.Session;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Created by serup on 23-12-15.
 */
public final class JavaWebSocketClientEndpoint extends Endpoint
{
    public Session clientSession;
    JavaWebSocketClientMessageHandler msgHandler;

    public JavaWebSocketClientEndpoint()
    {
        msgHandler = new JavaWebSocketClientMessageHandler();
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
}
