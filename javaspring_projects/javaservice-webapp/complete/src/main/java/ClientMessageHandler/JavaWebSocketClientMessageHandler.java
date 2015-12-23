package ClientMessageHandler;

import javax.websocket.MessageHandler;
import java.util.concurrent.CountDownLatch;

/**
 * Created by serup on 23-12-15.
 */
public final class JavaWebSocketClientMessageHandler implements MessageHandler.Whole<byte[]>
{
    public static CountDownLatch messageLatch;
    public byte[] receivedData=null;

    @Override
    public void onMessage(byte[] message) {
        System.out.println("Received message of length : "+message.length);
        this.messageLatch.countDown();
        receivedData = message;
    }

}
