package integrationTests.hadoop;

import dops.hadoop.handlers.hadoopDOPsFSHandler;
import dops.hadoop.mappers.hadoopDOPsFSMapper;
import dops.hadoop.reducers.hadoopDOPsFSReducer;
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


/**
 * Created by serup on 04-04-16.
 */
public class HadoopDOPsMapReduceTest {

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
    public void testMapReduce() {
    }

}
