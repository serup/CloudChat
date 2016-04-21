package dops.hadoop.mappers;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;
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
 * //INPUT IS AN XML structure
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
 */
public class ProfileFileMapper extends Mapper<LongWritable, Text, Text, Text>{

    public void map( LongWritable key, Text value, Mapper.Context context ) throws IOException, InterruptedException {

        String filename =((FileSplit) context.getInputSplit()).getPath().getName() ; // this will yield 'somefile' when used with Mock mapper framework
        DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder dBuilder;
        Document doc = null;
        try {
            dBuilder = dbFactory.newDocumentBuilder();
            doc = dBuilder.parse(new InputSource(new StringReader(value.toString())));

            String _key = "";
            String _value = "";
            doc.getDocumentElement().normalize();
            NodeList nList = doc.getElementsByTagName("ProfileRecord");
            for (int temp = 0; temp < nList.getLength(); temp++) {
                Node nNode = nList.item(temp);
                if (nNode.getNodeType() == Node.ELEMENT_NODE) {
                    Element eElement = (Element) nNode;
                    System.out.println(eElement.getTagName());
                    System.out.println(eElement.getElementsByTagName("TransGUID").item(0).getTextContent());
                    System.out.println(eElement.getElementsByTagName("Protocol").item(0).getTextContent());
                    System.out.println(eElement.getElementsByTagName("ProtocolVersion").item(0).getTextContent());
                    System.out.println(eElement.getElementsByTagName("DataSize").item(0).getTextContent());
                    System.out.println(eElement.getElementsByTagName("Data").item(0).getTextContent());
                    System.out.println(eElement.getElementsByTagName("DataMD5").item(0).getTextContent());

                    _key = filename;
                    _value = eElement.getElementsByTagName("Data").item(0).getTextContent();
                }
                //TODO: check if MD5 is correct, and only if it is correct, then use ProfileRecord data as result

                //TODO: fetch the TOAST file, then since TOAST file has same xml structure then take Data area of that file as _value for the keypair
                // since TOAST file has many records, then context.write has to be called for each record

                // forward result key pair
                context.write(new Text(_key.trim()), new Text(_value.trim())); // current filename is used as key and data is value
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

}
