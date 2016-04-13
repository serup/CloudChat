package dops.protocol.database;


import dops.database.DataBaseControl;
import org.junit.Before;
import org.junit.Test;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.File;
/**
 * Created by serup on 13-04-16.
 */
public class DataBaseControlTest {

    DataBaseControl dbctrl;

    @Before
    public void setUp() throws Exception {
        dbctrl = new DataBaseControl();
    }

    @Test
    public void testReadDataDictionaryDefinition() throws Exception {
        // Fetch from Data Dictionary the xml description of Customer Realm in the database
        // xml is following this setup:
        // <?xml version="1.0" encoding="utf-8"?>
        // <CustomerRealm>
        // <version>1</version>
        // <DDEntry>
        // <DDGuid></DDGuid>
        // <PhysicalDataElementName>customerID</PhysicalDataElementName>
        // <Location></Location>
        // <Category>RawData</Category>
        // <DataType>std::string</DataType>
        // <MaxLength>32</MaxLength>
        // <Description></Description>
        // <characteristics></characteristics>
        // <relationship></relationship>
        // <Mandatory>YES</Mandatory>
        // <accessrights></accessrights>
        // </DDEntry>
        // ..... more DDEntries
        // </CustomerRealm>

        String fileResource="DD_CUSTOMER.xml";
        File file = new File(this.getClass().getClassLoader().getResource(fileResource).getFile());
        //dbctrl.readDDEntityRealm(file, "Customer");

        try {

            File fXmlFile = file;
            DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
            Document doc = dBuilder.parse(fXmlFile);

            //optional, but recommended
            //read this - http://stackoverflow.com/questions/13786607/normalization-in-dom-parsing-with-java-how-does-it-work
            doc.getDocumentElement().normalize();

            System.out.println("Root element :" + doc.getDocumentElement().getNodeName());

            NodeList nList = doc.getElementsByTagName("DDEntry");

            System.out.println("----------------------------");

            for (int temp = 0; temp < nList.getLength(); temp++) {

                Node nNode = nList.item(temp);

                System.out.println("\n- " + nNode.getNodeName());

                if (nNode.getNodeType() == Node.ELEMENT_NODE) {

                    Element eElement = (Element) nNode;

                    //System.out.println("PhysicalDataElementName : " + eElement.getAttribute("PhysicalDataElementName"));
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
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

    }


}
