package integrationTests.hadoop;

import hadoop.hadoopFSHandlers.hadoopDOPsFSHandler;
import hadoop.hadoopMappers.hadoopDOPsFSMapper;
import hadoop.hadoopReducers.hadoopDOPsFSReducer;
import integrationTests.IntegrationEnvironmentSetup;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import static junit.framework.TestCase.assertEquals;

/**
 * Created by serup on 22-02-16.
 */
public class HadoopDOPsFSTest {

    MapDriver<LongWritable, Text, Text, IntWritable> mapDriver;
    ReduceDriver<Text, IntWritable, Text, IntWritable> reduceDriver;
    MapReduceDriver<LongWritable, Text, Text, IntWritable, Text, IntWritable> mapReduceDriver;
    hadoopDOPsFSHandler fshandlerDriver;

    /*
    */
    IntegrationEnvironmentSetup env = new IntegrationEnvironmentSetup();

    @Before
    public void setUp() throws Exception {
        Assert.assertEquals(true,env.setupHadoopIntegrationEnvironment());
        hadoopDOPsFSMapper mapper = new hadoopDOPsFSMapper();
        hadoopDOPsFSReducer reducer = new hadoopDOPsFSReducer();
        mapDriver = MapDriver.newMapDriver(mapper);
        reduceDriver = ReduceDriver.newReduceDriver(reducer);
        mapReduceDriver = MapReduceDriver.newMapReduceDriver(mapper, reducer);
        fshandlerDriver = new hadoopDOPsFSHandler();
    }

    @Test
    public void test_hdfs_ls() {
        List<String> itemsToAdd = new ArrayList<String>();
//        itemsToAdd.add("hdfs://one.cluster:8020/apps");
//        itemsToAdd.add("hdfs://one.cluster:8020/mapred");
//        itemsToAdd.add("hdfs://one.cluster:8020/tmp");
//        itemsToAdd.add("hdfs://one.cluster:8020/user");
        itemsToAdd.add("hdfs://one.cluster:8020/mapred");
        itemsToAdd.add("hdfs://one.cluster:8020/tmp");
        itemsToAdd.add("hdfs://one.cluster:8020/user");
        assertEquals("Expected 4 items in hdfs ls list", itemsToAdd, fshandlerDriver.ls("/"));
    }

    @Test
    public void test_hdfs_touch_and_remove() {
        try {
            fshandlerDriver.touch("/tmp/newfile2.txt");
            fshandlerDriver.remove("/tmp/newfile2.txt");
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

}
