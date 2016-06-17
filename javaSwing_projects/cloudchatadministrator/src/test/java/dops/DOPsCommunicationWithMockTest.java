package dops;

import JavaServicesApp.ProtocolHandlings.DOPsCommunication;
import dops.mocks.setupMockServer;
import dops.protocol.ded.DEDEncoder;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.nio.ByteBuffer;

import static dops.utils.utils.createChatInfo;
import static junit.framework.TestCase.assertEquals;
import static junit.framework.TestCase.assertTrue;

/**
 * Created by serup on 6/17/16.
 */
public class DOPsCommunicationWithMockTest {

    private static dops.mocks.setupMockServer setupMockServer =null;

    @Before
    public void setupMockServer()
    {
        /**
         * Start and connect to a MOCK server which can act as a Server and receive a DED packet and return a DED packet with result
         */
        if(setupMockServer ==null)
            setupMockServer = new setupMockServer(8047,"MockServerEndpoint");
    }

    @Test
    public void addActionHandler() throws Exception {
        DOPsCommunication dopsCommunications = new DOPsCommunication();
        assertEquals(true, setupMockServer.isOpen());

        class Ctest
        {
            boolean bCalledFunction=false;

            String actionHandlercallbackfunction(String type, DOPsCommunication.dedAnalyzed dana)
            {
                String str="OK";
                this.bCalledFunction=true;
                System.out.println("- actionHandlercallbackfunction called ");
                return str;
            }
        }

        Ctest t = new Ctest();
        String uniqueId = "983998727DF048B2A796B44C89345494";
        String username = "johndoe@email.com";
        String password = "12345";

        if(dopsCommunications.connectToDOPs(uniqueId, username, password)) {
            dopsCommunications.addActionHandler("ChatInfo", t::actionHandlercallbackfunction);

            byte[] data = createChatInfo();
            DOPsCommunication.dedAnalyzed dana = DOPsCommunication.decodeIncomingDED(data);
            Assert.assertTrue(dana.type == "ChatInfo");

            // send a test DED
            dopsCommunications.sendToServer(dana.getByteBuffer());

            // Wait for handling
            Thread.sleep(100);

            // verify that DED was analyzed and tranfered as object to action handler function
            assertTrue(t.bCalledFunction);
        }
        else
            throw new Exception("Connection with MOCK DOPS server - FAILED");
   }


}
