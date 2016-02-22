package hadoop.hadoopFSHandlers;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.FsShell;
import org.apache.hadoop.fs.Path;

import java.io.IOException;
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
}
