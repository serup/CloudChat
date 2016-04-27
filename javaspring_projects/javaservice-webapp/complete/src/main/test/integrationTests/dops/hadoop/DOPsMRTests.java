package integrationTests.dops.hadoop;

import dops.hadoop.handlers.DOPsHDFSHandler;
import dops.hadoop.mappers.ProfileFileMapper;
import dops.hadoop.reducers.ProfileFileReducer;
import integrationTests.IntegrationEnvironmentSetup;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.junit.Before;
import org.junit.Test;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.assertEquals;
import static org.mortbay.util.LazyList.contains;

/**
 * Created by serup on 26-04-16.
 */
public class DOPsMRTests {

    DOPsHDFSHandler fshandlerDriver;


    /*
    */
    IntegrationEnvironmentSetup env = new IntegrationEnvironmentSetup();

    @Before
    public void setUp() throws Exception {
        assertEquals(true,env.setupHadoopIntegrationEnvironment());
        fshandlerDriver = new DOPsHDFSHandler();

        File directory = new File("/tmp/output/findprofile");
        //make sure directory exists
        if (!directory.exists()) {
            // Directory does not exist
            directory.mkdir();
        }else {
            try {
                env.delete(directory); // clean out old
                directory.mkdir(); // create for this test
            } catch (IOException e) {
                e.printStackTrace();
                System.exit(0);
            }
        }

    }

    @Test
    public void testFindProfileFile() {

        try {
            Configuration conf = new Configuration();

            conf.set("fs.defaultFS", "hdfs://one.cluster/");
            conf.set("hadoop.job.ugi", "root, supergroup");
            conf.set("hadoop.home.dir", "/usr/local/hadoop");
            conf.set("dfs.domain.socket.path", "/var/lib/hadoop-hdfs/dn_socket");
            conf.set("dops.entities.database.dir", "hdfs://one.cluster:8020/tmp/input/findprofile/");
            conf.set("dops.toast.database.dir", "hdfs://one.cluster:8020/tmp/input/findprofile/toast/");
            conf.set("dops.elementofinterest", "username");
            conf.set("dops.elementofinterest.value", "johnnytest@email.com");

            conf.setBoolean("mapreduce.map.speculative", false);
            conf.setBoolean("mapreduce.reduce.speculative", false);

            Job job = null;
            job = Job.getInstance(conf);
            job.setJobName("FindProfileFile");
            job.setJarByClass(ProfileFileMapper.class);
            job.setMapperClass(ProfileFileMapper.class);
            job.setReducerClass(ProfileFileReducer.class);
            job.setOutputKeyClass(Text.class);
            job.setOutputValueClass(Text.class);

            FileInputFormat.addInputPath(job, new Path("hdfs://one.cluster:8020/tmp/input/findprofile/*.xml"));
            FileOutputFormat.setOutputPath(job, new Path("/tmp/output/findprofile/result/"));

            // copy the internal resource file watson.txt to remote hadoop hdfs system
            String fileResource1 = "DataDictionary/Database/ENTITIEs/355760fb6afaf9c41d17ac5b9397fd45.xml"; // This is a profile database file
            String fileResource2 = "DataDictionary/Database/TOASTs/355760fb6afaf9c41d17ac5b9397fd45_toast.xml"; // This is a profile toast database file
            String destFolder="tmp/input/findprofile";
            URL fileResourceUrl = this.getClass().getClassLoader().getResource(fileResource1);
            URL fileResourceUrl2 = this.getClass().getClassLoader().getResource(fileResource2);
            fshandlerDriver.copyTo(fileResourceUrl.getPath(), "/"+destFolder+"/355760fb6afaf9c41d17ac5b9397fd45.xml");
            fshandlerDriver.copyTo(fileResourceUrl2.getPath(), "/"+destFolder+"/toast"+"/355760fb6afaf9c41d17ac5b9397fd45_toast.xml");

            // verify that the input file is in hdfs
            List<String> itemsToAdd = new ArrayList<String>();
            String filepathname1 = destFolder+"/"+"355760fb6afaf9c41d17ac5b9397fd45.xml";
            String filepathname2 = destFolder+"/toast/"+"355760fb6afaf9c41d17ac5b9397fd45_toast.xml";
            itemsToAdd.add(fshandlerDriver.uri+filepathname1);
            itemsToAdd.add("hdfs://one.cluster:8020/tmp/input/findprofile/toast");
            assertEquals("Expected 2 item in hdfs ls list", itemsToAdd, fshandlerDriver.ls("/tmp/input/findprofile/"));
            itemsToAdd = new ArrayList<String>();
            itemsToAdd.add(fshandlerDriver.uri+filepathname2);
            assertEquals("Expected 1 item in hdfs ls list", itemsToAdd, fshandlerDriver.ls("/tmp/input/findprofile/toast"));

            job.waitForCompletion(true);
        } catch (IOException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }

        // This should result in a result file on LOCAL file system
        // see following example:
        // serup@serup-ThinkPad-T440s:~$ ls /tmp/output/findprofile/result/
        // part-r-00000  _SUCCESS
        // serup@serup-ThinkPad-T440s:~$ cat /tmp/output/findprofile/result/part-r-00000
        // <result>\t\n<file>355760fb6afaf9c41d17ac5b9397fd45.xml</file>\t\n</result>\t\n
        //
        // above shows that the field searched for was found in <file>

        String result = env.executeCmd("cat /tmp/output/findprofile/result/part-r-00000", "/");
        assertEquals(true, contains(result, new String("<result>\t\n<file>355760fb6afaf9c41d17ac5b9397fd45.xml</file>\t\n</result>\t\n")));

        System.out.println("FindProfileFile - mapreduce job finished - status : SUCCESS");
    }

}

