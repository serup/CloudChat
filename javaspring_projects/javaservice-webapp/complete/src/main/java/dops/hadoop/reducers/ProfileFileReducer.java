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
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.IntStream;
import java.util.stream.Stream;

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

    private final Text outputKey = new Text();

    public void setElementOfInterest(String elementOfInterest) {
        this.elementOfInterest = elementOfInterest;
    }

    private String elementOfInterest;

    public void setElementOfInterestValue(String elementOfInterestValue) {
        this.elementOfInterestValue = elementOfInterestValue;
    }

    private String elementOfInterestValue;

    private void setupContextConfigurationElements(Context context) {
        if(context.getConfiguration().get("dops.entities.database.dir") != null) {
            elementOfInterest = context.getConfiguration().get("dops.elementofinterest");
            elementOfInterestValue = context.getConfiguration().get("dops.elementofinterest.value");
        }
    }

    private boolean isElementOfInterest(Node nNode) {
        boolean bFound=false;
        assert(nNode != null);
        if (nNode.getNodeType() == Node.ELEMENT_NODE) {
            Element eElement = (Element) nNode;
            String id = eElement.getElementsByTagName("id").item(0).getTextContent();
            String data = eElement.getElementsByTagName("data").item(0).getTextContent();
            if (id.contentEquals(elementOfInterest) && data.contentEquals(elementOfInterestValue))
                bFound = true;
        }
        return bFound;
    }

    private Document setupDocument(Text xmlElement) {
        DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder dBuilder;
        Document doc = null;
        try {
            dBuilder = dbFactory.newDocumentBuilder();
            doc = dBuilder.parse(new InputSource(new StringReader(xmlElement.toString())));
            doc.getDocumentElement().normalize();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return doc;
    }

    private boolean searchForElementOfInterest(Document doc)
    {
        boolean bFound=false;
        NodeList nList = doc.getElementsByTagName("entity");

        // since NodeList does not have stream implemented, then use this hack
        Stream<Node> nodeStream = IntStream.range(0, nList.getLength()).mapToObj(nList::item);
        // search for element of interest in the NodeList
        if(nodeStream.parallel().filter(this::isElementOfInterest).collect(Collectors.toList()).size() > 0)
            bFound=true;

       return bFound;
    }

    private boolean parseXMLToFindElementOfInterest(Text xmlElement) {
        boolean bFound=false;
        try {
            Document doc = setupDocument(xmlElement);
            if(doc!=null)
                bFound = searchForElementOfInterest(doc);
       } catch (Exception e) {
            e.printStackTrace();
            bFound=false;
        }
        return bFound;
    }

    private static String constructResultXml(Text name) {
        return "<file>" + name + "</file>";
    }

    private void writeReducerContext(Text fileNameOfFileBeingProcessed, Context context) {
        try {
            outputKey.set(constructResultXml(fileNameOfFileBeingProcessed));
            context.write(outputKey, new Text(""));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private boolean parseLineFromFileToFindElementOfInterest(Iterable<Text> lineFromFile)
    {
        boolean bFound=false;
        for (Text xmlElement : lineFromFile) {
            bFound = parseXMLToFindElementOfInterest(xmlElement);
           if (bFound)
                break;
        }
        return bFound;
    }

    public void reduce(Text fileNameOfFileBeingProcessed, Iterable<Text> lineFromFile, Context context) throws IOException, InterruptedException {

        setupContextConfigurationElements(context);
        if(parseLineFromFileToFindElementOfInterest(lineFromFile))
             writeReducerContext(fileNameOfFileBeingProcessed, context);
    }


}
