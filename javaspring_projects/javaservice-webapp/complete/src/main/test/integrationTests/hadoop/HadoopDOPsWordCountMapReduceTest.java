package integrationTests.hadoop;

import hadoop.hadoopFSHandlers.hadoopDOPsFSHandler;
import hadoop.hadoopMappers.WordMapper;
import hadoop.hadoopReducers.WordReducer;
import integrationTests.IntegrationEnvironmentSetup;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

import static junit.framework.TestCase.assertEquals;

/**
 * Created by serup on 04-04-16.
 */

// http://stackoverflow.com/questions/9849776/calling-a-mapreduce-job-from-a-simple-java-program
// should find count the word 'Watson' from a created file on the one.cluster node in the DOPs hadoop system
public class HadoopDOPsWordCountMapReduceTest {

    hadoopDOPsFSHandler fshandlerDriver;

    IntegrationEnvironmentSetup env = new IntegrationEnvironmentSetup();

    @Before
    public void setUp() throws Exception {
        Assert.assertEquals(true,env.setupHadoopIntegrationEnvironment());
        fshandlerDriver = new hadoopDOPsFSHandler();
    }

    @Test
    public void testMapReduce() throws Exception {

        // create a configuration
        Configuration conf = new Configuration();
        // this should be like defined in your mapred-site.xml
        conf.set("mapred.job.tracker", "two.cluster:50030");
        // like defined in hdfs-site.xml
        conf.set("fs.default.name", "hdfs://one.cluster:50070");
        conf.set("fs.defaultFS", "hdfs://one.cluster:8020/");
        conf.set("hadoop.job.ugi", "root, supergroup");

        // create a new job based on the configuration
        Job job = new Job(conf);
        // here you have to put your mapper class
        job.setMapperClass(WordMapper.class);

        // here you have to put your reducer class
        job.setReducerClass(WordReducer.class);
        // here you have to set the jar which is containing your
        // map/reduce class, so you can use the mapper class
        job.setJarByClass(WordMapper.class);

        // key/value of your reducer output
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);

        // this is setting the format of your input, can be TextInputFormat
//        job.setInputFormatClass(SequenceFileInputFormat.class);
        //job.setInputFormatClass(FileInputFormat.class);
        FileInputFormat.setInputPaths(job, "/tmp/input/wordcount/");

        // same with output
        //job.setOutputFormatClass(TextOutputFormat.class);
        FileOutputFormat.setOutputPath(job, new Path("/tmp/output/wordcount/"));

        // here you can set the path of your input
//        SequenceFileInputFormat.addInputPath(job, new Path("hdfs://one.cluster:50070/tmp/input/wordcount/"));
//        SequenceFileInputFormat.addInputPath(job, new Path("/tmp/input/wordcount/"));
//        FileInputFormat.addInputPath(job, new Path("/tmp/input/wordcount/"));

        // this deletes possible output paths to prevent job failures
        fs = FileSystem.get(uri, conf); // fetch filesystem handle for hdfs on one.cluster server
        FileSystem fs = FileSystem.get(conf);
        Path out = new Path("/tmp/output/wordcount/");
        fs.delete(out, true);

        // finally set the empty out path
        TextOutputFormat.setOutputPath(job, out);

        try {
            String fileResource="watson.txt";
            String destFolder="tmp/input/wordcount";
            URL fileResourceUrl = this.getClass().getClassLoader().getResource(fileResource);
            // copy the internal resource file watson.txt to remote hadoop hdfs system
            fshandlerDriver.copyTo(fileResourceUrl.getPath(), "/"+destFolder);

            // verify that the input file is in hdfs
            List<String> itemsToAdd = new ArrayList<String>();
            String filepathname = destFolder+"/"+fileResource;
            itemsToAdd.add(fshandlerDriver.uri+filepathname);
            assertEquals("Expected 1 item in hdfs ls list", itemsToAdd, fshandlerDriver.ls("/"+filepathname));


            // MapReduce job start
            // this waits until the job completes and prints debug out to STDOUT or whatever
            // has been configured in your log4j properties.
            job.waitForCompletion(true);


            // cleanup - when after testing - the input file will be deleted on exit
            String newfileName = "/" + destFolder + "/" + fileResource;
            fshandlerDriver.remove(newfileName);
        } catch (IOException e) {
            e.printStackTrace();
        }



    }

}
