package com.zooktutorial.app;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.data.ACL;
import org.junit.Test;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.assertArrayEquals;

/**
 * Created by serup on 11/21/16.
 */
public class TestZkConnection {
    private static ZooKeeper zk;
    private static ZkConnector zkc;
    private static List<String> znodeList = new ArrayList<String>();

    @Test
    public void listZNodes() throws IOException, InterruptedException, KeeperException {
        zkc = new ZkConnector();
        zk = zkc.connect("localhost");
        znodeList = zk.getChildren("/", true);

        System.out.println("list of existing znodes: ");
        for (String znode : znodeList) {
            System.out.print(znode);
            System.out.print(",");
        }
        System.out.println("");
    }

    @Test
    public void createAndDeleteZNode() throws Exception {
        String path = "/sampleznode";
        byte[] data = "sample znode data".getBytes();

        CreateZNode znode = new CreateZNode();
        zkc = new ZkConnector();
        znode.zk = zkc.connect("localhost");
        znode.create(path, data);

        System.out.println("I created sampleznode successfully! ");

        DeleteZNode znode2 = new DeleteZNode();
        znode2.zk = zkc.connect("localhost");
        znode2.delete(path);

        System.out.println("I deleted sampleznode successfully! ");
    }

    @Test
    public void getACL() throws Exception {
        String path = "/sampleznode";
        byte[] data = "sample znode data".getBytes();

        CreateZNode znode = new CreateZNode();
        zkc = new ZkConnector();
        znode.zk = zkc.connect("localhost");
        znode.create(path, data);

        System.out.println("I created sampleznode successfully! ");

        GetACL cnode = new GetACL();
        cnode.zk = znode.zk;
        List<ACL> acl = cnode.getacl(path);

        System.out.println("Access Control List : ");
        for (ACL aclitem: acl) {
            System.out.println(aclitem.toString());
        }

        DeleteZNode znode2 = new DeleteZNode();
        znode2.zk = zkc.connect("localhost");
        znode2.delete(path);

        System.out.println("I deleted sampleznode successfully! ");
    }

    @Test
    public void updateZNode() throws Exception {
        String path = "/sampleznode";
        byte[] data = "updated sample znode data".getBytes();

        CreateZNode znode = new CreateZNode();
        zkc = new ZkConnector();
        znode.zk = zkc.connect("localhost");
        znode.create(path, data);

        System.out.println("I created sampleznode successfully! ");

        UpdateZNode cnode = new UpdateZNode();
        cnode.zk = znode.zk;
        cnode.update(path, data);

        System.out.println("I updated sampleznode successfully! ");

        DeleteZNode znode2 = new DeleteZNode();
        znode2.zk = zkc.connect("localhost");
        znode2.delete(path);

        System.out.println("I deleted sampleznode successfully! ");
    }

    @Test
    public void readZNode() throws Exception {
        String path = "/sampleznode";
        byte[] data = "sample znode data".getBytes();

        CreateZNode znode = new CreateZNode();
        zkc = new ZkConnector();
        znode.zk = zkc.connect("localhost");
        znode.create(path, data);

        System.out.println("I created sampleznode successfully! ");

        ReadZNode rnode = new ReadZNode();
        rnode.zk = znode.zk;
        byte[] rdata = rnode.read(path);

        for (byte b: data) {
            System.out.print((char)b);
        }
        System.out.println();

        assertArrayEquals(data, rdata);

        UpdateZNode cnode = new UpdateZNode();
        cnode.zk = znode.zk;
        byte[] tdata = "updated znode data".getBytes();
        cnode.update(path, tdata);

        System.out.println("I updated sampleznode successfully! ");

        byte[] cdata = rnode.read(path);
        assertArrayEquals(tdata, cdata);

        System.out.println("I read updated sampleznode successfully! ");

        for (byte b: cdata) {
            System.out.print((char)b);
        }
        System.out.println();

        DeleteZNode znode2 = new DeleteZNode();
        znode2.zk = zkc.connect("localhost");
        znode2.delete(path);

        System.out.println("I deleted sampleznode successfully! ");
    }

}
