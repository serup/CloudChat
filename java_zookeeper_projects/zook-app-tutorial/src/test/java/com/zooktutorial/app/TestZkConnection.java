package com.zooktutorial.app;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.data.ACL;
import org.apache.zookeeper.data.Id;
import org.junit.Test;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import static com.zooktutorial.app.SetACL.setacl;
import static com.zooktutorial.app.ZookeeperSecurityUtil.generateDigest;
import static com.zooktutorial.app.ZookeeperSecurityUtil.getPermFromString;
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

    @Test
    public void setACL() throws Exception {
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

        /*
        System.out.println("add authentication to znode - meaning lock it ");
        ACL newAcl = new ACL();
        newAcl.setId(new Id("digest", generateDigest("datanotfound" + ":" + "channel123") ));
        newAcl.setPerms(getPermFromString("car"));
        System.out.println("New Access Control List : ");
        System.out.println(newAcl.toString());
        List<ACL> lAcl = new ArrayList<ACL>();
        lAcl.add(newAcl);
        SetACL aclnode = new SetACL();
        aclnode.zk = znode.zk;
        aclnode.setacl(path, lAcl );
        */
        SetACL aclnode = new SetACL();
        aclnode.zk = znode.zk;
        aclnode.setacl(path, "datanotfound", "channel123" );

        System.out.println("Cleanup : ");
        DeleteZNode znode2 = new DeleteZNode();
        znode2.zk = zkc.connect("localhost");
        znode2.delete(path);
        System.out.println("I deleted sampleznode successfully! ");
    }

    @Test
    public void getDataWithAuthentication() throws Exception {
        String path = "/sampleznode";
        byte[] data = "sample znode data".getBytes();

        CreateZNode znode = new CreateZNode();
        zkc = new ZkConnector();
        znode.zk = zkc.connect("localhost");

        String user = "datanotfound";
        String pwd  = "channel123";

        System.out.println("create a znode ");
        znode.create(path, data);

       /*
        System.out.println("add authentication to znode - meaning lock it ");
        ACL newAcl = new ACL();
        newAcl.setId(new Id("digest", generateDigest("datanotfound" + ":" + "channel123") ));
        newAcl.setPerms(getPermFromString("car"));
        System.out.println(newAcl.toString());
        List<ACL> lAcl = new ArrayList<ACL>();
        lAcl.add(newAcl);
        SetACL aclnode = new SetACL();
        aclnode.zk = znode.zk;
        aclnode.setacl(path, lAcl );
        */

        SetACL aclnode = new SetACL();
        aclnode.zk = znode.zk;
        aclnode.setacl(path, "datanotfound", "channel123" );

        System.out.println("I created sampleznode with digest authentication successfully! ");
        System.out.println("- getData from znode without authentication : ");
        System.out.println("- getData result :  ");
        try {
            CreateZNode woauthZNode = new CreateZNode();
            ZkConnector woauthZKC = new ZkConnector();
            woauthZNode.zk = woauthZKC.connect("localhost");
            byte[] wdata = woauthZNode.zk.getData(path, true, woauthZNode.zk.exists(path, true));
            for (byte b : wdata) {
                System.out.print((char) b);
            }
            System.out.println();
        } catch (Exception e) { System.out.println(e.getMessage()); }


        System.out.println("- getData with authentication : ");
        System.out.println("add authentication info to zookeeper object ");
        znode.zk.addAuthInfo("digest", (user + ":" + pwd).getBytes());
        byte[] rdata = znode.zk.getData(path, true, znode.zk.exists(path,true));
        System.out.println("- getData result :  ");
        for (byte b: rdata ) {
           System.out.print((char)b);
        }
        System.out.println();

        System.out.println("Cleanup : ");
        DeleteZNode znode2 = new DeleteZNode();
        znode2.zk = zkc.connect("localhost");
        znode2.delete(path);

        System.out.println("I deleted sampleznode successfully! ");
    }
}
