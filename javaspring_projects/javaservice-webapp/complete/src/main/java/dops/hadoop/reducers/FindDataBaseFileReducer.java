package dops.hadoop.reducers;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import java.io.IOException;

/**
 * Created by serup on 20-04-16.
 *
 * //INPUT IS AN XML FILE
 * <configuration>
 *  <property>
 *    <name>dfs.replication</name>
 *    <value>1</value>
 *  </property>
 *  <property>
 *    <name>dfs</name>
 *    <value>2</value>
 *  </property>
 * </configuration>
 *
 */
public class FindDataBaseFileReducer extends Reducer<Text, Text, Text, Text> {

    @Override
    protected void setup(Context context) throws IOException, InterruptedException {
        context.write(new Text("<configuration>"), null);
    }

    @Override
    protected void cleanup(Context context) throws IOException, InterruptedException {
        context.write(new Text("</configuration>"), null);
    }

    private Text outputKey = new Text();

    public void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException {
        for (Text value : values) {
            outputKey.set(constructPropertyXml(key, value));
            context.write(outputKey, null);
        }
    }

    public static String constructPropertyXml(Text name, Text value) {
        StringBuilder sb = new StringBuilder();
        sb.append("<property><name>").append(name)
                .append("</name><value>").append(value)
                .append("</value></property>");
        return sb.toString();
    }
}
