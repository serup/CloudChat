package WebSocketEchoTestEndpoints;
import javax.websocket.OnMessage;
import javax.websocket.Session;
import javax.websocket.server.ServerEndpoint;

/**
 * Created by serup on 30-12-15.
 *
 * this class simulates a DOPS system login - login to pro actor server, which logs in to DFD backend database profile and then response
 */
@ServerEndpoint(value = "/MockServerLogin")
public class MockServer {
    @OnMessage
    public byte[] onMessage(byte[] dedpacket, Session session)
    {
        System.out.println("MockServerLogin endpoint received dedpacket - now handle it, and send a response dedpacket ");

        // 1. decode login packet

        // 2. determine what to respond

        // 3. create response packet

        // 4. send response packet
        byte[] dedResponsePacket=null;

        //TODO: make mockserver
        dedResponsePacket=dedpacket; // tmp echo

        return dedResponsePacket;
    }
}

