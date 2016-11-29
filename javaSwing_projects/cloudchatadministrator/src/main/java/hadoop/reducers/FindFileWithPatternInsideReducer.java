package hadoop.reducers;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import java.io.IOException;

/**
 * Created by serup on 12-04-16.
 */
public class FindFileWithPatternInsideReducer  extends Reducer<Text, IntWritable, Text, IntWritable> {

    @Override
    protected void reduce(Text filename, Iterable<IntWritable> patternFoundCount, Context context) throws IOException, InterruptedException {
        int patternCount = 0;
        for (IntWritable amount: patternFoundCount) {
            patternCount += amount.get();
        }
        context.write(filename, new IntWritable(patternCount));  // patternCount signifies how many times pattern was found in file
    }
}
