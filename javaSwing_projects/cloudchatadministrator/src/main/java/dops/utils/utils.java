package dops.utils;

import dops.protocol.ded.DEDEncoder;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.util.Arrays;
import java.util.List;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 * Created by serup on 6/14/16.
 */
public class utils {

    public boolean isEnvironmentOK() {
        boolean bResult=true;
        try {
            System.out.println("Check if backend is running...");
            String path = new File(".").getCanonicalPath();
            path = trimOffLastFileSeperator(path, 3);
            if (!executeCommand("ping backend.scanva.com -c 1", path).contains("1 received")) {
                System.out.println("Waiting for VM to start ...");
                assertEquals(true, executeCommand("vagrant --version", path).contains("Vagrant"));
                assertEquals(true, containsAny(executeCommand("vagrant up backend", path), new String[]{"VM is already running", "Machine booted and ready"}));
                if (!executeCommand("ping backend.scanva.com -c 1", path).contains("1 received"))
                    throw new Exception("backend.scanva.com - NOT responding to ping");
                else
                    System.out.println("VM is started - Integration Environment ready");
            }
            else
                System.out.println("backend.scanva.com is running - Integration Environment ready");

        } catch (Exception e) {
            bResult=false;
            e.printStackTrace();
        }
        return bResult;
    }



    private static String trimOffLastFileSeperator(String path, int amountToTrimOff)
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

    private static String executeCommand(String command, String path) {

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

    private static boolean containsAny(String str, String[] words)
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

    public static byte[] createChatInfo() {

        DEDEncoder DED = new DEDEncoder();
        DED.PUT_STRUCT_START ("ClientChatRequest");
        DED.PUT_METHOD ("Method", "JSCChatInfo");
        DED.PUT_USHORT ("TransID", (short)23);
        DED.PUT_STDSTRING("protocolTypeID", "DED1.00.00");
        DED.PUT_STDSTRING("dest", "ccAdmin");  // This Admins uniqueID, since this is a test it really does not matter
        DED.PUT_STDSTRING("src", "4086d4ab369e14ca1b6be7364d88cf85"); //  uniqueID
        DED.PUT_STDSTRING("srcAlias", "SERUP");
        DED.PUT_STDSTRING("srcHomepageAlias", "Montenegro");
        DED.PUT_STDSTRING("lastEntryTime", "20160101");
        DED.PUT_STRUCT_END("ClientChatRequest");
        byte[] byteArray = DED.GET_ENCODED_BYTEARRAY_DATA ();
        assertNotNull(byteArray);
        return byteArray;
    }

    public static byte[] createForwardInfoRequest() {

        DEDEncoder DED = new DEDEncoder();
        DED.PUT_STRUCT_START ("CloudManagerRequest");
        DED.PUT_METHOD ("Method", "JSCForwardInfo");
        DED.PUT_USHORT ("TransID", (short)23);
        DED.PUT_STDSTRING("protocolTypeID", "DED1.00.00");
        DED.PUT_STDSTRING("dest", "ccAdmin");  // This Admins uniqueID, since this is a test it really does not matter
        DED.PUT_STDSTRING("src", "4086d4ab369e14ca1b6be7364d88cf85"); // CloudChatManager uniqueID
        DED.PUT_STDSTRING("srcAlias", "SERUP");
        DED.PUT_STRUCT_END("CloudManagerRequest");
        byte[] byteArray = DED.GET_ENCODED_BYTEARRAY_DATA ();
        assertNotNull(byteArray);
        return byteArray;
    }


}
