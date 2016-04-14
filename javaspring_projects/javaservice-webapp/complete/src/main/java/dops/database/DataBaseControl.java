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

    public class EntityRealm extends ArrayList<DDEntityEntry> { }
    public class DEDElements extends ArrayList<Elements> { }

    public DEDElements createDEDElements()
    {
       return new DEDElements();
    }

    public DataBaseControl() {
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
}