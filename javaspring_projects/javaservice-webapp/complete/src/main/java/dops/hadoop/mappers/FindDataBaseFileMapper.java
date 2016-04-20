package dops.hadoop.mappers;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamConstants;
import javax.xml.stream.XMLStreamReader;
import java.io.ByteArrayInputStream;
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
public class FindDataBaseFileMapper extends Mapper<LongWritable, Text, Text, Text>{

    private final static IntWritable one = new IntWritable( 1 );
    private Text word = new Text();

    public void map( LongWritable key, Text value, Mapper.Context context ) throws IOException, InterruptedException {

        String document = value.toString();
        System.out.println("‘" + document + "‘");
        try {
            XMLStreamReader reader = XMLInputFactory.newInstance().createXMLStreamReader(new ByteArrayInputStream(document.getBytes()));
            String propertyName = "";
            String propertyValue = "";
            String currentElement = "";
            while (reader.hasNext()) {
                int code = reader.next();
                switch (code) {
                    case XMLStreamConstants.START_ELEMENT: //START_ELEMENT:
                        currentElement = reader.getLocalName();
                        break;
                    case XMLStreamConstants.CHARACTERS:  //CHARACTERS:
                        if (currentElement.equalsIgnoreCase("name")) {
                            propertyName += reader.getText();
                            //System.out.println(propertyName);
                        } else if (currentElement.equalsIgnoreCase("value")) {
                            propertyValue += reader.getText();
                            //System.out.println(propertyValue);
                        }
                        break;
                }
            }
            reader.close();
            context.write(new Text(propertyName.trim()), new Text(propertyValue.trim()));

        }
        catch(Exception e){
            throw new IOException(e);

        }
    }

}
