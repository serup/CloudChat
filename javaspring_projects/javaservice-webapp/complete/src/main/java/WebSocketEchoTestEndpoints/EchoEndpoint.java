package WebSocketEchoTestEndpoints;

import javax.websocket.OnMessage;
import javax.websocket.Session;
import javax.websocket.server.ServerEndpoint;

/**
 * Created by serup on 22-12-15.
 */
@ServerEndpoint(value = "/echo")
public class EchoEndpoint {
    @OnMessage
    public String onMessage(String message, Session session) {
        System.out.println("echo endpoint received message - now echo it");
        return message;
    }
}
