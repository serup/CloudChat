package dops.protocol.database;


import dops.database.DataBaseControl;
import org.junit.Before;
import org.junit.Test;

import java.io.File;
import java.nio.file.Files;
import java.util.Iterator;

import static junit.framework.TestCase.assertEquals;

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

        String fileResource= "DataDictionary/DD_CUSTOMER.xml";
        File file = new File(this.getClass().getClassLoader().getResource(fileResource).getFile());
        DataBaseControl.EntityRealm entityRealm = dbctrl.readDDEntityRealm(file, "Customer");

        DataBaseControl.DDEntityEntry foundEntry;
        Iterator<DataBaseControl.DDEntityEntry> i;
        for (i  = entityRealm.iterator(); i.hasNext(); ) {
            foundEntry = i.next();
            System.out.println("DDGuid : " + foundEntry.getDDGuid() );
            System.out.println("PhysicalDataElementName : " + foundEntry.getPhysicalDataElementName() );
            System.out.println("Location : " + foundEntry.getLocation() );
            System.out.println("Category : " + foundEntry.getCategory() );
            System.out.println("DataType : " + foundEntry.getDataType() );
            System.out.println("MaxLength : " + foundEntry.getMaxLength() );
            System.out.println("Description : " + foundEntry.getDescription() );
            System.out.println("characteristics : " + foundEntry.getCharacteristics() );
            System.out.println("relationship : " + foundEntry.getRelationship() );
            System.out.println("Mandatory : " + foundEntry.getMandatory() );
            System.out.println("accessrights : " + foundEntry.getAccessrights() );
        }
    }

    @Test
    public void testReadDDwithElements() throws Exception {
        String fileResource= "DataDictionary/DD_CUSTOMER.xml";
        File file = new File(this.getClass().getClassLoader().getResource(fileResource).getFile());

        DataBaseControl.DEDElements dedElements = this.dbctrl.createDEDElements();
        boolean bResult = this.dbctrl.readDDEntityRealm(file, "Customer", dedElements);
        assertEquals(true, bResult);
        for (DataBaseControl.Elements e: dedElements) {
            System.out.println(e.getStrElementID());
        }
    }

    @Test
    public void testReadXmlFile() throws Exception {
        // This is a dummy database file - it is following protocol as DataDictionary is stating in its DataDictionary/Entities/DD_TOAST.xml file
        String fileResource= "DataDictionary/Database/ENTITIEs/355760fb6afaf9c41d17ac5b9397fd45.xml"; // This is a profile database file
        File file = new File(this.getClass().getClassLoader().getResource(fileResource).getFile());

        DataBaseControl.DatabaseEntityRecord record = this.dbctrl.createEntityRecord();
        boolean bResult = this.dbctrl.ReadXmlFile(file, record, "Profile");
        assertEquals(true, bResult);
        for (DataBaseControl.DatabaseEntityRecordEntry e: record) {
            System.out.println(e.getDataMD5());
        }
    }

    @Test
    public void testReadEntitiyFile() throws Exception {
        // This is a dummy database file - it is following protocol as DataDictionary is stating in its DataDictionary/Entities/DD_TOAST.xml file
        String fileResource = "DataDictionary/Database/ENTITIEs/355760fb6afaf9c41d17ac5b9397fd45.xml"; // This is a profile database file
        String EntityFileName = "/tmp/355760fb6afaf9c41d17ac5b9397fd45.xml"; // This is the extracted file on local
        String EntityName       = "Profile"; // since this file is a customer profile database file, its entity name is 'Profile' -- DD_PROFILE.xml MUST reside in DataDictionary/Entities

        // 1. extract resource dummy file to local filesystem
        File resourceFile       = new File(this.getClass().getClassLoader().getResource(fileResource).getFile());
        File destinationFile    = new File(EntityFileName);
        Files.deleteIfExists(destinationFile.toPath());
        Files.copy(resourceFile.toPath(), destinationFile.toPath());

        // 2. use it as input to method to be tested
        DataBaseControl.DEDElements record_value = this.dbctrl.createDEDElements(); // Placeholder for retrieved DataEncoderDecoder elements
        boolean bResult =  this.dbctrl.ReadEntityFile(EntityName,EntityFileName,record_value);

        // 3. validate result
        assertEquals(true, bResult);
        for (DataBaseControl.Elements e: record_value) {
            System.out.println(e.getStrElementID());
        }
    }
}
