package dops.protocol;

import org.junit.Test;

import static org.junit.Assert.*;

/**
 * Created by serup on 05-01-16.
 */
public class DOPSTest {

    @Test
    public void testLoginToServer() throws Exception {

        DOPS dops = new DOPS();
        String url = "backend.scanva.com";
        String port = "7777";
        String uniqueId = "985998707DF048B2A796B44C89345494";
        String username = "johndoe@email.com"; // TODO: find a way to safely handle retrieval of username,password - should NOT be stored in source code
        String password = "12345";

        assertEquals(false,dops.loginToServer(url,port,uniqueId,username,password)); // expect false, since user is NOT registered in DFD
    }
}