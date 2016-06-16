package JavaServicesApp.ClientMessageHandler;

import JavaServicesApp.ProtocolHandlings.DOPsCommunication;
import JavaServicesApp.ProtocolHandlings.DOPsCommunication.dedAnalyzed;
import dops.protocol.DOPS;

import javax.websocket.MessageHandler;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;
import java.util.function.BiFunction;
import java.util.function.Function;

import static JavaServicesApp.ProtocolHandlings.DOPsCommunication.decodeIncomingDED;

/**
 * Created by serup on 6/14/16.
 */
public final class DEDMessageHandler implements MessageHandler.Whole<byte[]>
{
    public static CountDownLatch messageLatch;
    public static CountDownLatch dedLatch;
    public byte[] receivedData=null;
    public dedAnalyzed dana=null;
    private static Thread dedDistributerThread=null;
    public List<dedAnalyzed> danaList = new ArrayList<>();
    ReentrantLock lock = new ReentrantLock();
    public BiFunction<String, dedAnalyzed, String> DEDHandlerFunction;

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
                            for (dedAnalyzed dana : danaList) {
                                switch (dana.type) {
                                    case "ChatInfo":

                                        try { DEDHandlerFunction.apply(dana.type, dana);
                                        }catch(Exception e) {
                                            System.out.println("- ERROR: DED handler function was a NULL pointer");
                                            //DEDHandlerFunction = (k, v) -> v == null ? "ERROR ded is null" : defaultHandler(dana.type,dana);
                                            DEDHandlerFunction = (k, v) -> defaultHandler(dana.type,dana);
                                        }
                                        break;
                                    default:
                                        System.out.println("- Currently NO handler for this type");
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
                    System.out.println("ERROR: There will be NO distribution of DED packages due to Exception !!! ");
                }
            }

            public void stopDistribution() { bContinue = false;  }
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


