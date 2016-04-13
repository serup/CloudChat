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


    public class EntityRealm extends ArrayList<DDEntityEntry> { }

    public DataBaseControl() {
    }

    public EntityRealm readDDEntityRealm(File file, String EntityName) {
        EntityRealm _DDEntityRealm = new EntityRealm();
        try {

            File fXmlFile = file;
            DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
            Document doc = dBuilder.parse(fXmlFile);

            //optional, but recommended
            //read this - http://stackoverflow.com/questions/13786607/normalization-in-dom-parsing-with-java-how-does-it-work
            doc.getDocumentElement().normalize();
            String nodename = doc.getDocumentElement().getNodeName();
            String expectedEntity = EntityName+"Realm";
            if( !expectedEntity.contentEquals(nodename) )
                return null; // Error This is NOT the correct file

            //System.out.println("Root element :" + nodename);

            NodeList nList = doc.getElementsByTagName("DDEntry");

            //System.out.println("----------------------------");

            for (int temp = 0; temp < nList.getLength(); temp++) {

                Node nNode = nList.item(temp);

                //System.out.println("\n- " + nNode.getNodeName());

                if (nNode.getNodeType() == Node.ELEMENT_NODE) {

                    Element eElement = (Element) nNode;

/*
                    System.out.println("DDGuid : " + eElement.getElementsByTagName("DDGuid").item(0).getTextContent());
                    System.out.println("PhysicalDataElementName : " + eElement.getElementsByTagName("PhysicalDataElementName").item(0).getTextContent());
                    System.out.println("Location : " + eElement.getElementsByTagName("Location").item(0).getTextContent());
                    System.out.println("Category : " + eElement.getElementsByTagName("Category").item(0).getTextContent());
                    System.out.println("DataType : " + eElement.getElementsByTagName("DataType").item(0).getTextContent());
                    System.out.println("MaxLength : " + eElement.getElementsByTagName("MaxLength").item(0).getTextContent());
                    System.out.println("Description : " + eElement.getElementsByTagName("Description").item(0).getTextContent());
                    System.out.println("characteristics : " + eElement.getElementsByTagName("characteristics").item(0).getTextContent());
                    System.out.println("relationship : " + eElement.getElementsByTagName("relationship").item(0).getTextContent());
                    System.out.println("Mandatory : " + eElement.getElementsByTagName("Mandatory").item(0).getTextContent());
                    System.out.println("accessrights : " + eElement.getElementsByTagName("accessrights").item(0).getTextContent());
*/

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
}
