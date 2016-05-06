package dops.hadoop;

import com.google.common.base.Charsets;
import com.google.common.io.Resources;
import dops.hadoop.mappers.ProfileFileMapper;
import dops.hadoop.mappers.XMLFileMapper;
import dops.hadoop.reducers.ProfileFileReducer;
import dops.hadoop.reducers.XMLFileReducer;
import hadoop.mappers.FindFileWithPatternInsideMapper;
import hadoop.reducers.FindFileWithPatternInsideReducer;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;
import org.junit.Before;
import org.junit.Test;

import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;

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
        MapReduceDriver<LongWritable, Text, Text, IntWritable, Text, IntWritable> mapReduceDriver;
        FindFileWithPatternInsideMapper mapper = new FindFileWithPatternInsideMapper();
        FindFileWithPatternInsideReducer reducer = new FindFileWithPatternInsideReducer();
        mapper.setSearchPattern("Watson"); // pattern searched for inside the file
        mapReduceDriver = MapReduceDriver.newMapReduceDriver(mapper, reducer);
        String fileResource1= "dummyFiles/fileWithPattern1.txt";
        String fileResource2= "dummyFiles/fileWithPattern2.txt"; // should contain the pattern searched for


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


    @Test
    public void testMapReduceXMLFile() {

        // 1. Setup mapper and reducer
        XMLFileMapper mapper = new XMLFileMapper();
        XMLFileReducer reducer = new XMLFileReducer();
        MapReduceDriver<LongWritable, Text, Text, Text, Text, Text> mapReduceDriver;
        mapReduceDriver = MapReduceDriver.newMapReduceDriver(mapper, reducer);

        // 2. Set input for reducer
        String fileResource= "dummyFiles/xmlparserTestFile.xml";
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

    @Test
    public void testFindProfileFile() {

        // 1. Setup mapper and reducer
        ProfileFileMapper mapper = new ProfileFileMapper();
        ProfileFileReducer reducer = new ProfileFileReducer();
        MapReduceDriver<LongWritable, Text, Text, Text, Text, Text> mapReduceDriver;

        // 2. Setup search information - username - for profile to be found
        mapper.dbctrl.setRelativeENTITIES_DATABASE_PLACE("/tmp/");  // reset default value to work with test
        mapper.dbctrl.setRelativeTOASTS_DATABASE_PLACE("/tmp/"); // reset default value to work with test
        reducer.setElementOfInterest("username");
        reducer.setElementOfInterestValue("johnnytest@email.com");
        mapReduceDriver = MapReduceDriver.newMapReduceDriver(mapper, reducer);

        // 3. Set input for reducer
        String fileResource = "DataDictionary/Database/ENTITIEs/355760fb6afaf9c41d17ac5b9397fd45.xml"; // This is a profile database file
        try {
            String fileContent = this.readResource(fileResource, Charsets.UTF_8);
            Text content = new Text();
            content.set(fileContent);
            mapReduceDriver.withInput(new LongWritable(), content);
        } catch (Exception e) {
            e.printStackTrace();
        }

        // 4. Extract toast file to tmp area - mapper will use input and parse for toast file name, then try to read it and parse it, hence the need for extract of test toast file
        File resourceFileToast       = new File(this.getClass().getClassLoader().getResource("DataDictionary/Database/TOASTs/355760fb6afaf9c41d17ac5b9397fd45_toast.xml").getFile());
        File destinationFile         = new File("/tmp/355760fb6afaf9c41d17ac5b9397fd45_toast.xml");
        try {
            Files.deleteIfExists(destinationFile.toPath());
            Files.copy(resourceFileToast.toPath(), destinationFile.toPath());
        } catch (IOException e) {
            e.printStackTrace();
        }


        // 5. Set output for map/reduce job
        mapReduceDriver.withOutput(new Text("<result>"), new Text(""));
        mapReduceDriver.withOutput(new Text("<file>somefile</file>"), new Text(""));  // resource file yields 'somefile' when used with Mock mapper framework
        mapReduceDriver.withOutput(new Text("</result>"), new Text(""));

        // 6. run MapReduce job
        mapReduceDriver.runTest();

    }

}
