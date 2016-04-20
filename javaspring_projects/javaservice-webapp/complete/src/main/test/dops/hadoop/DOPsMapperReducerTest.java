package dops.hadoop;

import com.google.common.base.Charsets;
import com.google.common.io.Resources;
import dops.hadoop.mappers.FindDataBaseFileMapper;
import dops.hadoop.reducers.FindDataBaseFileReducer;
import hadoop.mappers.FindFileWithPatternInsideMapper;
import hadoop.reducers.FindFileWithPatternInsideReducer;
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
 * Created by serup on 22-02-16.
 */
public class DOPsMapperReducerTest {


    @Before
    public void setUp() throws Exception {
    }

    public String readResource(final String fileName, Charset charset) throws Exception {
        try {
            return Resources.toString(Resources.getResource(fileName), charset);
        } catch (IOException e) {
            throw new IllegalArgumentException(e);
        }
    }

    @Test
    public void testFindFileWithPatternMapReduce() {
        MapDriver<LongWritable, Text, Text, IntWritable> mapDriver;
        ReduceDriver<Text, IntWritable, Text, IntWritable> reduceDriver;
        MapReduceDriver<LongWritable, Text, Text, IntWritable, Text, IntWritable> mapReduceDriver;

        FindFileWithPatternInsideMapper mapper = new FindFileWithPatternInsideMapper();
        FindFileWithPatternInsideReducer reducer = new FindFileWithPatternInsideReducer();
        mapper.setSearchPattern("Watson"); // pattern searched for inside the file
        mapDriver = MapDriver.newMapDriver(mapper);
        reduceDriver = ReduceDriver.newReduceDriver(reducer);
        mapReduceDriver = MapReduceDriver.newMapReduceDriver(mapper, reducer);
        String fileResource1="fileWithPattern1.txt";
        String fileResource2="fileWithPattern2.txt"; // should contain the pattern searched for


        try {
            String fileContent = this.readResource(fileResource1, Charsets.UTF_8);
            Text content = new Text();
            content.set(fileContent);
            mapReduceDriver.withInput(new LongWritable(), content);
            String fileContent2 = this.readResource(fileResource2, Charsets.UTF_8);
            Text content2 = new Text();
            content.set(fileContent2);
            mapReduceDriver.withInput(new LongWritable(), content2);
        } catch (Exception e) {
            e.printStackTrace();
        }

        mapReduceDriver.withOutput(new Text("somefile"), new IntWritable(1));  // Expect 1 counts of pattern 'Watson' found in watson.txt resource file - however Mock's give filename 'somefile' for this resource
        mapReduceDriver.runTest();

        /*

        example output from test run:

        13:53:18.894 [main] DEBUG o.a.h.m.mapreduce.MapReduceDriver - Mapping input (0, hello Watson - this is the file with the correct pattern))
        13:53:19.311 [main] DEBUG o.a.h.m.mapreduce.MapReduceDriver - Mapping input (0, ))
        13:53:19.313 [main] DEBUG o.a.h.m.mapreduce.MapReduceDriver - Starting reduce phase with reducer: hadoop.reducers.FindFileWithPatternInsideReducer@3b2c72c2
        13:53:19.319 [main] DEBUG o.a.h.m.mapreduce.MapReduceDriver - Reducing input (somefile, (1))
        13:53:19.355 [main] DEBUG org.apache.hadoop.mrunit.TestDriver - Matched expected output (somefile, 1) at positions [0]

        */
    }

    //TODO: find DOPs database file
    @Test
    public void testFindDataBaseProfileFile() {

        // 1. Setup mapper and reducer
        FindDataBaseFileMapper mapper = new FindDataBaseFileMapper();
        FindDataBaseFileReducer reducer = new FindDataBaseFileReducer();
        MapDriver<LongWritable, Text, Text, Text> mapDriver;
        ReduceDriver<Text, Text, Text, Text> reduceDriver;
        MapReduceDriver<LongWritable, Text, Text, Text, Text, Text> mapReduceDriver;
        mapDriver = MapDriver.newMapDriver(mapper);
        reduceDriver = ReduceDriver.newReduceDriver(reducer);
        mapReduceDriver = MapReduceDriver.newMapReduceDriver(mapper, reducer);

        // 2. Set input for reducer
        String fileResource="xmlparserTestFile.xml";
        try {
            String fileContent = this.readResource(fileResource, Charsets.UTF_8);
            Text content = new Text();
            content.set(fileContent);
            mapReduceDriver.withInput(new LongWritable(), content);
        } catch (Exception e) {
            e.printStackTrace();
        }

        // 3. Set output for map/reduce job
        mapReduceDriver.withOutput(new Text("<configuration>"), new Text(""));
        mapReduceDriver.withOutput(new Text("<property><name>dfs</name><value>2</value></property>"), new Text(""));
        mapReduceDriver.withOutput(new Text("</configuration>"), new Text(""));
        mapReduceDriver.withOutput(new Text("<configuration>"), new Text(""));
        mapReduceDriver.withOutput(new Text("<property><name>dfs.replication</name><value>1</value></property>"), new Text(""));
        mapReduceDriver.withOutput(new Text("</configuration>"), new Text(""));


        // 4. run MapReduce job
        mapReduceDriver.runTest();

        // 5. verify result
    }

}
