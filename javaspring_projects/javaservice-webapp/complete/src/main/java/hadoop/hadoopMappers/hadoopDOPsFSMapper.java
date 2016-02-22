package hadoop.hadoopMappers;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/**
 * Created by serup on 22-02-16.
 */
public class hadoopDOPsFSMapper extends Mapper<LongWritable, Text, Text, IntWritable> {

    /**
     * Make a key value pair list of filenames with user and password
     */
    protected void map(LongWritable key, Text value, Context context) throws java.io.IOException, InterruptedException {

        /**
         * the value contains the username and password - first extract values and make a list of files in hadoop cluster
         * then add each file to the list - reducer will then reduce to the user in question
         */

    }
}