package JavaServicesApp.ClientMessageHandler;

import JavaServicesApp.ProtocolHandlings.DOPsCommunication;

import javax.websocket.MessageHandler;
import java.util.concurrent.CountDownLatch;

import static JavaServicesApp.ProtocolHandlings.DOPsCommunication.decodeIncomingDED;

/**
 * Created by serup on 6/14/16.
 */
public final class DEDMessageHandler implements MessageHandler.Whole<byte[]>
{
    public static CountDownLatch messageLatch;
    public byte[] receivedData=null;
    private DOPsCommunication dops = new DOPsCommunication();

    @Override
    public void onMessage(byte[] message) {
        System.out.println("Received message of length : "+message.length);
        System.out.println("- determine if message is of type DED protocol");
        try {
            DOPsCommunication.dedAnalyzed dana = decodeIncomingDED(message);
            if(dana.bDecoded)
                System.out.println("- message was a valid DED package.");
            else
                System.out.println("- message was <unknown>.");
        } catch (Exception e) {
            //e.printStackTrace();
            System.out.println("- message was NOT a valid DED package.");
        }
        this.messageLatch.countDown();
        receivedData = message;
    }
}