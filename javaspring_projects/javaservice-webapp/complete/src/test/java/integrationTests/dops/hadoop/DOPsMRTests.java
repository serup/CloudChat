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

public class DOPsMRTests {

    private DOPsHDFSHandler fshandlerDriver;
    private final IntegrationEnvironmentSetup env = new IntegrationEnvironmentSetup();
    private boolean bSetupOK=true;


    @SuppressWarnings("unused")
    @Before
    public void setUp() {
        assertEquals(true,env.setupHadoopIntegrationEnvironment());
        try {
            fshandlerDriver = new DOPsHDFSHandler();

            File directory = new File("/tmp/output/findprofile");
            //make sure directory exists
            if (!directory.exists()) {
                // Directory does not exist
                if(!directory.mkdirs())
                    throw new IOException("Could NOT create folder: /tmp/output/findprofile");
            }
            else {
                env.delete(directory); // clean out old
                if(!directory.mkdirs())
                    throw new IOException("Could NOT create folder: /tmp/output/findprofile"); // create for this test
            }
            directory = new File("/tmp/input/findprofile");
            if (directory.exists()) { // if this does not work, then manually try this on one node: hadoop fs -rmr /tmp/input/findprofile
                env.delete(directory); // clean out, since this must have been from a previous faulty run of the tests
            }
        } catch (Exception e) {
            bSetupOK=false;
            e.printStackTrace();
        }
        assertEquals(true,bSetupOK);
    }

    /**
     * This test could fail if ulimit for noproc is too low
     * suggest setting following in /etc/security/limits.conf
     *

     *          soft     nproc          1048570
     *          hard     nproc          1048570
     *          soft     nofile         1048570
     *          hard     nofile         1048570
     *          soft     nproc          1048570
     *          hard     nproc          1048570
     *          soft     nofile         1048570
     *          hard     nofile         1048570

     * This is done in the setupUlimit.sh file which is run in the install.sh script
     */
    @Test
    public void testFindProfileFile() {

        boolean bTestOK=true;

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

            Job job;
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
            if(fileResourceUrl!=null) {
                if (fshandlerDriver.copyTo(fileResourceUrl.getPath(), "/" + destFolder + "/355760fb6afaf9c41d17ac5b9397fd45.xml"))
                {
                    if (fileResourceUrl2 != null)
                        fshandlerDriver.copyTo(fileResourceUrl2.getPath(), "/" + destFolder + "/toast" + "/355760fb6afaf9c41d17ac5b9397fd45_toast.xml");
                    else
                        throw new IOException("Resource file NOT present");
                }
                else
                    throw new IOException("could NOT copy file to HDFS - this could be because input folder is existing, created by other user - PLEASE remove it first; do following on one.cluster: vagrant ssh one; hadoop fs -mv /tmp/input /tmp/garbage ");
            }
            else
                throw new IOException("Resource file NOT present");

            // verify that the input file is in hdfs
            List<String> itemsToAdd = new ArrayList<>();
            String filepathname1 = destFolder+"/"+"355760fb6afaf9c41d17ac5b9397fd45.xml";
            String filepathname2 = destFolder+"/toast/"+"355760fb6afaf9c41d17ac5b9397fd45_toast.xml";
            itemsToAdd.add(fshandlerDriver.uri+filepathname1);
            itemsToAdd.add("hdfs://one.cluster:8020/tmp/input/findprofile/toast");
            assertEquals("Expected 2 item in hdfs ls list", itemsToAdd, fshandlerDriver.ls("/tmp/input/findprofile/"));
            itemsToAdd = new ArrayList<>();
            itemsToAdd.add(fshandlerDriver.uri+filepathname2);
            assertEquals("Expected 1 item in hdfs ls list", itemsToAdd, fshandlerDriver.ls("/tmp/input/findprofile/toast"));

            job.waitForCompletion(true);

            // This should result in a result file on LOCAL file system
            // see following example:
            // serup@serup-ThinkPad-T440s:~$ ls /tmp/output/findprofile/result/
            // part-r-00000  _SUCCESS
            // serup@serup-ThinkPad-T440s:~$ cat /tmp/output/findprofile/result/part-r-00000
            // <result>\t\n<file>355760fb6afaf9c41d17ac5b9397fd45.xml</file>\t\n</result>\t\n
            //
            // above shows that the field searched for was found in <file>

            String result = env.executeCmd("cat /tmp/output/findprofile/result/part-r-00000", "/");
            assertEquals(true, contains(result, "<result>\t\n<file>355760fb6afaf9c41d17ac5b9397fd45.xml</file>\t\n</result>\t\n"));

            System.out.println("FindProfileFile - mapreduce job finished - status : SUCCESS");
        } catch (Exception e) {
            e.printStackTrace();
            bTestOK=false;
        }

        assertEquals(true,bTestOK);
    }

}

