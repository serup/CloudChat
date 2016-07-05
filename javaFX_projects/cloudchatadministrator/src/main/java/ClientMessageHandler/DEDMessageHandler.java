package ClientMessageHandler;

import ProtocolHandlings.DOPsCommunication.dedAnalyzed;
import javafx.application.Platform;

import javax.websocket.MessageHandler;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;
import java.util.function.BiFunction;

import static ProtocolHandlings.DOPsCommunication.decodeIncomingDED;

/**
 * Created by serup on 6/14/16.
 */
public final class DEDMessageHandler implements MessageHandler.Whole<byte[]>
{
    public static CountDownLatch messageLatch;
    public static CountDownLatch dedLatch;
    public byte[] receivedData=null;
    private static Thread dedDistributerThread=null;
    private List<dedAnalyzed> danaList = new ArrayList<>();
    private ReentrantLock WaitForlock = new ReentrantLock();
    private BiFunction<String, dedAnalyzed, String> DEDHandlerFunction;
    boolean bContinueDistributer = true;

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
            dedAnalyzed dana = decodeIncomingDED(message);
            if(dana.bDecoded) {
                System.out.println("- message was a valid DED package of type : " + dana.type);
                if(dedDistributerThread!=null){
                    addDEDtoDistributer(dana);
                }
                dedLatch.countDown(); // signal  a valid DED is ready for retrieval
            }
            else
                System.out.println("- [DEDMessageHandler] message was of type : " + dana.type);
        } catch (Exception e) {
            //e.printStackTrace();
            System.out.println("- message was NOT a valid DED package.");
        }
        receivedData = message;
        messageLatch.countDown(); // signal a valid message is ready for retrieval
    }

    public boolean addHandlerFunction(BiFunction<String, dedAnalyzed, String> customHandlerFunction)
    {
        boolean bResult = false;
        if(customHandlerFunction!=null) {
            DEDHandlerFunction = customHandlerFunction;
            bResult = true;
        }
        return bResult;
    }

    private void addDEDtoDistributer(dedAnalyzed dana) {
        WaitForlock.lock();
        danaList.add(dana);
        WaitForlock.unlock();
    }

    public void stopDEDdistributerThread()
    {
        bContinueDistributer = false; // thread should timeout an exit
    }

    private void runDEDdistributerThread() {
        dedDistributerThread = new Thread() {

            @Override
            public void run() {
                try {
                    // distribute DED objects to individual handlers
                    do
                    {
                        if(dedLatch.await(10, TimeUnit.SECONDS)) // wait for signal that valid ded has arrived
                        {
                            WaitForlock.lock();
                            for (dedAnalyzed dana : danaList) {
                                switch (dana.type) {
                                    case "ChatInfo":
                                        try {
                                            DEDHandlerFunction.apply(dana.type, dana);
                                        }catch(Exception e) {
                                            System.out.println("- ERROR: DED handler function was a NULL pointer");
                                            System.out.printf("- possible cause : %s\n", e);
                                            DEDHandlerFunction = (k, v) -> defaultHandler(dana.type,dana);
                                        }
                                        break;
                                    default:
                                        System.out.println("- Currently NO handler for this type");
                                        break;
                                }

                            }
                            danaList.clear(); // all on list have been processed
                            WaitForlock.unlock();
                        }
                        dedLatch = new CountDownLatch(1); // prepare for next signal
                    }
                    while(bContinueDistributer);
                } catch (Exception e) {
                    e.printStackTrace();
                    System.out.println("ERROR: There will be NO distribution of DED packages due to Exception !!! ");
                }
            }
        };
        dedDistributerThread.start();
    }

    public String defaultHandler(String type, dedAnalyzed dana)
    {
        String strResult = "dummy default handler";
        System.out.println("- WARNING: DED default handler function was called");
        return strResult;
    }
}


