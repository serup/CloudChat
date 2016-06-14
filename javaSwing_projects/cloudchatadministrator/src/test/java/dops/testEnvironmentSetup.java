package dops;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;

/**
 * Created by serup on 09-05-16.
 */
public class testEnvironmentSetup {
    public boolean setupTemporaryFolder() {
        boolean bResult=true;
        try {
            String path = new File(".").getCanonicalPath();
            String cmd = "mkdir temp";
            String result = executeCommand(cmd, path);
            System.out.println(result);
        } catch (Exception e) {
            e.printStackTrace();
            bResult=false;
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


}