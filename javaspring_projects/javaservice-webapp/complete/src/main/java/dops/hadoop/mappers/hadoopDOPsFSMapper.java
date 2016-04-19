package dops.hadoop.mappers;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/**
 * Created by serup on 22-02-16.
 */
public class hadoopDOPsFSMapper extends Mapper<LongWritable, Text, Text, IntWritable> {


    protected void map(LongWritable key, Text value, Context context) throws java.io.IOException, InterruptedException {

       /* //Extract the namespace declarations in the Csharp files
        Regex reg = new Regex(@"(using)\s[A-za-z0-9_\.]*\;");
        var matches = reg.matcher(inputLine);
        foreach (Match match in matches)
        {
            //Just emit the namespaces.
            context.EmitKeyValue(match.Value,"1");
        }
*/
    }
}