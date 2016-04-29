package integrationTests.dops.hadoop;

import dops.hadoop.handlers.DOPsHDFSHandler;
import hadoop.mappers.WordMapper;
import hadoop.reducers.WordReducer;
import integrationTests.IntegrationEnvironmentSetup;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

import static junit.framework.TestCase.assertEquals;
import static org.mortbay.util.LazyList.contains;


/**
 * Created by serup on 04-04-16.
 */

// http://stackoverflow.com/questions/9849776/calling-a-mapreduce-job-from-a-simple-java-program
// should find count the word 'Watson' from a created file on the one.cluster node in the DOPs hadoop system
public class WordCountMapReduceTest {


    DOPsHDFSHandler fshandlerDriver;

    IntegrationEnvironmentSetup env = new IntegrationEnvironmentSetup();

    public static void delete(File file) throws IOException{

        if(file.isDirectory()){

            //directory is empty, then delete it
            if(file.list().length==0){

                file.delete();
                System.out.println("Directory is deleted : "
                        + file.getAbsolutePath());

            }else{

                //list all the directory contents
                String files[] = file.list();

                for (String temp : files) {
                    //construct the file structure
                    File fileDelete = new File(file, temp);

                    //recursive delete
                    delete(fileDelete);
                }

                //check the directory again, if empty then delete it
                if(file.list().length==0){
                    file.delete();
                    System.out.println("Directory is deleted : "
                            + file.getAbsolutePath());
                }
            }

        }else{
            //if file, then delete it
            file.delete();
            System.out.println("File is deleted : " + file.getAbsolutePath());
        }
    }

    @Before
    public void setUp() throws Exception {
        Assert.assertEquals(true, env.setupHadoopIntegrationEnvironment());
        fshandlerDriver = new DOPsHDFSHandler();

        File directory = new File("/tmp/output/wordcount");
        //make sure directory exists
        if (!directory.exists()) {
            // Directory does not exist
            directory.mkdir();
        }else {
            try {
                delete(directory); // clean out old
                directory.mkdir(); // create for this test
            } catch (IOException e) {
                e.printStackTrace();
                System.exit(0);
            }
        }
    }

    @Test
    public void testMapReduceWordCount() throws Exception {
        Configuration conf = new Configuration();

        conf.set("fs.defaultFS", "hdfs://one.cluster/");
        conf.set("hadoop.job.ugi", "root, supergroup");
        conf.set("hadoop.home.dir", "/usr/local/hadoop");
        conf.set("dfs.domain.socket.path", "/var/lib/hadoop-hdfs/dn_socket");

        // job.setMapSpeculativeExecution(false)
        conf.setBoolean("mapreduce.map.speculative", false);
        conf.setBoolean("mapreduce.reduce.speculative", false);

        Job job = Job.getInstance(conf);
        job.setJobName("WordCount");

        job.setJarByClass(WordMapper.class);

        job.setMapperClass(WordMapper.class);
        job.setReducerClass(WordReducer.class);

        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);

        FileInputFormat.addInputPath(job, new Path("hdfs://one.cluster:8020/tmp/input/wordcount/*"));
        FileOutputFormat.setOutputPath(job, new Path("/tmp/output/wordcount/result/"));

        // copy the internal resource file watson.txt to remote hadoop hdfs system
        String fileResource= "watson.txt";
        String destFolder="tmp/input/wordcount";
        URL fileResourceUrl = this.getClass().getClassLoader().getResource(fileResource);
        fshandlerDriver.copyTo(fileResourceUrl.getPath(), "/"+destFolder+ "/watson.txt");

        // verify that the input file is in hdfs
        List<String> itemsToAdd = new ArrayList<String>();
        String filepathname = destFolder+"/"+fileResource;
        itemsToAdd.add(fshandlerDriver.uri+filepathname);
        assertEquals("Expected 1 item in hdfs ls list", itemsToAdd, fshandlerDriver.ls("/"+filepathname));

        job.waitForCompletion(true);

        // This should result in a result file on LOCAL file system
        // see following example:
        // serup@serup-ThinkPad-T440s:~$ ls /tmp/output/wordcount/result/
        // part-r-00000  _SUCCESS
        // serup@serup-ThinkPad-T440s:~$ cat /tmp/output/wordcount/result/part-r-00000
        // Watson	81
        // serup@serup-ThinkPad-T440s:~$
        //
        // above shows that the word 'watson' was found 81 times, which is correct according to the watson.txt resource file

        String result = env.executeCmd("cat /tmp/output/wordcount/result/part-r-00000", "/");
        Assert.assertEquals(true, contains(result, new String("Watson\t81\n")));

    }

    {
// example SUCCESS run:
    /*
    Waiting for VM hadoop one.cluster to start ... - perhaps you should start a minimum of three yourself, and make sure ambari-server and ambari-agent on all nodes is running - please wait it takes a long time...
NB! DO NOT use vagrant halt, when shutting down a cluster - use vagrant suspend and vagrant resume - otherwise you will have to use ambari-server one.cluster:8080 admin admin to reestablish connection between all nodes in cluster - its takes long time
VM hadoop cluster is started - now run test
File is deleted : /tmp/output/wordcount/result/.part-r-00000.crc
File is deleted : /tmp/output/wordcount/result/part-r-00000
File is deleted : /tmp/output/wordcount/result/_SUCCESS
File is deleted : /tmp/output/wordcount/result/._SUCCESS.crc
Directory is deleted : /tmp/output/wordcount/result
Directory is deleted : /tmp/output/wordcount
11:24:09.493 [main] DEBUG org.apache.hadoop.security.Groups -  Creating new Groups object
11:24:09.508 [main] DEBUG org.apache.hadoop.security.Groups - Group mapping impl=org.apache.hadoop.security.ShellBasedUnixGroupsMapping; cacheTimeout=300000
11:24:09.578 [main] DEBUG o.a.h.security.UserGroupInformation - hadoop login
11:24:09.578 [main] DEBUG o.a.h.security.UserGroupInformation - hadoop login commit
11:24:09.582 [main] DEBUG o.a.h.security.UserGroupInformation - using local user:UnixPrincipal: serup
11:24:09.585 [main] DEBUG o.a.h.security.UserGroupInformation - UGI loginUser:serup
11:24:09.598 [main] DEBUG org.apache.hadoop.fs.FileSystem - Creating filesystem for hdfs://one.cluster:8020/tmp/input/wordcount/*
11:24:09.691 [main] DEBUG o.apache.hadoop.io.retry.RetryUtils - multipleLinearRandomRetry = null
11:24:09.715 [main] DEBUG org.apache.hadoop.ipc.Client - The ping interval is60000ms.
11:24:09.717 [main] DEBUG org.apache.hadoop.ipc.Client - Use SIMPLE authentication for protocol ClientProtocol
11:24:09.718 [main] DEBUG org.apache.hadoop.ipc.Client - Connecting to one.cluster/192.168.0.101:8020
11:24:09.744 [main] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup sending #0
11:24:09.747 [IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup: starting, having connections 1
11:24:09.753 [IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup got value #0
11:24:09.755 [main] DEBUG org.apache.hadoop.ipc.RPC - Call: getProtocolVersion 61
11:24:09.755 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - Short circuit read is false
11:24:09.755 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - Connect to datanode via hostname is false
11:24:09.784 [main] DEBUG org.apache.hadoop.fs.FileSystem - Creating filesystem for file:///
11:24:09.792 [main] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup sending #1
11:24:09.805 [IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup got value #1
11:24:09.810 [main] DEBUG org.apache.hadoop.ipc.RPC - Call: getFileInfo 18
11:24:09.810 [main] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup sending #2
11:24:09.825 [IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup got value #2
11:24:09.827 [main] DEBUG org.apache.hadoop.ipc.RPC - Call: getFileInfo 17
11:24:09.830 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - /tmp/input/wordcount/watson.txt: masked=rwxr-xr-x
11:24:09.835 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - computePacketChunkSize: src=/tmp/input/wordcount/watson.txt, chunkSize=516, chunksPerPacket=127, packetSize=65557
11:24:09.835 [main] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup sending #3
11:24:09.848 [IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup got value #3
11:24:09.849 [main] DEBUG org.apache.hadoop.ipc.RPC - Call: create 14
11:24:09.853 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk allocating new packet seqno=0, src=/tmp/input/wordcount/watson.txt, packetSize=65557, chunksPerPacket=127, bytesCurBlock=0
11:24:09.854 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk packet full seqno=0, src=/tmp/input/wordcount/watson.txt, bytesCurBlock=65024, blockSize=67108864, appendChunk=false
11:24:09.855 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - computePacketChunkSize: src=/tmp/input/wordcount/watson.txt, chunkSize=516, chunksPerPacket=127, packetSize=65557
11:24:09.855 [Thread-2] DEBUG org.apache.hadoop.hdfs.DFSClient - Allocating new block
11:24:09.856 [Thread-2] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup sending #4
11:24:09.858 [IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup got value #4
11:24:09.865 [Thread-2] DEBUG org.apache.hadoop.ipc.RPC - Call: addBlock 10
11:24:09.865 [Thread-2] DEBUG org.apache.hadoop.hdfs.DFSClient - pipeline = 192.168.0.102:50010
11:24:09.865 [Thread-2] DEBUG org.apache.hadoop.hdfs.DFSClient - pipeline = 192.168.0.101:50010
11:24:09.865 [Thread-2] DEBUG org.apache.hadoop.hdfs.DFSClient - Connecting to 192.168.0.102:50010
11:24:09.867 [Thread-2] DEBUG org.apache.hadoop.hdfs.DFSClient - Send buf size 131072
11:24:09.889 [DataStreamer for file /tmp/input/wordcount/watson.txt block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DataStreamer block blk_5594538489331883481_1262 wrote packet seqno:0 size:65557 offsetInBlock:0 lastPacketInBlock:false
11:24:09.892 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk allocating new packet seqno=1, src=/tmp/input/wordcount/watson.txt, packetSize=65557, chunksPerPacket=127, bytesCurBlock=65024
11:24:09.892 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk packet full seqno=1, src=/tmp/input/wordcount/watson.txt, bytesCurBlock=130048, blockSize=67108864, appendChunk=false
11:24:09.892 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - computePacketChunkSize: src=/tmp/input/wordcount/watson.txt, chunkSize=516, chunksPerPacket=127, packetSize=65557
11:24:09.892 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk allocating new packet seqno=2, src=/tmp/input/wordcount/watson.txt, packetSize=65557, chunksPerPacket=127, bytesCurBlock=130048
11:24:09.893 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk packet full seqno=2, src=/tmp/input/wordcount/watson.txt, bytesCurBlock=195072, blockSize=67108864, appendChunk=false
11:24:09.893 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - computePacketChunkSize: src=/tmp/input/wordcount/watson.txt, chunkSize=516, chunksPerPacket=127, packetSize=65557
11:24:09.893 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk allocating new packet seqno=3, src=/tmp/input/wordcount/watson.txt, packetSize=65557, chunksPerPacket=127, bytesCurBlock=195072
11:24:09.894 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk packet full seqno=3, src=/tmp/input/wordcount/watson.txt, bytesCurBlock=260096, blockSize=67108864, appendChunk=false
11:24:09.894 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - computePacketChunkSize: src=/tmp/input/wordcount/watson.txt, chunkSize=516, chunksPerPacket=127, packetSize=65557
11:24:09.894 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk allocating new packet seqno=4, src=/tmp/input/wordcount/watson.txt, packetSize=65557, chunksPerPacket=127, bytesCurBlock=260096
11:24:09.894 [ResponseProcessor for block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient for block blk_5594538489331883481_1262 Replies for seqno 0 are SUCCESS SUCCESS
11:24:09.895 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk packet full seqno=4, src=/tmp/input/wordcount/watson.txt, bytesCurBlock=325120, blockSize=67108864, appendChunk=false
11:24:09.895 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - computePacketChunkSize: src=/tmp/input/wordcount/watson.txt, chunkSize=516, chunksPerPacket=127, packetSize=65557
11:24:09.895 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk allocating new packet seqno=5, src=/tmp/input/wordcount/watson.txt, packetSize=65557, chunksPerPacket=127, bytesCurBlock=325120
11:24:09.896 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk packet full seqno=5, src=/tmp/input/wordcount/watson.txt, bytesCurBlock=390144, blockSize=67108864, appendChunk=false
11:24:09.896 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - computePacketChunkSize: src=/tmp/input/wordcount/watson.txt, chunkSize=516, chunksPerPacket=127, packetSize=65557
11:24:09.896 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk allocating new packet seqno=6, src=/tmp/input/wordcount/watson.txt, packetSize=65557, chunksPerPacket=127, bytesCurBlock=390144
11:24:09.896 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk packet full seqno=6, src=/tmp/input/wordcount/watson.txt, bytesCurBlock=455168, blockSize=67108864, appendChunk=false
11:24:09.896 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - computePacketChunkSize: src=/tmp/input/wordcount/watson.txt, chunkSize=516, chunksPerPacket=127, packetSize=65557
11:24:09.896 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk allocating new packet seqno=7, src=/tmp/input/wordcount/watson.txt, packetSize=65557, chunksPerPacket=127, bytesCurBlock=455168
11:24:09.897 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk packet full seqno=7, src=/tmp/input/wordcount/watson.txt, bytesCurBlock=520192, blockSize=67108864, appendChunk=false
11:24:09.897 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - computePacketChunkSize: src=/tmp/input/wordcount/watson.txt, chunkSize=516, chunksPerPacket=127, packetSize=65557
11:24:09.897 [main] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient writeChunk allocating new packet seqno=8, src=/tmp/input/wordcount/watson.txt, packetSize=65557, chunksPerPacket=127, bytesCurBlock=520192
11:24:09.902 [DataStreamer for file /tmp/input/wordcount/watson.txt block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DataStreamer block blk_5594538489331883481_1262 wrote packet seqno:1 size:65557 offsetInBlock:65024 lastPacketInBlock:false
11:24:09.908 [DataStreamer for file /tmp/input/wordcount/watson.txt block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DataStreamer block blk_5594538489331883481_1262 wrote packet seqno:2 size:65557 offsetInBlock:130048 lastPacketInBlock:false
11:24:09.908 [ResponseProcessor for block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient for block blk_5594538489331883481_1262 Replies for seqno 1 are SUCCESS SUCCESS
11:24:09.909 [DataStreamer for file /tmp/input/wordcount/watson.txt block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DataStreamer block blk_5594538489331883481_1262 wrote packet seqno:3 size:65557 offsetInBlock:195072 lastPacketInBlock:false
11:24:09.912 [DataStreamer for file /tmp/input/wordcount/watson.txt block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DataStreamer block blk_5594538489331883481_1262 wrote packet seqno:4 size:65557 offsetInBlock:260096 lastPacketInBlock:false
11:24:09.915 [DataStreamer for file /tmp/input/wordcount/watson.txt block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DataStreamer block blk_5594538489331883481_1262 wrote packet seqno:5 size:65557 offsetInBlock:325120 lastPacketInBlock:false
11:24:09.917 [DataStreamer for file /tmp/input/wordcount/watson.txt block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DataStreamer block blk_5594538489331883481_1262 wrote packet seqno:6 size:65557 offsetInBlock:390144 lastPacketInBlock:false
11:24:09.917 [DataStreamer for file /tmp/input/wordcount/watson.txt block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DataStreamer block blk_5594538489331883481_1262 wrote packet seqno:7 size:65557 offsetInBlock:455168 lastPacketInBlock:false
11:24:09.919 [DataStreamer for file /tmp/input/wordcount/watson.txt block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DataStreamer block blk_5594538489331883481_1262 wrote packet seqno:8 size:62193 offsetInBlock:520192 lastPacketInBlock:true
11:24:09.920 [ResponseProcessor for block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient for block blk_5594538489331883481_1262 Replies for seqno 2 are SUCCESS SUCCESS
11:24:09.920 [ResponseProcessor for block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient for block blk_5594538489331883481_1262 Replies for seqno 3 are SUCCESS SUCCESS
11:24:09.923 [ResponseProcessor for block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient for block blk_5594538489331883481_1262 Replies for seqno 4 are SUCCESS SUCCESS
11:24:09.925 [ResponseProcessor for block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient for block blk_5594538489331883481_1262 Replies for seqno 5 are SUCCESS SUCCESS
11:24:09.926 [ResponseProcessor for block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient for block blk_5594538489331883481_1262 Replies for seqno 6 are SUCCESS SUCCESS
11:24:09.928 [ResponseProcessor for block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient for block blk_5594538489331883481_1262 Replies for seqno 7 are SUCCESS SUCCESS
11:24:09.930 [ResponseProcessor for block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - DFSClient for block blk_5594538489331883481_1262 Replies for seqno 8 are SUCCESS SUCCESS
11:24:09.931 [DataStreamer for file /tmp/input/wordcount/watson.txt block blk_5594538489331883481_1262] DEBUG org.apache.hadoop.hdfs.DFSClient - Closing old block blk_5594538489331883481_1262
11:24:09.931 [main] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup sending #5
11:24:09.934 [IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup got value #5
11:24:09.940 [main] DEBUG org.apache.hadoop.ipc.RPC - Call: complete 9
11:24:09.941 [main] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup sending #6
11:24:09.949 [IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup got value #6
11:24:09.956 [main] DEBUG org.apache.hadoop.ipc.RPC - Call: getListing 15
11:24:09.959 [main] DEBUG o.a.h.security.UserGroupInformation - PriviledgedAction as:serup from:org.apache.hadoop.mapreduce.Job.connect(Job.java:561)
11:24:09.986 [main] DEBUG o.a.h.security.UserGroupInformation - PriviledgedAction as:serup from:org.apache.hadoop.mapred.JobClient.submitJobInternal(JobClient.java:936)
11:24:09.989 [main] DEBUG o.a.hadoop.util.NativeCodeLoader - Trying to load the custom-built native-hadoop library...
11:24:09.990 [main] DEBUG o.a.hadoop.util.NativeCodeLoader - Failed to load native-hadoop with error: java.lang.UnsatisfiedLinkError: no hadoop in java.library.path
11:24:09.990 [main] DEBUG o.a.hadoop.util.NativeCodeLoader - java.library.path=/opt/idea/bin::/usr/java/packages/lib/amd64:/usr/lib64:/lib64:/lib:/usr/lib
11:24:09.990 [main] WARN  o.a.hadoop.util.NativeCodeLoader - Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
11:24:09.991 [main] DEBUG org.apache.hadoop.mapred.JobClient - adding the following namenodes' delegation tokens:null
11:24:09.992 [main] WARN  org.apache.hadoop.mapred.JobClient - Use GenericOptionsParser for parsing the arguments. Applications should implement Tool for the same.
11:24:09.992 [main] DEBUG org.apache.hadoop.mapred.JobClient - default FileSystem: file:///
11:24:09.996 [main] WARN  org.apache.hadoop.mapred.JobClient - No job jar file set.  User classes may not be found. See JobConf(Class) or JobConf#setJar(String).
11:24:10.000 [main] DEBUG org.apache.hadoop.mapred.JobClient - Creating splits at file:/tmp/hadoop-serup/mapred/staging/serup1019743195/.staging/job_local1019743195_0001
11:24:10.009 [main] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup sending #7
11:24:10.018 [IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup got value #7
11:24:10.029 [main] DEBUG org.apache.hadoop.ipc.RPC - Call: getListing 20
11:24:10.029 [main] INFO  o.a.h.m.lib.input.FileInputFormat - Total input paths to process : 1
11:24:10.030 [main] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup sending #8
11:24:10.038 [IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup got value #8
11:24:10.044 [main] DEBUG org.apache.hadoop.ipc.RPC - Call: getBlockLocations 15
11:24:10.066 [main] WARN  o.a.h.io.compress.snappy.LoadSnappy - Snappy native library not loaded
11:24:10.069 [main] DEBUG o.a.h.m.lib.input.FileInputFormat - Total # of splits: 1
11:24:10.148 [main] DEBUG org.apache.hadoop.mapred.JobClient - Printing tokens for job: job_local1019743195_0001
11:24:10.167 [main] DEBUG org.apache.hadoop.mapred.TaskRunner - Fully deleting contents of /tmp/hadoop-serup/mapred/local/localRunner
11:24:10.200 [main] INFO  org.apache.hadoop.mapred.JobClient - Running job: job_local1019743195_0001
11:24:10.240 [Thread-5] DEBUG o.a.hadoop.mapred.LocalJobRunner - Starting thread pool executor.
11:24:10.240 [Thread-5] DEBUG o.a.hadoop.mapred.LocalJobRunner - Max local threads: 1
11:24:10.240 [Thread-5] DEBUG o.a.hadoop.mapred.LocalJobRunner - Map tasks to process: 1
11:24:10.243 [Thread-5] INFO  o.a.hadoop.mapred.LocalJobRunner - Waiting for map tasks
11:24:10.247 [pool-1-thread-1] INFO  o.a.hadoop.mapred.LocalJobRunner - Starting task: attempt_local1019743195_0001_m_000000_0
11:24:10.263 [pool-1-thread-1] DEBUG o.apache.hadoop.mapred.SortedRanges - currentIndex 0   0:0
11:24:10.273 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:10.273 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding SPILLED_RECORDS
11:24:10.282 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.TaskRunner - mapred.local.dir for child : /tmp/hadoop-serup/mapred/local/taskTracker/serup/jobcache/job_local1019743195_0001/attempt_local1019743195_0001_m_000000_0
11:24:10.284 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Task - using new api for output committer
11:24:10.296 [pool-1-thread-1] INFO  org.apache.hadoop.util.ProcessTree - setsid exited with exit code 0
11:24:10.305 [pool-1-thread-1] INFO  org.apache.hadoop.mapred.Task -  Using ResourceCalculatorPlugin : org.apache.hadoop.util.LinuxResourceCalculatorPlugin@7e59b06e
11:24:10.312 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding SPLIT_RAW_BYTES
11:24:10.313 [pool-1-thread-1] INFO  org.apache.hadoop.mapred.MapTask - Processing split: hdfs://one.cluster:8020/tmp/input/wordcount/watson.txt:0+581876
11:24:10.314 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding MAP_INPUT_RECORDS
11:24:10.320 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.input.FileInputFormat$Counter with bundle
11:24:10.320 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding BYTES_READ
11:24:10.331 [pool-1-thread-1] INFO  org.apache.hadoop.mapred.MapTask - io.sort.mb = 100
11:24:10.393 [pool-1-thread-1] INFO  org.apache.hadoop.mapred.MapTask - data buffer = 79691776/99614720
11:24:10.393 [pool-1-thread-1] INFO  org.apache.hadoop.mapred.MapTask - record buffer = 262144/327680
11:24:10.394 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding MAP_OUTPUT_BYTES
11:24:10.394 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding MAP_OUTPUT_RECORDS
11:24:10.394 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding COMBINE_INPUT_RECORDS
11:24:10.394 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding COMBINE_OUTPUT_RECORDS
11:24:10.394 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding MAP_OUTPUT_MATERIALIZED_BYTES
11:24:10.401 [pool-1-thread-1] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup sending #9
11:24:10.412 [IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup got value #9
11:24:10.419 [pool-1-thread-1] DEBUG org.apache.hadoop.ipc.RPC - Call: getBlockLocations 17
11:24:10.421 [pool-1-thread-1] DEBUG org.apache.hadoop.hdfs.DFSClient - Connecting to /192.168.0.102:50010
11:24:10.427 [pool-1-thread-1] DEBUG org.apache.hadoop.fs.FSInputChecker - DFSClient readChunk got seqno 0 offsetInBlock 0 lastPacketInBlock false packetLen 132100
11:24:10.579 [pool-1-thread-1] DEBUG org.apache.hadoop.fs.FSInputChecker - DFSClient readChunk got seqno 1 offsetInBlock 131072 lastPacketInBlock false packetLen 132100
11:24:10.691 [pool-1-thread-1] DEBUG org.apache.hadoop.fs.FSInputChecker - DFSClient readChunk got seqno 2 offsetInBlock 262144 lastPacketInBlock false packetLen 132100
11:24:10.741 [pool-1-thread-1] DEBUG org.apache.hadoop.fs.FSInputChecker - DFSClient readChunk got seqno 3 offsetInBlock 393216 lastPacketInBlock false packetLen 132100
11:24:10.784 [pool-1-thread-1] DEBUG org.apache.hadoop.fs.FSInputChecker - DFSClient readChunk got seqno 4 offsetInBlock 524288 lastPacketInBlock false packetLen 57796
11:24:10.800 [pool-1-thread-1] DEBUG org.apache.hadoop.fs.FSInputChecker - DFSClient readChunk got seqno 5 offsetInBlock 581632 lastPacketInBlock true packetLen 252
11:24:10.802 [pool-1-thread-1] INFO  org.apache.hadoop.mapred.MapTask - Starting flush of map output
11:24:10.968 [pool-1-thread-1] INFO  org.apache.hadoop.mapred.MapTask - Finished spill 0
11:24:10.969 [pool-1-thread-1] INFO  org.apache.hadoop.mapred.Task - Task:attempt_local1019743195_0001_m_000000_0 is done. And is in the process of commiting
11:24:10.971 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:10.971 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding HDFS_BYTES_READ
11:24:10.971 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding HDFS_BYTES_WRITTEN
11:24:10.972 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding FILE_BYTES_READ
11:24:10.972 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding FILE_BYTES_WRITTEN
11:24:10.972 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding COMMITTED_HEAP_BYTES
11:24:10.972 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding CPU_MILLISECONDS
11:24:10.972 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding PHYSICAL_MEMORY_BYTES
11:24:10.972 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Adding VIRTUAL_MEMORY_BYTES
11:24:10.984 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:10.984 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.input.FileInputFormat$Counter with bundle
11:24:10.984 [pool-1-thread-1] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:10.984 [pool-1-thread-1] INFO  o.a.hadoop.mapred.LocalJobRunner -
11:24:10.984 [pool-1-thread-1] INFO  org.apache.hadoop.mapred.Task - Task 'attempt_local1019743195_0001_m_000000_0' done.
11:24:10.985 [pool-1-thread-1] INFO  o.a.hadoop.mapred.LocalJobRunner - Finishing task: attempt_local1019743195_0001_m_000000_0
11:24:10.985 [Thread-5] INFO  o.a.hadoop.mapred.LocalJobRunner - Map task executor complete.
11:24:10.990 [Thread-5] DEBUG o.apache.hadoop.mapred.SortedRanges - currentIndex 0   0:0
11:24:10.991 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:10.991 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Adding SPILLED_RECORDS
11:24:10.991 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Adding REDUCE_SHUFFLE_BYTES
11:24:10.991 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Adding REDUCE_INPUT_GROUPS
11:24:10.991 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Adding REDUCE_INPUT_RECORDS
11:24:10.991 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Adding REDUCE_OUTPUT_RECORDS
11:24:10.991 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Adding COMBINE_OUTPUT_RECORDS
11:24:10.992 [Thread-5] DEBUG org.apache.hadoop.mapred.TaskRunner - mapred.local.dir for child : /tmp/hadoop-serup/mapred/local/taskTracker/serup/jobcache/job_local1019743195_0001/attempt_local1019743195_0001_r_000000_0
11:24:11.003 [Thread-5] DEBUG org.apache.hadoop.mapred.Task - using new api for output committer
11:24:11.004 [Thread-5] INFO  org.apache.hadoop.mapred.Task -  Using ResourceCalculatorPlugin : org.apache.hadoop.util.LinuxResourceCalculatorPlugin@ce8d0bc
11:24:11.014 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:11.015 [Thread-5] INFO  o.a.hadoop.mapred.LocalJobRunner -
11:24:11.019 [Thread-5] INFO  org.apache.hadoop.mapred.Merger - Merging 1 sorted segments
11:24:11.022 [Thread-5] INFO  org.apache.hadoop.mapred.Merger - Down to the last merge-pass, with 1 segments left of total size: 1200402 bytes
11:24:11.031 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:11.032 [Thread-5] INFO  o.a.hadoop.mapred.LocalJobRunner -
11:24:11.035 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:11.035 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Adding BYTES_WRITTEN
11:24:11.205 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 0%
11:24:17.019 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:17.019 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Adding HDFS_BYTES_READ
11:24:17.019 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Adding HDFS_BYTES_WRITTEN
11:24:17.019 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Adding FILE_BYTES_READ
11:24:17.019 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Adding FILE_BYTES_WRITTEN
11:24:17.019 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Adding COMMITTED_HEAP_BYTES
11:24:17.022 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Adding CPU_MILLISECONDS
11:24:17.026 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Adding PHYSICAL_MEMORY_BYTES
11:24:17.029 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Adding VIRTUAL_MEMORY_BYTES
11:24:17.067 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:17.067 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:17.067 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:17.073 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:24:17.237 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 68%
11:24:20.083 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:20.084 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:20.084 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:20.085 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:24:20.250 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 69%
11:24:20.404 [IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup: closed
11:24:20.404 [IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup] DEBUG org.apache.hadoop.ipc.Client - IPC Client (47) connection to one.cluster/192.168.0.101:8020 from serup: stopped, remaining connections 0
11:24:23.103 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:23.111 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:23.113 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:23.117 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:24:23.259 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 70%
11:24:26.130 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:26.130 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:26.130 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:26.131 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:24:26.270 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 71%
11:24:29.157 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:29.171 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:29.171 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:29.173 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:24:29.275 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 72%
11:24:32.195 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:32.196 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:32.196 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:32.199 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:24:32.284 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 73%
11:24:35.213 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:35.219 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:35.222 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:35.226 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:24:35.299 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 74%
11:24:38.243 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:38.247 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:38.250 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:38.253 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:24:38.311 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 75%
11:24:41.266 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:41.271 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:41.275 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:41.279 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:24:41.313 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 77%
11:24:44.293 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:44.298 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:44.303 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:44.305 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:24:44.328 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 78%
11:24:47.312 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:47.314 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:47.314 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:47.316 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:24:47.339 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 79%
11:24:50.331 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:50.337 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:50.340 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:50.343 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:24:50.350 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 80%
11:24:53.362 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:53.367 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:53.371 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:53.374 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:24:54.359 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 81%
11:24:56.392 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:56.393 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:56.393 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:56.396 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:24:57.361 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 82%
11:24:59.406 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:24:59.408 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:24:59.408 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:24:59.409 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:00.374 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 84%
11:25:02.433 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:02.433 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:02.434 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:02.440 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:03.380 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 85%
11:25:05.468 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:05.471 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:05.474 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:05.477 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:06.383 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 86%
11:25:08.500 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:08.504 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:08.507 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:08.510 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:09.394 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 87%
11:25:11.528 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:11.528 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:11.529 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:11.530 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:12.420 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 88%
11:25:14.551 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:14.555 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:14.557 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:14.560 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:15.423 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 89%
11:25:17.578 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:17.578 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:17.580 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:17.583 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:18.436 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 91%
11:25:20.600 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:20.601 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:20.601 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:20.602 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:21.442 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 92%
11:25:23.615 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:23.620 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:23.624 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:23.627 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:24.450 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 93%
11:25:26.639 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:26.640 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:26.640 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:26.643 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:27.470 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 94%
11:25:29.671 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:29.679 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:29.683 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:29.686 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:30.486 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 95%
11:25:32.710 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:32.714 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:32.716 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:32.718 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:33.497 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 96%
11:25:35.732 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:35.736 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:35.737 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:35.738 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:36.505 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 97%
11:25:38.752 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:38.754 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:38.754 [communication thread] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:38.761 [communication thread] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:39.515 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 98%
11:25:41.554 [Thread-5] INFO  org.apache.hadoop.mapred.Task - Task:attempt_local1019743195_0001_r_000000_0 is done. And is in the process of commiting
11:25:41.559 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:41.559 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:41.560 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:41.561 [Thread-5] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:41.561 [Thread-5] INFO  org.apache.hadoop.mapred.Task - Task attempt_local1019743195_0001_r_000000_0 is allowed to commit now
11:25:41.562 [Thread-5] DEBUG o.a.h.m.l.output.FileOutputCommitter - Moved file:/tmp/output/wordcount/result/_temporary/_attempt_local1019743195_0001_r_000000_0/part-r-00000 to /tmp/output/wordcount/result/part-r-00000
11:25:41.562 [Thread-5] INFO  o.a.h.m.l.output.FileOutputCommitter - Saved output of task 'attempt_local1019743195_0001_r_000000_0' to /tmp/output/wordcount/result
11:25:41.567 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:41.567 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:41.567 [Thread-5] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:41.568 [Thread-5] INFO  o.a.hadoop.mapred.LocalJobRunner - reduce > reduce
11:25:41.568 [Thread-5] INFO  org.apache.hadoop.mapred.Task - Task 'attempt_local1019743195_0001_r_000000_0' done.
11:25:42.521 [main] INFO  org.apache.hadoop.mapred.JobClient -  map 100% reduce 100%
11:25:42.521 [main] INFO  org.apache.hadoop.mapred.JobClient - Job complete: job_local1019743195_0001
11:25:42.521 [main] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:42.521 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding COMBINE_OUTPUT_RECORDS
11:25:42.521 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding SPILLED_RECORDS
11:25:42.521 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding MAP_OUTPUT_MATERIALIZED_BYTES
11:25:42.521 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding VIRTUAL_MEMORY_BYTES
11:25:42.521 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding MAP_INPUT_RECORDS
11:25:42.521 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding SPLIT_RAW_BYTES
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding MAP_OUTPUT_RECORDS
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding MAP_OUTPUT_BYTES
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding PHYSICAL_MEMORY_BYTES
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding COMBINE_INPUT_RECORDS
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding CPU_MILLISECONDS
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding COMMITTED_HEAP_BYTES
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.input.FileInputFormat$Counter with bundle
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding BYTES_READ
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding HDFS_BYTES_READ
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding FILE_BYTES_WRITTEN
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding FILE_BYTES_READ
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding HDFS_BYTES_WRITTEN
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapred.Task$Counter with bundle
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding COMBINE_OUTPUT_RECORDS
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding SPILLED_RECORDS
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding MAP_OUTPUT_MATERIALIZED_BYTES
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding VIRTUAL_MEMORY_BYTES
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding MAP_INPUT_RECORDS
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding SPLIT_RAW_BYTES
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding MAP_OUTPUT_RECORDS
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding MAP_OUTPUT_BYTES
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding PHYSICAL_MEMORY_BYTES
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding COMBINE_INPUT_RECORDS
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding CPU_MILLISECONDS
11:25:42.522 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding COMMITTED_HEAP_BYTES
11:25:42.523 [main] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.input.FileInputFormat$Counter with bundle
11:25:42.523 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding BYTES_READ
11:25:42.523 [main] DEBUG org.apache.hadoop.mapred.Counters - Creating group FileSystemCounters with nothing
11:25:42.523 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding HDFS_BYTES_READ
11:25:42.523 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding FILE_BYTES_WRITTEN
11:25:42.523 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding FILE_BYTES_READ
11:25:42.523 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding HDFS_BYTES_WRITTEN
11:25:42.523 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding REDUCE_INPUT_RECORDS
11:25:42.523 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding REDUCE_OUTPUT_RECORDS
11:25:42.523 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding REDUCE_SHUFFLE_BYTES
11:25:42.523 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding REDUCE_INPUT_GROUPS
11:25:42.523 [main] DEBUG org.apache.hadoop.mapred.Counters - Creating group org.apache.hadoop.mapreduce.lib.output.FileOutputFormat$Counter with bundle
11:25:42.523 [main] DEBUG org.apache.hadoop.mapred.Counters - Adding BYTES_WRITTEN
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient - Counters: 22
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -   Map-Reduce Framework
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     Spilled Records=215066
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     Map output materialized bytes=1200406
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     Reduce input records=107533
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     Virtual memory (bytes) snapshot=0
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     Map input records=13052
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     SPLIT_RAW_BYTES=119
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     Map output bytes=985334
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     Reduce shuffle bytes=0
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     Physical memory (bytes) snapshot=0
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     Reduce input groups=9442
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     Combine output records=0
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     Reduce output records=1
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     Map output records=107533
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     Combine input records=0
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     CPU time spent (ms)=0
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -     Total committed heap usage (bytes)=514850816
11:25:42.523 [main] INFO  org.apache.hadoop.mapred.JobClient -   File Input Format Counters
11:25:42.524 [main] INFO  org.apache.hadoop.mapred.JobClient -     Bytes Read=581876
11:25:42.524 [main] INFO  org.apache.hadoop.mapred.JobClient -   FileSystemCounters
11:25:42.524 [main] INFO  org.apache.hadoop.mapred.JobClient -     HDFS_BYTES_READ=1163752
11:25:42.524 [main] INFO  org.apache.hadoop.mapred.JobClient -     FILE_BYTES_WRITTEN=2538688
11:25:42.524 [main] INFO  org.apache.hadoop.mapred.JobClient -     FILE_BYTES_READ=2364538
11:25:42.524 [main] INFO  org.apache.hadoop.mapred.JobClient -     HDFS_BYTES_WRITTEN=1163752
11:25:42.524 [main] INFO  org.apache.hadoop.mapred.JobClient -   File Output Format Counters
11:25:42.524 [main] INFO  org.apache.hadoop.mapred.JobClient -     Bytes Written=22
     */
    }
}
