package hadoop.hadoopFSHandlers;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.FsShell;
import org.apache.hadoop.fs.Path;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by serup on 22-02-16.
 */
public class hadoopDOPsFSHandler {
    /**
     * Should handle all commands going to the hdfs
     */
    Configuration conf;
    URI uri;
    FsShell fileSystem;
    FileSystem fs;

    private String executeMultiCommand(String[] command, String path) {

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


    public hadoopDOPsFSHandler() // constructor
    {
        //fileSystem = new org.apache.hadoop.fs.FsShell();
        conf = new Configuration();
        conf.set("fs.defaultFS", "hdfs://one.cluster:8020/");
        uri = URI.create ("hdfs://one.cluster:8020/");
    }


    public List<String> ls(String strPath){
        List<String> itemsToAdd = new ArrayList<String>();

        FileSystem fs = null;
        try {
            fs = FileSystem.get(uri, conf);
        } catch (IOException e) {
            e.printStackTrace();
        }
        Path filePath = new Path(strPath); // root = "/"
        FileStatus[] fileStatuses = null;
        try {
            fileStatuses = fs.listStatus(filePath);
            for(FileStatus status : fileStatuses) {
               itemsToAdd.add(status.getPath().toString());
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return itemsToAdd;
    }

    public boolean touch(String strFilepathname) throws IOException {
        boolean bResult;
        fs = FileSystem.get(uri, conf);
        Path f = new Path(strFilepathname);
        bResult = fs.createNewFile(f);
        return bResult;
    }

    public boolean remove(String strFilepathname) throws IOException {
        boolean bResult;
        fs = FileSystem.get(uri, conf);
        Path f = new Path(strFilepathname);
        bResult = fs.deleteOnExit(f);
        return bResult;
    }

    public boolean copyTo(String inputfile, String destinationHDFSPath) throws IOException {
        boolean bResult = false;

        // init
        String path = new File(".").getCanonicalPath();
        path = trimOffLastFileSeperator(path, 3);
        path = path.concat("/hadoop_projects/hadoopServices"); // focus on current path

        // first make a scp copy to one.cluster - use script, since sshpass is used and multi cmd from here does NOT work
        String cmd = "./scpToHadoop.sh " + inputfile;
        String result = executeCommand(cmd, path);

        // second put file into hdfs at destinationHDFSPath
        String fileName = inputfile.substring( inputfile.lastIndexOf('/')+1, inputfile.length() );


        return bResult;
    }
}
