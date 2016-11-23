package com.zooktutorial.app;

import org.apache.zookeeper.CreateMode;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooDefs;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.data.ACL;
import org.apache.zookeeper.data.Id;

import java.io.IOException;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.List;

import static com.zooktutorial.app.ZookeeperSecurityUtil.generateDigest;
import static com.zooktutorial.app.ZookeeperSecurityUtil.getPermFromString;


/**
 * Created by serup on 11/22/16.
 */
public class SetACL {
    public static ZooKeeper zk;
    private static ZkConnector zkc;

    public static boolean setacl(String path, String username, String password) throws NoSuchAlgorithmException, KeeperException, InterruptedException {
        boolean bResult=false;

        try {
            System.out.println("add authentication to znode - meaning lock it ");
            ACL newAcl = new ACL();
            newAcl.setId(new Id("digest", generateDigest(username + ":" + password) ));
            newAcl.setPerms(getPermFromString("car"));
            System.out.println("New Access Control List : ");
            System.out.println(newAcl.toString());
            List<ACL> lAcl = new ArrayList<ACL>();
            lAcl.add(newAcl);
            setacl(path, lAcl );
            bResult=true;
        } catch (Exception e) { System.out.println(e.getMessage()); }

        return bResult;
   }

    // https://ihong5.wordpress.com/2014/07/24/apache-zookeeper-setting-acl-in-zookeeper-client/
    private static void setacl(String path, List<ACL> acl) throws KeeperException.InvalidACLException, KeeperException, InterruptedException, IllegalArgumentException {
       zk.setACL(path,acl,zk.exists(path,true).getVersion());
    }

    public static void main(String[] args) throws KeeperException, InterruptedException, IOException, NoSuchAlgorithmException {
        String path = "/sampleznode";
        byte[] data = "data in authenticated znode".getBytes();

        zkc = new ZkConnector();
        zk = zkc.connect("localhost");
        zk.create(path, data, ZooDefs.Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);

        List<ACL> acl = zk.getACL(path, zk.exists(path,true));
        System.out.println(acl.toString());

        ACL newAcl = new ACL();
        newAcl.setId(new Id("digest", generateDigest("datanotfound" + ":" + "channel123") ));
        newAcl.setPerms(getPermFromString("car"));

        System.out.println(newAcl.toString());

        List<ACL> lAcl = new ArrayList<ACL>();
        lAcl.add(newAcl);

        setacl(path, lAcl );

    }
}
