package hadoop.hadoopMappers;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;

import java.io.IOException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by serup on 12-04-16.
 */
public class FindFileWithPatternInsideMapper extends Mapper<LongWritable, Text, Text, IntWritable>  {

    private Text foundFile = new Text();

    public void setSearchPattern(String searchPattern) {
        SearchPattern = searchPattern;
    }

    private String SearchPattern = new String("abc123");

    @Override
    protected void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
        String lineFromFile = value.toString();
        Pattern p = Pattern.compile(SearchPattern); // pattern which should be found in 'foundFile'
        Matcher matcher = p.matcher(lineFromFile);
        if(matcher.find()) {
            // foundFile has the pattern, now find its filename
            String filename =((FileSplit) context.getInputSplit()).getPath().getName() ; // this will yield 'somefile' when used with Mock mapper framework
            foundFile.set(filename);
            context.write(foundFile, new IntWritable(1)); // add 1 count to key ( foundFile )
        }
    }

}
