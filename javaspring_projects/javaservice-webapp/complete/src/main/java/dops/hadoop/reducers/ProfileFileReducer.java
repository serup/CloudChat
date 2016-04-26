package dops.hadoop.reducers;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
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
 * INPUT IS A XML structure
 *
 *  <Entity>
 *   <version>1</version>
 *   <ProfileRecord>
 *     <TransGUID>581dd417e98c4e11b3b6aa15435cd929</TransGUID> -- each record has a unique transaction id to be used with security, when detecting inconsistencies
 *     <Protocol>DED</Protocol>
 *     <ProtocolVersion>1.0.0.0</ProtocolVersion>
 *     <DataSize></DataSize>
 *     <Data>
 *          <record>
 *              <chunk_id></chunk_id>
 *              <aiid></aiid>
 *              <chunk_seq></chunk_seq>
 *              <chunk_data></chunk_data>
 *          </record>
 *     </Data> -- here is the pCompressedData from above protocol encoding, compression, encrypting algoritm -- currently DED (dataencoderdecoder)
 *     <DataMD5>EB11373F99512CD9731B74621A9F6B15<DataMD5> -- this md5 checksum is used as an extra verification -- Data must be verified with this value before being used.
 *   </ProfileRecord>
 * </Entity>
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

    public String getElementOfInterest() {
        return elementOfInterest;
    }

    public void setElementOfInterest(String elementOfInterest) {
        this.elementOfInterest = elementOfInterest;
    }

    public String elementOfInterest;

    public String getElementOfInterestValue() {
        return elementOfInterestValue;
    }

    public void setElementOfInterestValue(String elementOfInterestValue) {
        this.elementOfInterestValue = elementOfInterestValue;
    }

    public String elementOfInterestValue;


    public void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException {
        DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder dBuilder;
        Document doc = null;

        for (Text value : values) {


            try {
                dBuilder = dbFactory.newDocumentBuilder();
                doc = dBuilder.parse(new InputSource(new StringReader(value.toString())));

                doc.getDocumentElement().normalize();
                NodeList nList = doc.getElementsByTagName("entity");
                for (int temp = 0; temp < nList.getLength(); temp++) {
                    Node nNode = nList.item(temp);
                    if (nNode.getNodeType() == Node.ELEMENT_NODE) {
                        Element eElement = (Element) nNode;
                        //System.out.println(eElement.getTagName());
                        String id = eElement.getElementsByTagName("id").item(0).getTextContent();
                        String data = eElement.getElementsByTagName("data").item(0).getTextContent();
                        // reduce to find the element being searched for
                        if(id.contentEquals(elementOfInterest) && data.contentEquals(elementOfInterestValue))
                        {
                            // if found search values, thus key aka. file is of interest
                            outputKey.set(constructPropertyXml(key));
                            context.write(outputKey, new Text(""));
                        }
                    }

                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public static String constructPropertyXml(Text name) {
        StringBuilder sb = new StringBuilder();
              sb.append("<file>").append(name)
                .append("</file>");
        return sb.toString();
    }
}
