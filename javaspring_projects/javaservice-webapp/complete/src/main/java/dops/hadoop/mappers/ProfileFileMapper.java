package dops.hadoop.mappers;

import dops.database.DataBaseControl;
import dops.protocol.ded.DEDDecoder;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
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
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.util.List;
import java.util.stream.Collectors;

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

    public DataBaseControl dbctrl  = new DataBaseControl();

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
                    _key = filename;
                    _value = getElementValue(eElement);
                    String md5 = dbctrl.CalculateMD5CheckSum(_value.getBytes());
                    if (!md5.equalsIgnoreCase(eElement.getElementsByTagName("DataMD5").item(0).getTextContent()))
                    {
                        System.out.println("ERROR: data area in file have changed without having the MD5 checksum changed -- Warning data could be compromised ; ");
                        return;  // data area in file have changed without having the MD5 checksum changed -- Warning data could be compromised
                    }
                }

                // fetch the EntityFile then the associated TOAST file, since TOAST file has same xml structure then take Data area of that file as _value for the keypair
                // since TOAST file has many records, then context.write has to be called for each record
                DataBaseControl.DEDElements dedElements = dbctrl.createDEDElements(); // Placeholder for retrieved DataEncoderDecoder elements
                String uppercaseEntityName = "DataDictionary/Entities/DD_" + "PROFILE"  + ".xml";  // example: DataDictionary/DD_PROFILE.xml
                File dataDictionaryFile  = new File(this.getClass().getClassLoader().getResource(uppercaseEntityName).getFile());
                String EntityName = "Profile";
                if(dataDictionaryFile.exists()) {
                    boolean bResult = dbctrl.readDDEntityRealm(dataDictionaryFile, EntityName, dedElements);
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
                        if(n != 0) {
                            /// fetch the TOAST file, then since TOAST file has same xml structure then take Data area of that file as _value for the keypair
                            /// now fetch ALL elements with their attribute values  -- all incl. TOAST attributes
                            /// this means that now we should fetch all attributes from the TOAST entity file
                            String idOfTOASTfile = findToastFileName(dedElements, EntityName);
                            if(!idOfTOASTfile.isEmpty()) {
                                Document toastdoc = readToastFileIntoDocument(idOfTOASTfile,EntityName, context);
                                if(toastdoc != null) {
                                    assembleToastChunksAndSetupKeyPair(toastdoc, context, _key, EntityName);
                                    // now all elements from Entity should have been read, including the ones in TOAST
                                    // (they have also been merged, so no chunks exists)
                                } else {
                                    // warning there could not be found a TOAST file, this is not necessary an error, however strange
                                    System.out.println("[FetchTOASTEntities] WARNING: there could not be found a TOAST file, this is not necessary an error, however strange : " + idOfTOASTfile);
                                }
                            }
                        }
                        else
                            System.out.println("[ProfileFileMapper] ERROR : DECODING ERROR: no record found ");
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private Document readToastFileIntoDocument(String idOfTOASTfile, String entityName, Mapper.Context context) throws Exception
    {
        Document toastdoc=null;
        String TOASTFilePath;
        DocumentBuilderFactory dbFactory2 = DocumentBuilderFactory.newInstance();
        DocumentBuilder dBuilder2;
        if(context.getConfiguration().get("dops.toast.database.dir") == null) {
            TOASTFilePath = dbctrl.getRelativeENTITIES_DATABASE_TOAST_PLACE() + idOfTOASTfile + ".xml";
            // now open its toast file and put all attributes and values on record_value
            File ToastFile = new File(TOASTFilePath);
            if (ToastFile.exists()) {
                File fXmlFile = ToastFile;
                try {
                    dBuilder2 = dbFactory2.newDocumentBuilder();
                    toastdoc = dBuilder2.parse(fXmlFile);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
        else {
            // handle file from hdfs
            TOASTFilePath = context.getConfiguration().get("dops.toast.database.dir") + idOfTOASTfile + ".xml";
            Configuration configuration = new Configuration();
            FileSystem fs = FileSystem.get(new URI(context.getConfiguration().get("fs.defaultFS")),configuration);
            Path filePath = new Path(TOASTFilePath);
            FSDataInputStream fsDataInputStream = fs.open(filePath);
            try {
                dBuilder2 = dbFactory2.newDocumentBuilder();
                toastdoc = dBuilder2.parse(fsDataInputStream);
                System.out.println("hdfs file parsed : TransGUID: " + toastdoc.getElementsByTagName("TransGUID").item(0).getTextContent());
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return toastdoc;
    }

    /**
     * It will find a specific TOAST element in the dedElements list
     * and this elements value will be the name of the TOAST file
     *
     * @param dedElements -- list of elements stored as data encoder decoder objects
     * @param EntityName -- fx. Profile
     * @return -- value of the found Toast element
     */
    private String findToastFileName(DataBaseControl.DEDElements dedElements, String EntityName)
    {
        // each element has pattern <name>_chunk_id ; fx profile_chunk_id
        String chunkID = (EntityName + "_chunk_id").toLowerCase();
        String chunkIdValue = "";
        // finding the element
        List<DataBaseControl.Elements> resultElement = dedElements.parallelStream()
                .filter((p)-> p.getStrElementID().equals((chunkID)))
                .collect(Collectors.toList());
        if(resultElement.size()>0) {
            byte[] bytes = resultElement.get(0).getElementData();
            try {
                chunkIdValue = new String(bytes, "UTF-8"); // for UTF-8 encoding
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }
        return chunkIdValue;
    }

    private void assembleToastChunksAndSetupKeyPair(Document toastdoc,Mapper.Context context, String _key, String expectedChildEntity) throws Exception {
        String PrevChunkId = "nothing";
        boolean isPushed=true;
        String ChildRecord = expectedChildEntity + "Record";
        assert toastdoc != null;
        toastdoc.getDocumentElement().normalize();
        String nodeName = toastdoc.getDocumentElement().getNodeName();
        if (!expectedChildEntity.contentEquals(nodeName))
            return;  // Error This is NOT the correct file


        DataBaseControl.Elements Element = dbctrl.createElements();
        NodeList listOfElementsInToastDoc = toastdoc.getElementsByTagName(ChildRecord);
        for (int index = 0; index < listOfElementsInToastDoc.getLength(); index++) {
            Node nodeelement = listOfElementsInToastDoc.item(index);
            if (nodeelement.getNodeType() == Node.ELEMENT_NODE) {
                Element eElement = (Element) nodeelement;
                DataBaseControl.DatabaseEntityRecordEntry recordEntry = createRecordEntry(eElement);
                String md5 = dbctrl.CalculateMD5CheckSum(recordEntry.getData().getBytes());
                if (!md5.equalsIgnoreCase(recordEntry.getDataMD5()))
                {
                    System.out.println("ERROR: data area in file have changed without having the MD5 checksum changed -- Warning data could be compromised ; " + expectedChildEntity );
                    return;  // data area in file have changed without having the MD5 checksum changed -- Warning data could be compromised
                }

                // According to what the protocol prescribes for DED entries in TOAST, then a loop of decode, according to specs, of entries is needed
                // fetch the data area and unpack it with DED to check it

                DataBaseControl.EntityChunkDataInfo chunk = getToastRecord(recordEntry, expectedChildEntity);

                if(PrevChunkId.contentEquals("nothing") || !PrevChunkId.contentEquals(chunk.entity_chunk_id))
                {
                    if(!PrevChunkId.contentEquals("nothing") && !PrevChunkId.contentEquals(chunk.entity_chunk_id))
                    {
                        // SETUP KEY/PAIR
                        String tmp = new String(Element.ElementData, "UTF-8");
                        if(tmp.contentEquals("<empty>"))
                            tmp="null";
                        String resultValue = constructEntityXml(new Text(Element.getStrElementID()), new Text(tmp));
                        // FORWARD RESULT KEY/PAIR
                        context.write(new Text(_key), new Text(resultValue));
                        Element = dbctrl.createElements();
                    }

                    // new Element
                    Element.strElementID = chunk.entity_chunk_id;
                    Element.ElementData = new byte[0];
                    PrevChunkId = chunk.entity_chunk_id;
                    isPushed=false;
                }
                // this will, chunk by chunk, assemble the element data - fx. a foto element consists of many chunks in the file
                Element.setElementData(dbctrl.back_inserter(chunk.entity_chunk_data, Element.ElementData));
            }
        }

        // should only add last element if chunks have been assembled
        if(!isPushed){
            // SETUP KEY/PAIR
            String tmp = new String(Element.ElementData, "UTF-8");
            if(tmp.contentEquals("<empty>"))
                tmp="null";
            String resultValue = constructEntityXml(new Text(Element.getStrElementID()), new Text(tmp));
            // FORWARD RESULT KEY/PAIR
            context.write(new Text(_key), new Text(resultValue));
        }
    }

    private DataBaseControl.EntityChunkDataInfo getToastRecord(DataBaseControl.DatabaseEntityRecordEntry record, String child)
    {
        byte[] DataInUnHexedBuffer = DatatypeConverter.parseHexBinary(record.getData());
        DataBaseControl.EntityChunkDataInfo chunk = dbctrl.createEntityChunkDataInfo();
        DEDDecoder DED2 = new DEDDecoder();
        DED2.PUT_DATA_IN_DECODER(DataInUnHexedBuffer,DataInUnHexedBuffer.length);
        String EntityChunkId    = (child + "_chunk_id").toLowerCase(); // eg. profile_chunk_id
        String EntityChunkSeq   = (child + "_chunk_seq").toLowerCase(); // eg. profile_chunk_seq
        String EntityChunkData  = (child + "_chunk_data").toLowerCase(); // eg. profile_chunk_data
        // decode data ...
        DED2.GET_STRUCT_START( "record" );
        chunk.entity_chunk_id   = DED2.GET_STDSTRING	( EntityChunkId ); // key of particular item
        chunk.aiid              = DED2.GET_ULONG   	( "aiid" ); // this number is continuously increasing all throughout the entries in this table
        chunk.entity_chunk_seq  = DED2.GET_ULONG   	( EntityChunkSeq ); // sequence number of particular item
        chunk.entity_chunk_data = DED2.GET_STDVECTOR	( EntityChunkData ); //
        DED2.GET_STRUCT_END( "record" );
        return chunk;
    }

    private String getElementValue(Element eElement)
    {
       /* System.out.println(eElement.getTagName());
        System.out.println(eElement.getElementsByTagName("TransGUID").item(0).getTextContent());
        System.out.println(eElement.getElementsByTagName("Protocol").item(0).getTextContent());
        System.out.println(eElement.getElementsByTagName("ProtocolVersion").item(0).getTextContent());
        System.out.println(eElement.getElementsByTagName("DataSize").item(0).getTextContent());
        System.out.println(eElement.getElementsByTagName("Data").item(0).getTextContent());
        System.out.println(eElement.getElementsByTagName("DataMD5").item(0).getTextContent()); */
        return eElement.getElementsByTagName("Data").item(0).getTextContent();
    }

    private DataBaseControl.DatabaseEntityRecordEntry createRecordEntry(Element eElement)
    {
        DataBaseControl.DatabaseEntityRecordEntry record = dbctrl.createEntityRecordEntry();
        record.setTransGUID(eElement.getElementsByTagName("TransGUID").item(0).getTextContent());
        record.setProtocol(eElement.getElementsByTagName("Protocol").item(0).getTextContent());
        record.setProtocolVersion(eElement.getElementsByTagName("ProtocolVersion").item(0).getTextContent());
        record.setDataSize(Integer.parseInt(eElement.getElementsByTagName("DataSize").item(0).getTextContent()));
        record.setData(eElement.getElementsByTagName("Data").item(0).getTextContent());
        record.setDataMD5(eElement.getElementsByTagName("DataMD5").item(0).getTextContent());
        return record;
    }

    public static String constructEntityXml(Text id, Text data) {
        StringBuilder sb = new StringBuilder();
        sb.append("<entity>").append("<id>")
                .append(id)
                .append("</id><data>")
                .append(data)
                .append("</data>")
                .append("</entity>");
        return sb.toString();
    }
}
