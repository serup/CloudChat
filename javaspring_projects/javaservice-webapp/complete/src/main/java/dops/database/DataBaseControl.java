package dops.database;

import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import java.io.*;
import java.util.ArrayList;

/**
 * Created by serup on 13-04-16.
 */
public class DataBaseControl {

    class DDEntityEntry
    {
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
        EntityRealm _DDEntityRealm = null;
        try {

            SAXParserFactory factory = SAXParserFactory.newInstance();
            SAXParser saxParser = factory.newSAXParser();

            DefaultHandler handler = new DefaultHandler() {
            /*
                boolean bfname = false;
                boolean blname = false;
                boolean bnname = false;
                boolean bsalary = false;
            */
                public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {

                    System.out.println("Start Element :" + qName);

                   /* if (qName.equalsIgnoreCase("FIRSTNAME")) {
                        bfname = true;
                    }

                    if (qName.equalsIgnoreCase("LASTNAME")) {
                        blname = true;
                    }

                    if (qName.equalsIgnoreCase("NICKNAME")) {
                        bnname = true;
                    }

                    if (qName.equalsIgnoreCase("SALARY")) {
                        bsalary = true;
                    }
                    */
                }

                public void endElement(String uri, String localName, String qName) throws SAXException {
                    System.out.println("End Element :" + qName);
                }

                public void characters(char ch[], int start, int length) throws SAXException {

                    System.out.println(new String(ch, start, length));
                    /*
                    if (bfname) {
                        System.out.println("First Name : "
                                + new String(ch, start, length));
                        bfname = false;
                    }

                    if (blname) {
                        System.out.println("Last Name : "
                                + new String(ch, start, length));
                        blname = false;
                    }

                    if (bnname) {
                        System.out.println("Nick Name : "
                                + new String(ch, start, length));
                        bnname = false;
                    }

                    if (bsalary) {
                        System.out.println("Salary : "
                                + new String(ch, start, length));
                        bsalary = false;
                    }
                    */
                }

            };

            //File file = new File("c:\\file.xml");
            InputStream inputStream= new FileInputStream(file);
            Reader reader = new InputStreamReader(inputStream,"UTF-8");

            InputSource is = new InputSource(reader);
            is.setEncoding("UTF-8");

            saxParser.parse(is, handler);

        } catch (Exception e) {
            e.printStackTrace();
        }

        return _DDEntityRealm;
    }
}
