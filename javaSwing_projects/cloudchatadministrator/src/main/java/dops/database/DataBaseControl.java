package dops.database;

import dops.protocol.ded.DEDDecoder;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import javax.xml.bind.DatatypeConverter;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.File;
import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/**
 * Created by serup on 13-04-16.
 */
public class DataBaseControl {

    String relativeENTITIES_DATABASE_PLACE;
    String relativeENTITIES_DATABASE_TOAST_PLACE;

    public String getRelativeENTITIES_DATABASE_PLACE() {
        return relativeENTITIES_DATABASE_PLACE;
    }

    public void setRelativeENTITIES_DATABASE_PLACE(String relativeENTITIES_DATABASE_PLACE) {
        this.relativeENTITIES_DATABASE_PLACE = relativeENTITIES_DATABASE_PLACE;
    }


    public String getRelativeENTITIES_DATABASE_TOAST_PLACE() {
        return relativeENTITIES_DATABASE_TOAST_PLACE;
    }

    public void setRelativeTOASTS_DATABASE_PLACE(String relativeENTITIES_DATABASE_TOAST_PLACE) {
        this.relativeENTITIES_DATABASE_TOAST_PLACE = relativeENTITIES_DATABASE_TOAST_PLACE;
    }


    public class DDEntityEntry
    {
        public String getDDGuid() {
            return DDGuid;
        }

        public void setDDGuid(String DDGuid) {
            this.DDGuid = DDGuid;
        }

        public String getPhysicalDataElementName() {
            return PhysicalDataElementName;
        }

        public void setPhysicalDataElementName(String physicalDataElementName) {
            PhysicalDataElementName = physicalDataElementName;
        }

        public String getLocation() {
            return Location;
        }

        public void setLocation(String location) {
            Location = location;
        }

        public String getCategory() {
            return Category;
        }

        public void setCategory(String category) {
            Category = category;
        }

        public String getDataType() {
            return DataType;
        }

        public void setDataType(String dataType) {
            DataType = dataType;
        }

        public String getMaxLength() {
            return MaxLength;
        }

        public void setMaxLength(String maxLength) {
            MaxLength = maxLength;
        }

        public String getDescription() {
            return Description;
        }

        public void setDescription(String description) {
            Description = description;
        }

        public String getCharacteristics() {
            return characteristics;
        }

        public void setCharacteristics(String characteristics) {
            this.characteristics = characteristics;
        }

        public String getRelationship() {
            return relationship;
        }

        public void setRelationship(String relationship) {
            this.relationship = relationship;
        }

        public String getMandatory() {
            return Mandatory;
        }

        public void setMandatory(String mandatory) {
            Mandatory = mandatory;
        }

        public String getAccessrights() {
            return accessrights;
        }

        public void setAccessrights(String accessrights) {
            this.accessrights = accessrights;
        }

        String  DDGuid;
        String  PhysicalDataElementName;
        String  Location;
        String  Category;
        String  DataType;
        String  MaxLength;
        String  Description;
        String  characteristics;
        String  relationship;
        String  Mandatory;
        String  accessrights;
    };


    public class Elements {
        public Elements() {
        }

        public String getStrElementID() {
            return strElementID;
        }

        public void setStrElementID(String strElementID) {
            this.strElementID = strElementID;
        }

        public byte[] getElementData() {
            return ElementData;
        }

        public void setElementData(byte[] elementData) {
            ElementData = elementData;
        }

        public String strElementID;
        public byte[] ElementData;
    }

    public class DatabaseEntityRecordEntry
    {

        public String getTransGUID() {
            return TransGUID;
        }

        public void setTransGUID(String transGUID) {
            TransGUID = transGUID;
        }

        public String getProtocol() {
            return Protocol;
        }

        public void setProtocol(String protocol) {
            Protocol = protocol;
        }

        public String getProtocolVersion() {
            return ProtocolVersion;
        }

        public void setProtocolVersion(String protocolVersion) {
            ProtocolVersion = protocolVersion;
        }

        public int getDataSize() {
            return DataSize;
        }

        public void setDataSize(int dataSize) {
            DataSize = dataSize;
        }

        public String getData() {
            return Data;
        }

        public void setData(String data) {
            Data = data;
        }

        public String getDataMD5() {
            return DataMD5;
        }

        public void setDataMD5(String dataMD5) {
            DataMD5 = dataMD5;
        }

        String TransGUID;
        String Protocol;
        String ProtocolVersion;
        int    DataSize;
        String Data;
        String DataMD5;
    }

    public class EntityChunkDataInfo
    {
        public String entity_chunk_id;
        public long aiid;
        public long entity_chunk_seq;
        public byte[] entity_chunk_data;

        public EntityChunkDataInfo() {
            aiid = -1;
            entity_chunk_id = "";
            entity_chunk_seq = -1;
            entity_chunk_data = new byte[0];
        }
    }

    public class EntityTOASTDEDRecord extends ArrayList<EntityChunkDataInfo> { };
    public class EntityRealm extends ArrayList<DDEntityEntry> { }
    public class DEDElements extends ArrayList<Elements> { }
    public class DatabaseEntityRecord extends ArrayList<DatabaseEntityRecordEntry> { }

    public EntityChunkDataInfo createEntityChunkDataInfo() { return new EntityChunkDataInfo(); }
    public DatabaseEntityRecordEntry createEntityRecordEntry() { return new DatabaseEntityRecordEntry(); }
    public DatabaseEntityRecord createEntityRecord()
    {
        return new DatabaseEntityRecord();
    }
    public DEDElements createDEDElements()
    {
       return new DEDElements();
    }

    public Elements createElements() { return new Elements(); }

    public DataBaseControl()
    {
        relativeENTITIES_DATABASE_PLACE         = "/DataDictionary/Database/ENTITIEs/";
        relativeENTITIES_DATABASE_TOAST_PLACE   = "/DataDictionary/Database/TOASTs/";
    }

    public EntityRealm readDDEntityRealm(File file, String EntityName) {
        EntityRealm _DDEntityRealm = new EntityRealm();
        try {

            File fXmlFile = file;
            DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
            Document doc = dBuilder.parse(fXmlFile);

            doc.getDocumentElement().normalize();
            String nodeName = doc.getDocumentElement().getNodeName();
            String expectedEntity = EntityName+"Realm";
            if( !expectedEntity.contentEquals(nodeName) )
                return null; // Error This is NOT the correct file

            NodeList nList = doc.getElementsByTagName("DDEntry");
            for (int temp = 0; temp < nList.getLength(); temp++) {

                Node nNode = nList.item(temp);
                if (nNode.getNodeType() == Node.ELEMENT_NODE) {

                    Element eElement = (Element) nNode;
                    DDEntityEntry newEntry = new DDEntityEntry();
                    newEntry.setDDGuid(eElement.getElementsByTagName("DDGuid").item(0).getTextContent());
                    newEntry.setPhysicalDataElementName(eElement.getElementsByTagName("PhysicalDataElementName").item(0).getTextContent());
                    newEntry.setLocation(eElement.getElementsByTagName("Location").item(0).getTextContent());
                    newEntry.setCategory(eElement.getElementsByTagName("Category").item(0).getTextContent());
                    newEntry.setDataType(eElement.getElementsByTagName("DataType").item(0).getTextContent());
                    newEntry.setMaxLength(eElement.getElementsByTagName("MaxLength").item(0).getTextContent());
                    newEntry.setDescription(eElement.getElementsByTagName("Description").item(0).getTextContent());
                    newEntry.setCharacteristics(eElement.getElementsByTagName("characteristics").item(0).getTextContent());
                    newEntry.setRelationship(eElement.getElementsByTagName("relationship").item(0).getTextContent());
                    newEntry.setMandatory(eElement.getElementsByTagName("Mandatory").item(0).getTextContent());
                    newEntry.setAccessrights(eElement.getElementsByTagName("accessrights").item(0).getTextContent());
                    _DDEntityRealm.add(newEntry);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return _DDEntityRealm;
    }

    public boolean readDDEntityRealm(File file, String EntityName, DEDElements dedElements) {
        boolean bResult = false;
        EntityRealm _DDEntityRealm = new EntityRealm();
        try {

            File fXmlFile = file;
            DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
            Document doc = dBuilder.parse(fXmlFile);

            doc.getDocumentElement().normalize();
            String nodeName = doc.getDocumentElement().getNodeName();
            String expectedEntity = EntityName+"Realm";
            if( !expectedEntity.contentEquals(nodeName) )
                return false; // Error This is NOT the correct file

            NodeList nList = doc.getElementsByTagName("DDEntry");
            for (int temp = 0; temp < nList.getLength(); temp++) {
                Node nNode = nList.item(temp);
                if (nNode.getNodeType() == Node.ELEMENT_NODE) {
                    Element eElement = (Element) nNode;
                    DDEntityEntry newEntry = new DDEntityEntry();
                    Elements elm = new Elements();
                    newEntry.setPhysicalDataElementName(eElement.getElementsByTagName("PhysicalDataElementName").item(0).getTextContent());
                    elm.setStrElementID(newEntry.getPhysicalDataElementName());
                    dedElements.add(elm);
                    _DDEntityRealm.add(newEntry);
                    bResult = true;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return bResult;
    }

    /**
     * -- pronounced FetchGet
     * it fetches one record from a specific realm in the database
     * the database consists of flat files and each file has an internal structure descriped in DataDictionary .xml files
     * the main structure of a file looks like this:
     *
     *  <?xml version="1.0" encoding="utf-8"?>
     *  <Entity>
     *   <version>1</version>
     *   <EntityRecord>
     *     <TransGUID></TransGUID> -- each record has a unique transaction id to be used with security, when detecting inconsistencies
     *     <Protocol>DED</Protocol>
     *     <ProtocolVersion>1.0.0.0</ProtocolVersion>
     *     <DataSize></DataSize>
     *     <Data></Data> -- here is the pCompressedData from above protocol encoding, compression, encrypting algoritm -- currently DED (dataencoderdecoder)
     *     <DataMD5><DataMD5> -- this md5 checksum is used as an extra verification -- Data must be verified with this value before being used.
     *   </EntityRecord>
     *   ...
     *  </Entity>
     *
     * Inside the Data field lies yet another structure compressed with DED and described in DataDictionary under Entities following this file:
     * DD_<entity realm name>.xml  -- entity could fx. be 'Profile'
     * This 'front' file is to be used for fast lookup - only fields of primary importance is in this file, the rest are put inside the TOAST and
     * the toast file has its id in field '<entity realm name>_chunk_id' - this id maps to the toast file !!!
     *
     * inorder to handle dynamically adding more fields, and fields of oversize, then a TOAST structure is used
     * DD_<entity realm name>_TOAST.xml describes the 'toast - the Oversized Attribute Storage Technique' elements and the file
     * DD_<entity realm name>_TOAST_ATTRIBUTES.xml
     * describes the fields inside the toast
     *
     * The toast structure follows this format:
     * <record>
     *     <chunk_id></chunk_id>
     *     <aiid></aiid>
     *     <chunk_seq></chunk_seq>
     *     <chunk_data></chunk_data>
     * </record>
     *
     *
     * @param realm_name
     * @param index_name  -- translated internally to a filenamepath to the index file
     * @param record_value
     * @return true/false
     */
    public boolean ftgt(String realm_name, String index_name, DEDElements record_value )
    {
        boolean bResult = false;
        String EntityName       = realm_name;
        String EntityFileName   = relativeENTITIES_DATABASE_PLACE + index_name + ".xml";

        // now read the actual entity file and make sure it follows current datadictionary configuration
        bResult = ReadEntityFile(EntityName,EntityFileName,record_value);

        /// now fetch ALL elements with their attribute values  -- all incl. TOAST attributes
        /// this means that now we should fetch all attributes from the TOAST entity file
        if(bResult==false)
        {
            System.out.println("[ftgt] ERROR : reading file failed : Entity name : " + EntityName + " filename: " + EntityFileName );    /// no need to go further, something is wrong with the file
            return bResult;
        }
        else {//TODO: Consider putting below fetch from TOAST into ReadEntityFile, since toast is a part of an entity it should reside there
            bResult = FetchTOASTEntities(EntityName, record_value);
        }
        return bResult;
    }

    public boolean FetchTOASTEntities(String realm_name, DEDElements record_value)
    {
        boolean bResult = false;
        String EntityName = realm_name;

        /// now fetch ALL elements with their attribute values  -- all incl. TOAST attributes
        /// this means that now we should fetch all attributes from the TOAST entity file
        String ChunkID = (EntityName + "_chunk_id").toLowerCase();
        String ChunkIdValue = "";
        // finding the element
        List<Elements> resultElement = record_value.stream()
                                                    .filter((p)-> p.getStrElementID().equals((ChunkID)))
                                                    .collect(Collectors.toList());
        if(resultElement.size()>0)
        {
            byte[] bytes = resultElement.get(0).getElementData();
            try {
                ChunkIdValue = new String(bytes, "UTF-8"); // for UTF-8 encoding
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
            String TOASTFilePath =  relativeENTITIES_DATABASE_TOAST_PLACE + ChunkIdValue + ".xml";
            // now open its toast file and put all attributes and values on record_value
            File ToastFile  = new File(TOASTFilePath);
            if(ToastFile.exists()) {
                bResult = ReadTOASTXmlFile(ToastFile, record_value, EntityName);
                if(bResult==false)
                {
                    System.out.println("[FetchTOASTEntities] ERROR : File can not be read : file name : " + TOASTFilePath );    /// no need to go further, something is wrong with the file
                    return bResult;
                }
                // now all elements from Entity should have been read, including the ones in TOAST (they have also been merged, so no chunks exists)
            }
            else
            {
                // warning there could not be found a TOAST file, this is not necessary an error, however strange
                System.out.println("[FetchTOASTEntities] WARNING: there could not be found a TOAST file, this is not necessary an error, however strange : " + TOASTFilePath );
                bResult=true;
            }
        }

        return bResult;
    }

    /**
     * Read a database xml file which follows following format:
     *
     *  <?xml version="1.0" encoding="utf-8"?>
     *  <Entity>
     *   <version>1</version>
     *   <EntityRecord>
     *     <TransGUID></TransGUID> -- each record has a unique transaction id to be used with security, when detecting inconsistencies
     *     <Protocol>DED</Protocol>
     *     <ProtocolVersion>1.0.0.0</ProtocolVersion>
     *     <DataSize></DataSize>
     *     <Data></Data> -- here is the pCompressedData from above protocol encoding, compression, encrypting algoritm -- currently DED (dataencoderdecoder)
     *     <DataMD5><DataMD5> -- this md5 checksum is used as an extra verification -- Data must be verified with this value before being used.
     *   </EntityRecord>
     *
     * @param file
     * @param records
     * @param EntityName
     * @return
     */
    public boolean ReadXmlFile(File file, DatabaseEntityRecord records, String EntityName) {
        boolean bResult = false;

        String Child = EntityName;
        String ChildRecord = EntityName + "Record";

        File fXmlFile = file;
        DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder dBuilder;
        Document doc = null;
        try {
            dBuilder = dbFactory.newDocumentBuilder();
            doc = dBuilder.parse(fXmlFile);
        } catch (Exception e) {
            e.printStackTrace();
        }

        doc.getDocumentElement().normalize();
        String nodeName = doc.getDocumentElement().getNodeName();
        String expectedEntity = Child;
        if (!expectedEntity.contentEquals(nodeName))
            return false; // Error This is NOT the correct file

        NodeList nList = doc.getElementsByTagName(Child);
        for (int temp = 0; temp < nList.getLength(); temp++) {
            Node nNode = nList.item(temp);
            if (nNode.getNodeType() == Node.ELEMENT_NODE) {
                Element eElement = (Element) nNode;
                DatabaseEntityRecordEntry record = new DatabaseEntityRecordEntry();
                record.setTransGUID(eElement.getElementsByTagName("TransGUID").item(0).getTextContent());
                record.setProtocol(eElement.getElementsByTagName("Protocol").item(0).getTextContent());
                record.setProtocolVersion(eElement.getElementsByTagName("ProtocolVersion").item(0).getTextContent());
                record.setDataSize(Integer.parseInt(eElement.getElementsByTagName("DataSize").item(0).getTextContent()));
                record.setData(eElement.getElementsByTagName("Data").item(0).getTextContent());
                record.setDataMD5(eElement.getElementsByTagName("DataMD5").item(0).getTextContent());
                records.add(record);
                bResult=true;
            }
        }

        return bResult;
    }

    public String CalculateMD5CheckSum(byte[] bytes)
    {
        MessageDigest md = null;
        String md5 = "";
        try {
            md = MessageDigest.getInstance("MD5");
            md.update(bytes);
            byte[] md5Bytes = md.digest();
            //convert the byte to hex format
            StringBuffer sb = new StringBuffer("");
            for (int i = 0; i < md5Bytes.length; i++) {
                sb.append(Integer.toString((md5Bytes[i] & 0xff) + 0x100, 16).substring(1));
            }
            md5 = sb.toString();
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        return md5;
    }

    /**
     *
     * Will append IncommingData to a new enlarged CurrentBuffer
     *
     * @param IncommingData
     * @param CurrentBuffer
     * @return new buffer with old and appended data
     */
    public byte[] back_inserter(byte[] IncommingData, byte[] CurrentBuffer)
    {
        if(CurrentBuffer == null)
            CurrentBuffer = new byte[0]; // assign an empty buffer
        if(IncommingData == null)
            return CurrentBuffer;  // no data to append, hence return CurrentBuffer
        int CurrentBufferSize = CurrentBuffer.length;
        int IncomingDataSize = IncommingData.length;
        int AppendPosition    = CurrentBufferSize;
        int NewBufferSize     = CurrentBufferSize + IncomingDataSize;
        byte[] NewBuffer      = new byte[NewBufferSize];

        System.arraycopy(CurrentBuffer,0,NewBuffer,0,CurrentBufferSize); // First move previous data to new larger space
        System.arraycopy(IncommingData,0,NewBuffer,AppendPosition,IncomingDataSize); // Second append new data

        return NewBuffer;
    }

    /**
     * Read a database TOAST xml file which follows this format:
     *
     *  <?xml version="1.0" encoding="utf-8"?>
     *  <Entity>
     *   <version>1</version>
     *   <EntityRecord>
     *     <TransGUID></TransGUID> -- each record has a unique transaction id to be used with security, when detecting inconsistencies
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
     *     <DataMD5><DataMD5> -- this md5 checksum is used as an extra verification -- Data must be verified with this value before being used.
     *   </EntityRecord>

     * @param file
     * @param records_elements
     * @param EntityName
     * @return
     */
    public boolean ReadTOASTXmlFile(File file, DEDElements records_elements, String EntityName)
    {
        boolean bResult=true;

        String Child = EntityName;
        String ChildRecord = EntityName + "Record";

        File fXmlFile = file;
        DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder dBuilder;
        Document doc = null;
        try {
            dBuilder = dbFactory.newDocumentBuilder();
            doc = dBuilder.parse(fXmlFile);
        } catch (Exception e) {
            e.printStackTrace();
        }

        doc.getDocumentElement().normalize();
        String nodeName = doc.getDocumentElement().getNodeName();
        String expectedEntity = Child;
        if (!expectedEntity.contentEquals(nodeName))
            return false; // Error This is NOT the correct file

        String PrevChunkId = "nothing";
        boolean isPushed=false;

        Elements Element = new Elements();
        NodeList nList = doc.getElementsByTagName(ChildRecord);
        for (int temp = 0; temp < nList.getLength(); temp++) {
            Node nNode = nList.item(temp);
            if (nNode.getNodeType() == Node.ELEMENT_NODE) {
                Element eElement = (Element) nNode;
                DatabaseEntityRecordEntry record = new DatabaseEntityRecordEntry();
                record.setTransGUID(eElement.getElementsByTagName("TransGUID").item(0).getTextContent());
                record.setProtocol(eElement.getElementsByTagName("Protocol").item(0).getTextContent());
                record.setProtocolVersion(eElement.getElementsByTagName("ProtocolVersion").item(0).getTextContent());
                record.setDataSize(Integer.parseInt(eElement.getElementsByTagName("DataSize").item(0).getTextContent()));
                record.setData(eElement.getElementsByTagName("Data").item(0).getTextContent());
                record.setDataMD5(eElement.getElementsByTagName("DataMD5").item(0).getTextContent());

                String md5 = CalculateMD5CheckSum(record.getData().getBytes());
                if (!md5.equalsIgnoreCase(record.getDataMD5()))
                {
                    System.out.println("ERROR: data area in file have changed without having the MD5 checksum changed -- Warning data could be compromised ; " + Child );
                    return false;  // data area in file have changed without having the MD5 checksum changed -- Warning data could be compromised
                }

                byte[] DataInUnHexedBuffer = DatatypeConverter.parseHexBinary(record.getData());

                // According to what the protocol prescribes for DED entries in TOAST, then a loop of decode, according to specs, of entries is needed
                // fetch the data area and unpack it with DED to check it
                EntityChunkDataInfo chunk = new EntityChunkDataInfo();
                DEDDecoder DED = new DEDDecoder();
                DED.PUT_DATA_IN_DECODER(DataInUnHexedBuffer,DataInUnHexedBuffer.length);
                String EntityChunkId    = (Child + "_chunk_id").toLowerCase(); // eg. profile_chunk_id
                String EntityChunkSeq   = (Child + "_chunk_seq").toLowerCase(); // eg. profile_chunk_seq
                String EntityChunkData  = (Child + "_chunk_data").toLowerCase(); // eg. profile_chunk_data
                // decode data ...
                DED.GET_STRUCT_START( "record" );
                chunk.entity_chunk_id   = DED.GET_STDSTRING	( EntityChunkId ); // key of particular item
                chunk.aiid              = DED.GET_ULONG   	( "aiid" ); // this number is continuously increasing all throughout the entries in this table
                chunk.entity_chunk_seq  = DED.GET_ULONG   	( EntityChunkSeq ); // sequence number of particular item
                chunk.entity_chunk_data = DED.GET_STDVECTOR	( EntityChunkData ); //
                DED.GET_STRUCT_END( "record" );

                if(PrevChunkId.contentEquals("nothing") || !PrevChunkId.contentEquals(chunk.entity_chunk_id))
                {
                    if(!PrevChunkId.contentEquals("nothing") && !PrevChunkId.contentEquals(chunk.entity_chunk_id))
                    {
                        records_elements.add(Element);
                        Element = new Elements();
                        isPushed=true;
                    }

                    // new Element
                    Element.strElementID = chunk.entity_chunk_id;
                    Element.ElementData = new byte[0];
                    PrevChunkId = chunk.entity_chunk_id;
                    isPushed=false;
                }
                // this will, chunk by chunk, assemble the element data - fx. a foto element consists of many chunks in the file
                Element.setElementData(back_inserter(chunk.entity_chunk_data, Element.ElementData));
            }
        }
        // should only add last element if chunks have been assembled
        if(isPushed==false){
            records_elements.add(Element);
        }

        return bResult;
    }

    public boolean ReadEntityFile(String EntityName, String EntityFileName, DEDElements dedElements)
    {
        boolean bResult=false;

        String FilePath = EntityFileName;

        // NB! DataDictionary is embedded as resource files
        String uppercaseEntityName = "DataDictionary/Entities/DD_" + EntityName.toUpperCase()  + ".xml";  // example: DataDictionary/DD_CUSTOMER.xml
        File dataDictionaryFile  = new File(this.getClass().getClassLoader().getResource(uppercaseEntityName).getFile());
        if(dataDictionaryFile.exists()) {
            bResult = readDDEntityRealm(dataDictionaryFile, EntityName, dedElements);
            if(!bResult) {
                System.out.println("[ReadEntityFile] ERROR : DataDictionary File can not be read - please check access rights : " + uppercaseEntityName);
                return bResult;
            }
            // Read data from file
            DatabaseEntityRecord EntityRecordResult = new DatabaseEntityRecord();
            {
                File databaseFile = new File(FilePath);
                if (!databaseFile.exists()) {
                    System.out.println("[ReadEntityFile] File NOT found : " + FilePath);
                    bResult = false;
                }
                else
                    bResult = ReadXmlFile(databaseFile, EntityRecordResult, EntityName);

            }
            if(!bResult)
                return bResult;

            bResult=true;
            // now traverse record
            for (DatabaseEntityRecordEntry f: EntityRecordResult) {
                // take the hex converted data and unhex it before DED will decode it
                byte[] data_in_unhexed_buf = DatatypeConverter.parseHexBinary(f.getData());

                // fetch the data area and unpack it with DED to check it
                DEDDecoder DED = new DEDDecoder();
                DED.PUT_DATA_IN_DECODER( data_in_unhexed_buf, data_in_unhexed_buf.length ); // data should be decompressed using LZSS algorithm
                if( DED.GET_STRUCT_START( "record" )==1 ) {
                    int n=0;
                    for (Elements e: dedElements) {
                        String tmp = ""; // clear
                        if((tmp = DED.GET_STDSTRING(e.getStrElementID())) != "")
                        {
                            // put attribute values from entity file into DEDElements
                            Elements elm = dedElements.get(n);
                            elm.setElementData(tmp.getBytes());
                            dedElements.set(n,elm);
                            n++;
                        }
                        else
                        {
                            System.out.println("[ReadEntityFile] WARNING : DECODING ERROR: elementID : " + e.getStrElementID());
                            bResult=false;
                            break;
                        }
                    }
                }

            }
        }
        return bResult;
    }
}
