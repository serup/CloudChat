package hadoop.hadoopReducers;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

/**
 * Created by serup on 22-02-16.
 */
public class hadoopDOPsFSReducer extends Reducer<Text, IntWritable, Text, IntWritable> {

    /**
     * Find only files that has map key value pairs
     * @param key
     * @param values
     * @param context
     * @throws java.io.IOException
     * @throws InterruptedException
     */
    protected void reduce(Text key, Iterable<IntWritable> values, Context context) throws java.io.IOException, InterruptedException {
        //TODO: recuce amount of files in list
        int sum = 0;
        for (IntWritable value : values) {
            sum += value.get();
        }
        context.write(key, new IntWritable(sum));
    }
}