package integrationTests;

import dops.protocol.DOPS;
import org.junit.Test;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Arrays;
import java.util.List;
import java.util.Scanner;

import static org.junit.Assert.*;

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

    boolean bIntegrationEnvironmentAlreadySetup=false;

    public static boolean containsAny(String str, String[] words)
    {
        boolean bResult=false; // if any of the words are found, this will be set true
        //String[] words = {"word1", "word2", "word3", "word4", "word5"};

        List<String> list = Arrays.asList(words);
        for (String word: list ) {
            boolean bFound = str.contains(word);
            if (bFound) {bResult=bFound; break;}
        }
        return bResult;
    }

    private String executeCommand(String command, String path) {

        StringBuffer output = new StringBuffer();
        File dir = new File(path);

        // create a process and execute cmd and currect environment
        try {
            Process process = Runtime.getRuntime().exec(command, null, dir);

            BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            process.waitFor();

            String line = "";
            while ((line = reader.readLine())!= null) {
                output.append(line + "\n");
            }

        } catch (Exception e) {
            e.printStackTrace();
            output.append(e.getMessage());
        }

        return output.toString();
    }

    public String trimOffLastFileSeperator(String path, int amountToTrimOff)
    {
        String separator = System.getProperty("file.separator");

        String strtmp = path;
        int lastSeparatorIndex;
        for(int i=amountToTrimOff; i>0; i--)
        {
            lastSeparatorIndex = strtmp.lastIndexOf(separator);
            strtmp = strtmp.substring(0,lastSeparatorIndex);
        }
        return strtmp;
    }

    private boolean setupIntegrationEnvironment()
    {
        boolean bResult=true;
        if (!bIntegrationEnvironmentAlreadySetup) {
            try {
                // Make sure VM is running for this test
                System.out.println("Waiting for VM to start ...");

                String path = new File(".").getCanonicalPath();
                path = trimOffLastFileSeperator(path, 3);

                String cmd = "vagrant --version";
                String result = executeCommand(cmd, path);
                assertEquals(true, result.contains("Vagrant"));

                cmd = "vagrant up backend";
                result = executeCommand(cmd, path);
                assertEquals(true, containsAny(result, new String[]{"VM is already running", "Machine booted and ready"}));

                System.out.println("VM is started - now run test");
            }catch(Exception e){
                e.printStackTrace();
                bResult = false;
            }
            bIntegrationEnvironmentAlreadySetup = bResult;
        }
        else
          System.out.println("- Integration Environment already setup");

        return bResult;
    }

    @Test
    public void testLoginToServer() throws Exception {

        DOPS dops = new DOPS();
        String url = "backend.scanva.com";
        String port = "7777";
        String uniqueId = "985998707DF048B2A796B44C89345494";
        String username = "johndoe@email.com"; // TODO: find a way to safely handle retrieval of username,password - should NOT be stored in source code
        String password = "12345";

        assertEquals(true,setupIntegrationEnvironment());
        assertEquals(false,dops.loginToServer(url,port,uniqueId,username,password)); // expect false, since user is NOT registered in DFD
        assertEquals(true,dops.isOpen()); // even though login to profile failed, then connection to DOPS main part should have been established

        System.out.println("done - success");
    }
}