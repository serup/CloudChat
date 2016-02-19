package hadoop.hadoopMappers;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/**
 * Created by serup on 19-02-16.
 */
public class SMSCDRMapper extends Mapper<LongWritable, Text, Text, IntWritable> {

    private Text status = new Text();
    private final static IntWritable addOne = new IntWritable(1);

    public enum CDRCounter {
        NonSMSCDR;
    }

    /**
     * Returns the SMS status code and its count
     */
    protected void map(LongWritable key, Text value, Context context) throws java.io.IOException, InterruptedException {

        String[] line = value.toString().split(";");
        // If record is of SMS CDR
        if (Integer.parseInt(line[1]) == 1) {
            status.set(line[4]);
            context.write(status, addOne);
        } else {// CDR record is not of type SMS so increment the counter
            context.getCounter(CDRCounter.NonSMSCDR).increment(1);
        }
    }
}
