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

    public String getElementOfInterest() {
        return elementOfInterest;
    }

    public void setElementOfInterest(String elementOfInterest) {
        this.elementOfInterest = elementOfInterest;
    }

    public String elementOfInterest;

/* deprecated
    public void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException {
        for (Text value : values) {
            //TODO: use DED to lookup Data and check for search values
            DataBaseControl dbctrl = new DataBaseControl();
            DataBaseControl.DEDElements dedElements = dbctrl.createDEDElements(); // Placeholder for retrieved DataEncoderDecoder elements
            String uppercaseEntityName = "DataDictionary/Entities/DD_" + "PROFILE"  + ".xml";  // example: DataDictionary/DD_PROFILE.xml
            File dataDictionaryFile  = new File(this.getClass().getClassLoader().getResource(uppercaseEntityName).getFile());
            if(dataDictionaryFile.exists()) {
                boolean bResult = dbctrl.readDDEntityRealm(dataDictionaryFile, "Profile", dedElements);
                if (!bResult) {
                    System.out.println("[ProfileFileReducer] ERROR : DataDictionary File can not be read - please check access rights : " + uppercaseEntityName);
                    return;
                }

                // take the hex converted data and unhex it before DED will decode it
                String data = value.toString();
                byte[] data_in_unhexed_buf = DatatypeConverter.parseHexBinary(data);

                // fetch the data area and unpack it with DED to check it
                DEDDecoder DED = new DEDDecoder();
                DED.PUT_DATA_IN_DECODER(data_in_unhexed_buf, data_in_unhexed_buf.length); // data should be decompressed using LZSS algorithm
                if (DED.GET_STRUCT_START("record") == 1) {
                    int n = 0;
                    for (DataBaseControl.Elements e : dedElements) {
                        String tmp = ""; // clear
                        if ((tmp = DED.GET_STDSTRING(e.getStrElementID())) != "") {
                            // put attribute values from entity file into DEDElements
                            DataBaseControl.Elements elm = dedElements.get(n);
                            elm.setElementData(tmp.getBytes());
                            dedElements.set(n, elm);
                            n++;
                        } else {
                            System.out.println("WARNING : DECODING ERROR: elementID : " + e.getStrElementID());
                            break;
                        }
                    }
                }
                // if found search values, thus key aka. file is of interest
                outputKey.set(constructPropertyXml(key));
                context.write(outputKey, new Text(""));
            }
        }
    }
*/

/*    public void reduce(Text key, Iterable<DataBaseControl.Elements> values, Context context) throws IOException, InterruptedException {
        for (DataBaseControl.Elements value : values) {
            DataBaseControl dbctrl = new DataBaseControl();

            // reduce to find the element being searched for
            if(value.getStrElementID() == elementOfInterest)
            {
                // if found search values, thus key aka. file is of interest
                outputKey.set(constructPropertyXml(key));
                context.write(outputKey, new Text(""));
            }
        }
    }
*/
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
                        System.out.println(eElement.getTagName());
                        String id = eElement.getElementsByTagName("id").item(0).getTextContent();
                        String data = eElement.getElementsByTagName("data").item(0).getTextContent();
                        // reduce to find the element being searched for
                        if(id.contentEquals(elementOfInterest))
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
