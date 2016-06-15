package JavaServicesApp.ClientMessageHandler;

import JavaServicesApp.ProtocolHandlings.DOPsCommunication;
import javax.websocket.MessageHandler;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;

import static JavaServicesApp.ProtocolHandlings.DOPsCommunication.decodeIncomingDED;

/**
 * Created by serup on 6/14/16.
 */
public final class DEDMessageHandler implements MessageHandler.Whole<byte[]>
{
    public static CountDownLatch messageLatch;
    public static CountDownLatch dedLatch;
    public byte[] receivedData=null;
    public DOPsCommunication.dedAnalyzed dana=null;
    private static Thread dedDistributerThread=null;
    public List<DOPsCommunication.dedAnalyzed> danaList = new ArrayList<>();
    ReentrantLock lock = new ReentrantLock();

    public DEDMessageHandler() {}

    public DEDMessageHandler(boolean bRunDistributer)
    {
        if(bRunDistributer)
            runDEDdistributerThread();
    }

    @Override
    public void onMessage(byte[] message) {
        System.out.println("Received message of length : "+message.length);
        System.out.println("- determine if message is of type DED protocol");
        try {
            dana = decodeIncomingDED(message);
            if(dana.bDecoded) {
                System.out.println("- message was a valid DED package of type : " + dana.type);
                if(dedDistributerThread!=null){
                    addDEDtoDistributer(dana);
                }
                dedLatch.countDown(); // signal  a valid DED is ready for retrieval
            }
            else
                System.out.println("- message was <unknown>.");
        } catch (Exception e) {
            //e.printStackTrace();
            System.out.println("- message was NOT a valid DED package.");
        }
        receivedData = message;
        messageLatch.countDown(); // signal a valid message is ready for retrieval
    }

    private void addDEDtoDistributer(DOPsCommunication.dedAnalyzed dana) {
        lock.lock();
        danaList.add(dana);
        lock.unlock();
    }

    private void runDEDdistributerThread() {
        dedDistributerThread = new Thread() {
            boolean bContinue = true;

            public void run() {
                try {
                    // distribute DED objects to individual handlers
                    do
                    {
                        if(dedLatch.await(100, TimeUnit.SECONDS)) // wait for signal that valid ded has arrived
                        {
                            lock.lock();
                            for (DOPsCommunication.dedAnalyzed dana : danaList) {
                                switch (dana.type) {
                                    case "ChatInfo":
                                        System.out.println("- Currently NO handlers for this type");

                                        break;
                                }

                            }
                            danaList.clear(); // all on list have been processed
                            lock.unlock();
                        }
                        dedLatch = new CountDownLatch(1); // prepare for next signal
                    }
                    while(bContinue);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

            public void stopDistribution() { bContinue = false;  }
        };
        dedDistributerThread.start();
    }

}


