package integrationTests;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.util.Arrays;
import java.util.List;

import static org.junit.Assert.assertEquals;

/**
 * Created by serup on 08-01-16.
 */
public class IntegrationEnvironmentSetup {

    boolean bIntegrationEnvironmentAlreadySetup=false;
    boolean bHadoopIntegrationEnvironmentAlreadySetup=false;

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

    public boolean setupIntegrationEnvironment()
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

    public boolean setupHadoopIntegrationEnvironment()
    {
        boolean bResult=true;
        if (!bHadoopIntegrationEnvironmentAlreadySetup) {
            try {
                // Make sure VM is running for this test
                System.out.println("Waiting for VM hadoop cluster to start ...");

                String path = new File(".").getCanonicalPath();
                path = trimOffLastFileSeperator(path, 3);
                path = path.concat("/hadoop_projects/hadoopServices/ambari-vagrant/centos6.4");

                String cmd = "vagrant --version";
                String result = executeCommand(cmd, path);
                assertEquals(true, result.contains("Vagrant"));

                cmd = "vagrant status c6401";
                result = executeCommand(cmd, path);
                if (result.isEmpty() || !result.contains("running"))
                {
                    // cmd = "vagrant up"; // will start all nodes described in Vagrantfile
                    cmd = "./up.sh 3"; // will start 3 nodes
                    result = executeCommand(cmd, path);
                    assertEquals(true, containsAny(result, new String[]{"VM is already running", "Machine booted and ready"}));
                    System.out.println("VM hadoop cluster is started - now run test");
                }
                else {
                    System.out.println("VM hadoop cluster already started - now run test");
                }
            }catch(Exception e){
                e.printStackTrace();
                bResult = false;
            }
            bHadoopIntegrationEnvironmentAlreadySetup = bResult;
        }
        else
            System.out.println("- Hadoop Integration Environment already setup");

        return bResult;
    }

}
