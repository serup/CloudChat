package config;

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
        return message;
    }
}


