package dops.hadoop.mappers;

import dops.database.DataBaseControl;
import dops.protocol.ded.DEDDecoder;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;

import javax.xml.bind.DatatypeConverter;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.File;
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

    public DataBaseControl dbctrl;

    public ProfileFileMapper() {
        DataBaseControl dbctrl = new DataBaseControl();
    }

    public void map(LongWritable key, Text value, Mapper.Context context ) throws IOException, InterruptedException {

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

                // fetch the EntityFile then the associated TOAST file, since TOAST file has same xml structure then take Data area of that file as _value for the keypair
                // since TOAST file has many records, then context.write has to be called for each record
                DataBaseControl.DEDElements dedElements = dbctrl.createDEDElements(); // Placeholder for retrieved DataEncoderDecoder elements
                String uppercaseEntityName = "DataDictionary/Entities/DD_" + "PROFILE"  + ".xml";  // example: DataDictionary/DD_PROFILE.xml
                File dataDictionaryFile  = new File(this.getClass().getClassLoader().getResource(uppercaseEntityName).getFile());
                if(dataDictionaryFile.exists()) {
                    boolean bResult = dbctrl.readDDEntityRealm(dataDictionaryFile, "Profile", dedElements);
                    if (!bResult) {
                        System.out.println("[ProfileFileMapper] ERROR : DataDictionary File can not be read - please check access rights : " + uppercaseEntityName);
                        return;
                    }

                    // take the hex converted data and unhex it before DED will decode it
                    String data = _value.toString();
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
                                System.out.println("[ProfileFileMapper] WARNING : DECODING ERROR: elementID : " + e.getStrElementID());
                                break;
                            }
                        }
                    }

                    // fetch the TOAST file, then since TOAST file has same xml structure then take Data area of that file as _value for the keypair

                }
                // forward result key pair
                context.write(new Text(_key.trim()), new Text(_value.trim())); // current filename is used as key and data is value
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

}
