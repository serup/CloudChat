package dops.websocket;

import ProtocolHandlings.DOPsCommunication;
import dops.mocks.setupMockServer;
import junit.framework.TestCase;
import org.junit.*;
import org.junit.runner.Result;
import org.junit.runner.notification.RunListener;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import static dops.utils.utils.createChatInfo;
import static junit.framework.TestCase.assertEquals;
import static junit.framework.TestCase.assertTrue;

/**
 * Created by serup on 6/17/16.
 */
public class DOPsCommunicationWithMockTest extends RunListener {

    private static dops.mocks.setupMockServer setupMockServer =null;
    private static int mockServerPort = 8045;
    private static String mockServerURI = "ws://localhost:"+ mockServerPort;
    private static DOPsCommunication dopsCommunications;

    @BeforeClass
    public static void doOneTimeSetupMockServer()
    {
        /**
         * Start and connect to a MOCK server which can act as a Server and receive a DED packet and return a DED packet with result
         */
        if(setupMockServer == null) {
            setupMockServer = new setupMockServer(mockServerPort);
            mockServerURI = setupMockServer.getMockServerURI();
            setupMockServer.connectToMockServer();
            dopsCommunications = new DOPsCommunication();
            dopsCommunications.setServerURI(mockServerURI);
        }
        assertEquals(true, setupMockServer.isOpen());
    }


    @Test
    public void addActionHandler() throws Exception {

        class Ctest
        {
            private boolean bCalledFunction=false;
            private CountDownLatch signalEvent = new CountDownLatch(1);

            private String actionHandlercallbackfunction(String type, DOPsCommunication.dedAnalyzed dana)
            {
                String str="OK";
                this.bCalledFunction=true;
                System.out.println("- actionHandlercallbackfunction called ");
                Assert.assertTrue(dana.type == type);
                signalEvent.countDown(); // signal  a valid DED has arrived
                return str;
            }
        }

        Ctest t = new Ctest();
        String uniqueId = "985998707DF048B2A796B44C89345494";
        String username = "johndoe@email.com";
        String password = "12345";

        if(dopsCommunications.connectToDOPs(uniqueId, username, password)) {

            /**
             * Function to be tested
             * adding an action handler, forces all incoming data traffic to be
             * transfer to the action handler function if the type is the same
             */
            dopsCommunications.addActionHandler("ChatInfo", t::actionHandlercallbackfunction);

            byte[] data = createChatInfo(uniqueId);
            DOPsCommunication.dedAnalyzed dana = DOPsCommunication.decodeIncomingDED(data);
            Assert.assertTrue(dana.type == "ChatInfo");

            // send a DED package of type ChatInfo
            dopsCommunications.sendToServer(dana.getByteBuffer());

            // Wait for handling
            assertTrue(t.signalEvent.await(10, TimeUnit.SECONDS)); // wait for signal that valid ded has arrived in action handler function

            // verify that DED was analyzed and tranfered as object to action handler function
            assertTrue(t.bCalledFunction);
        }
        else
            throw new Exception("Connection with MOCK DOPS server - FAILED -- NB pt. mock server does NOT make correct handshake");
   }


}
