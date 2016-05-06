package JavaServicesApp;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.util.Arrays;
import java.util.List;

import static org.junit.Assert.assertEquals;

@SpringBootApplication
public class Application {

    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);  // https://spring.io/guides/gs/spring-boot/

        // connect to DOPS and login to DFD as HadoopJavaServiceApp
        if(isEnvironmentOK()) {
            if(connectToDOPs()) {

            }
       }
    }


    private static boolean connectToDOPs(){
        boolean bResult=false;

        return bResult;
    }

    private static boolean isEnvironmentOK() {
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
}
