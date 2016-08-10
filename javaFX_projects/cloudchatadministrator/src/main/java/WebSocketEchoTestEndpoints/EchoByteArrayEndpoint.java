package WebSocketEchoTestEndpoints;

import javax.websocket.OnMessage;
import javax.websocket.Session;
import javax.websocket.server.ServerEndpoint;

/**
 * Created by serup on 23-12-15.
 */
@ServerEndpoint(value = "/echoBinary")
public class EchoByteArrayEndpoint {
    @OnMessage
    public byte[] onMessage(byte[] message, Session session)
    {
        System.out.print("echoBinary received a message - now echo it");
        return message;
    }
}


