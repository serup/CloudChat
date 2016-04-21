package dops.hadoop.reducers;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import java.io.IOException;

/**
 * Created by serup on 21-04-16.
 *
 * INPUT IS A XML structure
 *
 * <ProfileRecord>
 *     <TransGUID>581dd417e98c4e11b3b6aa15435cd929</TransGUID>
 *     <Protocol>DED</Protocol>
 *     <ProtocolVersion>1.0.0.0</ProtocolVersion>
 *     <DataSize>492</DataSize>
 *     <Data>...</Data>
 *     <DataMD5>EB11373F99512CD9731B74621A9F6B15</DataMD5>
 * </ProfileRecord>
 *
 *
 * output result:
 *
 * <result>
 *     <file></file>
 * </result>
 */
public class ProfileFileReducer extends Reducer<Text, Text, Text, Text> {

    @Override
    protected void setup(Context context) throws IOException, InterruptedException {
        context.write(new Text("<result>"), new Text(""));
    }

    @Override
    protected void cleanup(Context context) throws IOException, InterruptedException {
        context.write(new Text("</result>"), new Text(""));
    }

    private Text outputKey = new Text();

    public void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException {
        for (Text value : values) {
            //TODO: use DED to lookup Data and check for search values

            // if found search values, thus key aka. file is of interest
            outputKey.set(constructPropertyXml(key));
            context.write(outputKey, new Text(""));
        }
    }

    public static String constructPropertyXml(Text name) {
        StringBuilder sb = new StringBuilder();
              sb.append("<file>").append(name)
                .append("</file>");
        return sb.toString();
    }
}
