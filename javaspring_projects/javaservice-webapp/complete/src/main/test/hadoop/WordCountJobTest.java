package hadoop;

import com.google.common.base.Charsets;
import com.google.common.io.Resources;
import hadoop.hadoopMappers.WordMapper;
import hadoop.hadoopReducers.WordReducer;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;
import java.nio.charset.Charset;

/**
 * Created by serup on 04-04-16.
 */

public class WordCountJobTest {

    MapDriver<LongWritable, Text, Text, IntWritable> mapDriver;
    ReduceDriver<Text, IntWritable, Text, IntWritable> reduceDriver;
    MapReduceDriver<LongWritable, Text, Text, IntWritable, Text, IntWritable> mapReduceDriver;


    @Before
    public void setUp() throws Exception {
        WordMapper mapper = new WordMapper();
        WordReducer reducer = new WordReducer();
        mapDriver = MapDriver.newMapDriver(mapper);
        reduceDriver = ReduceDriver.newReduceDriver(reducer);
        mapReduceDriver = MapReduceDriver.newMapReduceDriver(mapper, reducer);
    }

    public String readResource(final String fileName, Charset charset) throws Exception {
        try {
            return Resources.toString(Resources.getResource(fileName), charset);
        } catch (IOException e) {
            throw new IllegalArgumentException(e);
        }
    }

    @Test
    public void testWordCountMapReduce() {
        String fileResource="watson.txt";
        try {
            String fileContent = this.readResource(fileResource, Charsets.UTF_8);
            Text content = new Text();
            content.set(fileContent);
            mapReduceDriver.withInput(new LongWritable(), content);
        } catch (Exception e) {
            e.printStackTrace();
        }

        mapReduceDriver.withOutput(new Text("Watson"), new IntWritable(81));  // Expect 81 counts of word 'Watson' in watson.txt resource file
        mapReduceDriver.runTest();

        /*

        example output from test run:

        12:13:07.990 [main] DEBUG o.a.h.m.mapreduce.MapReduceDriver - Reducing input (Water, (1))
        12:13:07.990 [main] DEBUG o.a.h.m.mapreduce.MapReduceDriver - Reducing input (Waterloo, (1, 1, 1, 1, 1, 1, 1, 1))
        12:13:07.992 [main] DEBUG o.a.h.m.mapreduce.MapReduceDriver - Reducing input (Watson, (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1))
        12:13:07.994 [main] DEBUG o.a.h.m.mapreduce.MapReduceDriver - Reducing input (We, (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1))
        12:13:07.994 [main] DEBUG o.a.h.m.mapreduce.MapReduceDriver - Reducing input (Web, (1, 1, 1))
        12:13:07.994 [main] DEBUG o.a.h.m.mapreduce.MapReduceDriver - Reducing input (Wedding, (1))
        12:13:07.995 [main] DEBUG o.a.h.m.mapreduce.MapReduceDriver - Reducing input (Wedlock, (1))
        ...
        12:13:13.612 [main] DEBUG o.a.h.m.mapreduce.MapReduceDriver - Reducing input (zest, (1))
        12:13:13.612 [main] DEBUG o.a.h.m.mapreduce.MapReduceDriver - Reducing input (zigzag, (1))
        12:13:13.617 [main] DEBUG org.apache.hadoop.mrunit.TestDriver - Matched expected output (Watson, 81) at positions [0]

        */
    }


}
