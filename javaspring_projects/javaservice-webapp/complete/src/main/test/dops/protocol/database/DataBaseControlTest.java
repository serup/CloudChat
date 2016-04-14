package dops.protocol.database;


import dops.database.DataBaseControl;
import org.junit.Before;
import org.junit.Test;

import java.io.File;
import java.util.Iterator;

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
        String fileResource="DD_CUSTOMER.xml";
        File file = new File(this.getClass().getClassLoader().getResource(fileResource).getFile());

        DataBaseControl dataBaseControl = new DataBaseControl();
        DataBaseControl.DEDElements dedElements = dataBaseControl.createDEDElements();
        boolean bResult = this.dbctrl.readDDEntityRealm(file, "Customer", dedElements);
        for (DataBaseControl.Elements e: dedElements) {
            System.out.println(e.getStrElementID());
        }
    }

}
