package dops.database;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

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

        public ArrayList<?> getElementData() {
            return ElementData;
        }

        public void setElementData(ArrayList<?> elementData) {
            ElementData = elementData;
        }

        String strElementID;
        ArrayList<?> ElementData;
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

    public boolean ftgt(String realm_name, String index_name, DEDElements record_value )
    {
        boolean bResult = false;
        String EntityName       = realm_name;
        String EntityFileName   = index_name;
        DEDElements _DEDElements = new DEDElements();

        // NB! DataDictionary is embedded as resource files
/*
        String uppercaseEntityName = "DD_" + EntityName.toUpperCase()  + ".xml";  // example: DD_CUSTOMER.xml
        File file  = new File(this.getClass().getClassLoader().getResource(uppercaseEntityName).getFile());
        if(file.exists()) {
            bResult = readDDEntityRealm(file, EntityName, _DEDElements);
            if(bResult == false)
            {
                System.out.println("[ftgt] ERROR : reading file failed : Entity name : " + EntityName + " filename: " + EntityFileName);
            }
            else {
                // now read the actual entity file and make sure it follows current datadictionary configuration
                //bResult = ReadEntityFile(EntityName,EntityFileName,record_value);
            //}
*/

        // now read the actual entity file and make sure it follows current datadictionary configuration
        bResult = ReadEntityFile(EntityName,EntityFileName,record_value);

        /// now fetch ALL elements with their attribute values  -- all incl. TOAST attributes
        /// this means that now we should fetch all attributes from the TOAST entity file

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
        String uppercaseEntityName = "DD_" + EntityName.toUpperCase()  + ".xml";  // example: DD_CUSTOMER.xml
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

            bResult=false;
            // now traverse record
            //TODO: FOREACH

        }
        return bResult;
    }
}
