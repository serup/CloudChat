package hadoop;

import hadoop.mappers.SMSCDRMapper;
import hadoop.reducers.SMSCDRReducer;
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
 * Created by serup on 19-02-16.
 */
public class SMSCDRMapperReducerTest {
    MapDriver<LongWritable, Text, Text, IntWritable> mapDriver;
    ReduceDriver<Text, IntWritable, Text, IntWritable> reduceDriver;
    MapReduceDriver<LongWritable, Text, Text, IntWritable, Text, IntWritable> mapReduceDriver;

    /*
    Following is an example to use MRUnit to unit test a Map Reduce program that does SMS CDR (call details record) analysis.

    The records look like

    CDRID;CDRType;Phone1;Phone2;SMS Status Code
    655209;1;796764372490213;804422938115889;6
            353415;0;356857119806206;287572231184798;4
            835699;1;252280313968413;889717902341635;0
    */

    @Before
    public void setUp() {
        SMSCDRMapper mapper = new SMSCDRMapper();
        SMSCDRReducer reducer = new SMSCDRReducer();
        mapDriver = MapDriver.newMapDriver(mapper);
        reduceDriver = ReduceDriver.newReduceDriver(reducer);
        mapReduceDriver = MapReduceDriver.newMapReduceDriver(mapper, reducer);
    }

    @Test
    public void testMapper() {
        mapDriver.withInput(new LongWritable(), new Text(
                "655209;0;796764372490213;804422938115889;6"));
        //mapDriver.withOutput(new Text("6"), new IntWritable(1));
        mapDriver.runTest();
        assertEquals("Expected 1 counter increment", 1, mapDriver.getCounters()
                .findCounter(SMSCDRMapper.CDRCounter.NonSMSCDR).getValue());
    }
    @Test
    public void testReducer() {
        List<IntWritable> values = new ArrayList<IntWritable>();
        values.add(new IntWritable(1));
        values.add(new IntWritable(1));
        reduceDriver.withInput(new Text("6"), values);
        reduceDriver.withOutput(new Text("6"), new IntWritable(2));
        reduceDriver.runTest();
    }

    @Test
    public void testMapReduce() {
        // example from here: https://cwiki.apache.org/confluence/display/MRUNIT/MRUnit+Tutorial
        mapReduceDriver.withInput(new LongWritable(), new Text(
                "655209;1;796764372490213;804422938115889;6"));
        List<IntWritable> values = new ArrayList<IntWritable>();
        values.add(new IntWritable(1));
        values.add(new IntWritable(1));
        mapReduceDriver.withOutput(new Text("6"), new IntWritable(1));
        mapReduceDriver.runTest();
    }
}
