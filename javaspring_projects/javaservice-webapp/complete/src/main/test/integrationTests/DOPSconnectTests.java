package integrationTests;

import ClientEndpoint.JavaWebSocketClientEndpoint;
import org.junit.Test;

/**
 * Created by serup on 31-12-15.
 */
public class DOPSconnectTests {

    @Test
    public void testConnectToDFD()
    {
        /**
        * setup client
        */
        JavaWebSocketClientEndpoint clientEndpoint = new JavaWebSocketClientEndpoint();

        /**
         * connect client
         */
        clientEndpoint.connectToServer("ws://localhost:7777");

        /**
         * prepare data to be send
         */


    }


}
