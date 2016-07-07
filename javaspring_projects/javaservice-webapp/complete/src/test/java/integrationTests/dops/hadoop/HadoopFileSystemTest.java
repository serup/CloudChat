package integrationTests.dops.hadoop;

import dops.hadoop.handlers.DOPsHDFSHandler;
import integrationTests.IntegrationEnvironmentSetup;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

import static junit.framework.TestCase.assertEquals;

/**
 * Created by serup on 22-02-16.
 */
public class HadoopFileSystemTest {

    private boolean bSetupOK=true;
    DOPsHDFSHandler fshandlerDriver;

    IntegrationEnvironmentSetup env = new IntegrationEnvironmentSetup();

    @Before
    public void setUp() throws Exception {
        bSetupOK = env.setupHadoopIntegrationEnvironment();
        try{
            fshandlerDriver = new DOPsHDFSHandler();
        }
        catch (Exception e) {
            bSetupOK=false;
            e.printStackTrace();
        }
        assertEquals(true,bSetupOK);
    }

    @Test
    public void test_hdfs_ls() {
        List<String> itemsToAdd = new ArrayList<String>();
        itemsToAdd.add("hdfs://one.cluster:8020/apps");
        itemsToAdd.add("hdfs://one.cluster:8020/mapred");
        itemsToAdd.add("hdfs://one.cluster:8020/tmp");
        itemsToAdd.add("hdfs://one.cluster:8020/user");
        assertEquals("Expected 4 items in hdfs ls list", itemsToAdd, fshandlerDriver.ls("/"));
    }

    @Test
    public void test_hdfs_touch_and_remove() {
        try {
            fshandlerDriver.touch("/tmp/newfile2.txt");
            fshandlerDriver.remove("/tmp/newfile2.txt");
        } catch (IOException e) {
            e.printStackTrace();
            bSetupOK=false;
        }
        assertEquals(true,bSetupOK);
    }

    @Test
    public void test_copy_file_to_hdfs() {
        try {
            String fileResource= "dummyFiles/helloworld.txt";
            String fileResourceWithoutPath = "helloworld.txt";
            String destFolder="tmp";
            URL fileResourceUrl = this.getClass().getClassLoader().getResource(fileResource);
            if(fshandlerDriver.copyTo(fileResourceUrl.getPath(), "/"+destFolder)) {

                // verify that file is in hdfs
                List<String> itemsToAdd = new ArrayList<String>();
                String filepathname = destFolder + "/" + fileResourceWithoutPath;
                itemsToAdd.add(fshandlerDriver.uri + filepathname);
                assertEquals("Expected 1 item in hdfs ls list", itemsToAdd, fshandlerDriver.ls("/" + filepathname));

                // cleanup - when after testing - file will be deleted on exit
                String newfileName = "/" + destFolder + "/" + fileResourceWithoutPath;
                fshandlerDriver.remove(newfileName);
            }
            else
                throw new IOException("could NOT copy file to HDFS");
        } catch (Exception e) {
            e.printStackTrace();
            bSetupOK=false;
        }
        assertEquals(true,bSetupOK);

    }

}
