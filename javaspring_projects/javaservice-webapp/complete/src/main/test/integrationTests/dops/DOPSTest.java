package integrationTests.dops;

import dops.protocol.DOPS;
import integrationTests.IntegrationEnvironmentSetup;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

/**
 * INTEGRATION TESTS
 *
 * To run these tests, then the VM's needed must be running
 *
 * how to manually start VM's :
 *
 * vagrant up <name of VM>
 *
 * Created by serup on 05-01-16.
 */
public class DOPSTest {

    IntegrationEnvironmentSetup env = new IntegrationEnvironmentSetup();

    @Before
    public void setup() throws Exception {
        assertEquals(true,env.setupIntegrationEnvironment());
    }

    @Test
    public void testLoginToServer() throws Exception {

        DOPS dops = new DOPS();
        String url = "backend.scanva.com";
        String port = "7777";
        String uniqueId = "985998707DF048B2A796B44C89345494";
        String username = "johndoe@email.com"; // TODO: find a way to safely handle retrieval of username,password - should NOT be stored in source code
        String password = "12345";

        assertEquals(false,dops.loginToServer(url,port,uniqueId,username,password)); // expect false, since user is NOT registered in DFD
        assertEquals(true,dops.isOpen()); // even though login to profile failed, then connection to DOPS main part should have been established

        System.out.println("done - success");
    }
}