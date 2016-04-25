package dops.hadoop.mappers;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.IOException;
import java.io.StringReader;


/**
 * Created by serup on 21-04-16.
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
public class XMLFileMapper extends Mapper<LongWritable, Text, Text, Text> {

    private final static IntWritable one = new IntWritable( 1 );
    private Text word = new Text();

    public void map( LongWritable key, Text value, Mapper.Context context ) throws IOException, InterruptedException {

        DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder dBuilder;
        Document doc = null;
        try {
            dBuilder = dbFactory.newDocumentBuilder();
            doc = dBuilder.parse(new InputSource(new StringReader(value.toString())));

            String _name = "";
            String _value = "";
            doc.getDocumentElement().normalize();
            NodeList nList = doc.getElementsByTagName("property");
            for (int temp = 0; temp < nList.getLength(); temp++) {
                Node nNode = nList.item(temp);
                if (nNode.getNodeType() == Node.ELEMENT_NODE) {
                    Element eElement = (Element) nNode;
                    System.out.println(eElement.getTagName());
                    System.out.println(eElement.getElementsByTagName("name").item(0).getTextContent());
                    System.out.println(eElement.getElementsByTagName("value").item(0).getTextContent());
                    _name = eElement.getElementsByTagName("name").item(0).getTextContent();
                    _value = eElement.getElementsByTagName("value").item(0).getTextContent();
                }
                context.write(new Text(_name.trim()), new Text(_value.trim()));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}
