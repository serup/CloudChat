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
import java.util.ArrayList;

/**
 * Created by serup on 13-04-16.
 */
public class DataBaseControl {

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

        String strElementID;
        byte[] ElementData;
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

    public class EntityRealm extends ArrayList<DDEntityEntry> { }
    public class DEDElements extends ArrayList<Elements> { }
    public class DatabaseEntityRecord extends ArrayList<DatabaseEntityRecordEntry> { }

    public DatabaseEntityRecord createEntityRecord()
    {
        return new DatabaseEntityRecord();
    }

    public DEDElements createDEDElements()
    {
       return new DEDElements();
    }

    public DataBaseControl()
    {
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
     * -- pronounsed FetchGet
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
     * This 'front' file is to be used for fast lookup - only fields of primary importance is in this file, the rest are put inside the TOAST
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
     * @param index_name
     * @param record_value
     * @return true/false
     */
    public boolean ftgt(String realm_name, String index_name, DEDElements record_value )
    {
        boolean bResult = false;
        String EntityName       = realm_name;
        String EntityFileName   = index_name;
        DEDElements _DEDElements = new DEDElements();

        // now read the actual entity file and make sure it follows current datadictionary configuration
        bResult = ReadEntityFile(EntityName,EntityFileName,record_value);

        /// now fetch ALL elements with their attribute values  -- all incl. TOAST attributes
        /// this means that now we should fetch all attributes from the TOAST entity file
//TODO: - here
        return bResult;
    }


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
