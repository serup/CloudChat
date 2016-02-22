package hadoop;

import hadoop.hadoopMappers.hadoopDOPsFSMapper;
import hadoop.hadoopReducers.hadoopDOPsFSReducer;
import hadoop.hadoopFSHandlers.hadoopDOPsFSHandler;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

import static junit.framework.TestCase.assertEquals;

/**
 * Created by serup on 22-02-16.
 */
public class HandleHadoopFSTest {

    MapDriver<LongWritable, Text, Text, IntWritable> mapDriver;
    ReduceDriver<Text, IntWritable, Text, IntWritable> reduceDriver;
    MapReduceDriver<LongWritable, Text, Text, IntWritable, Text, IntWritable> mapReduceDriver;

    /*
    */

    @Before
    public void setUp() {
        hadoopDOPsFSMapper mapper = new hadoopDOPsFSMapper();
        hadoopDOPsFSReducer reducer = new hadoopDOPsFSReducer();
        mapDriver = MapDriver.newMapDriver(mapper);
        reduceDriver = ReduceDriver.newReduceDriver(reducer);
        mapReduceDriver = MapReduceDriver.newMapReduceDriver(mapper, reducer);
    }

    @Test
    public void test_hdfs_ls() {
        List<String> itemsToAdd = new ArrayList<String>();
        itemsToAdd.add("<empty>");
        assertEquals("Expected <empty> files in hdfs ls list", itemsToAdd, hadoopDOPsFSHandler.hadoopFS_ls());
    }

}
