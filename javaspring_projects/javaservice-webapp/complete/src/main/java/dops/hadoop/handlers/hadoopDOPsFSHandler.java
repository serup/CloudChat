package dops.hadoop.handlers;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
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
    public URI uri;
    FileSystem fs;

    private String executeCommand(String command, String path) {

        StringBuffer output = new StringBuffer();
        File dir = new File(path);

        // create a process and execute cmd
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
        conf = new Configuration();
        conf.set("fs.defaultFS", "hdfs://one.cluster:8020/");

        // this should be like defined in your mapred-site.xml
        conf.set("mapred.job.tracker", "two.cluster:50030");
        //conf.set("mapred.job.tracker", "hdfs://two.cluster:50030");
        // like defined in hdfs-site.xml
        conf.set("fs.default.name", "hdfs://one.cluster:50070");
        conf.set("hadoop.job.ugi", "root, supergroup");
        conf.set("dfs.block.local-path-access.user", "gpadmin,hdfs,mapred,yarn,hbase,hive,serup,root");
        conf.set("dfs.client.read.shortcircuit", "true");

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

    public boolean copyTo(String inputfile, String destinationHDFSPath) {
        boolean bResult = true;

        String fileName = inputfile.substring( inputfile.lastIndexOf('/')+1, inputfile.length() );
        // put file into hdfs at destinationHDFSPath
        try {
            fs = FileSystem.get(uri, conf); // fetch filesystem handle for hdfs on one.cluster server
            Path dest = new Path(destinationHDFSPath);
            fs.copyFromLocalFile(new Path(inputfile), dest);
        } catch (IOException e) {
            bResult = false;
            e.printStackTrace();
        }

        // cleanup - when testing
        //String newfileName = destinationHDFSPath + "/" + fileName;
        ////fs.deleteOnExit(new Path(newfileName));
        //remove(newfileName);

        return bResult;
    }
}
