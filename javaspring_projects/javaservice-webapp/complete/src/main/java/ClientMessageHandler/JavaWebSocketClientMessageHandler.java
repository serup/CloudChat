package ClientMessageHandler;

import javax.websocket.MessageHandler;

/**
 * Created by serup on 23-12-15.
 */
public final class JavaWebSocketClientMessageHandler implements MessageHandler.Whole<byte[]>
{
    @Override
    public void onMessage(byte[] message) {
        System.out.println("Received message of length : "+message.length);

    }

}
