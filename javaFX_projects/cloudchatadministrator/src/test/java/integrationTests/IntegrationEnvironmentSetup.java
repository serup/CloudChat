package integrationTests;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
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
                System.out.println("Check if backend is running...");
                String path = new File(".").getCanonicalPath();
                path = trimOffLastFileSeperator(path, 2);
                String cmd = "ping backend.scanva.com -c 1";
                String result = executeCommand(cmd, path);
                if(!result.contains("1 received")) {

                    // Make sure VM is running
                    System.out.println("Waiting for VM to start ...");

                    cmd = "vagrant --version";
                    result = executeCommand(cmd, path);
                    assertEquals(true, result.contains("Vagrant"));

                    /// if this does not work, then it could be because of download issues
                    /// try this:
                    ///
                    /// C:\Users<username>.vagrant.d\tmp*
                    /// After you delete it, you can re-run vagrant up and everything should work fine.
                    ///
                    cmd = "vagrant up backend";
                    result = executeCommand(cmd, path);
                    if(result=="") {
                        cmd = "vagrant resume backend";
                        result = executeCommand(cmd, path);
                    }
                    assertEquals(true, containsAny(result, new String[]{"VM is already running", "Machine booted and ready"}));
                    System.out.println("VM is started - Integration Environment ready");
                }
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
                System.out.println("Check if main node in hadoop cluster is running...");
                System.out.println("* if cluster is corrupt then do following: ");
                System.out.println("* vagrant ssh one ");
                System.out.println("* sudo -su hdfs ");
                System.out.println("* hadoop  fsck /");
                System.out.println("* hadoop  dfsadmin -safemode leave");
                System.out.println("* hadoop  -rmr -skipTrash /tmp/input");
                System.out.println("* hadoop  fsck -files delete /");
                System.out.println("* hadoop  fsck /");
                System.out.println("*  - status should be HEALTHY now - then manually restart everything in Ambari one.cluster:8080 admin admin");
                String path = new File(".").getCanonicalPath();
                path = trimOffLastFileSeperator(path, 3);
                String cmd = "ping one.cluster -c 1";
                String result = executeCommand(cmd, path);
                if(!result.contains("1 received")) {

                    // Make sure VM is running for this test
                    System.out.println("Waiting for VM hadoop one.cluster to start ... - perhaps you should start a minimum of three yourself, and make sure ambari-server and ambari-agent on all nodes is running - please " +
                            "wait it " +
                            "takes a long time...");
                    System.out.println("NB! DO NOT use vagrant halt, when shutting down a cluster - use vagrant suspend and vagrant resume - otherwise you will have to use ambari-server one.cluster:8080 admin admin to " +
                            "reestablish connection between all nodes in cluster - its takes long time");
                    path = path.concat("/hadoop_projects/hadoopServices");

                    cmd = "vagrant --version";
                    result = executeCommand(cmd, path);
                    assertEquals(true, result.contains("Vagrant"));

                    cmd = "vagrant status one.cluster";
                    result = executeCommand(cmd, path);
                    if (result.isEmpty() || !result.contains("running")) {
                        System.out.println("VM hadoop cluster was not present - now if vagrant up takes too long time, then perhaps nodes needs to be started manually and ambari installed againg");
                        // cmd = "vagrant up"; // will start all nodes described in Vagrantfile
                        cmd = "vagrant up one"; // will start 1 nodes
                        result = executeCommand(cmd, path);
                        assertEquals(true, containsAny(result, new String[]{"VM is already running", "Machine booted and ready"}));
                        cmd = "vagrant up two"; // will start 1 nodes
                        result = executeCommand(cmd, path);
                        assertEquals(true, containsAny(result, new String[]{"VM is already running", "Machine booted and ready"}));
                        System.out.println("VM hadoop cluster is started - now run test");
                    } else {
                        System.out.println("VM hadoop cluster already started - now run test");
                    }
                }
                else
                    System.out.println("hadoop cluster is running - outside virtualbox, thus no need for vagrant up - now run test ");

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

    public String executeCmd(String command, String path)
    {
        return executeCommand(command, path);
    }

    public void delete(File file) throws IOException {

        // NB! if this does not work then manually try this :
        // hadoop -skipTrash fs -rmr /tmp/input/findprofile
        //
        if(file.isDirectory()){

            //directory is empty, then delete it
            if(file.list().length==0){

                file.delete();
                System.out.println("Directory is deleted : "
                        + file.getAbsolutePath());

            }else{

                //list all the directory contents
                String files[] = file.list();

                for (String temp : files) {
                    //construct the file structure
                    File fileDelete = new File(file, temp);

                    //recursive delete
                    delete(fileDelete);
                }

                //check the directory again, if empty then delete it
                if(file.list().length==0){
                    file.delete();
                    System.out.println("Directory is deleted : "
                            + file.getAbsolutePath());
                }
            }

        }else{
            //if file, then delete it
            file.delete();
            System.out.println("File is deleted : " + file.getAbsolutePath());
        }
    }

}
