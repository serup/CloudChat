package com.zooktutorial.app;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.data.ACL;

import java.io.IOException;
import java.util.List;

/**
 * Created by serup on 11/21/16.
 */
public class GetACL {
    private static ZooKeeper zk;
    private static ZkConnector zkc;

    private static List<ACL> getacl(String path) throws KeeperException, InterruptedException {
       return zk.getACL(path, zk.exists(path,true));
    }

    public static void main(String[] args) throws KeeperException, InterruptedException, IOException {
        String path = "/sampleznode";

        zkc = new ZkConnector();
        zk = zkc.connect("localhost");

        List<ACL> acl = getacl(path);

        for (ACL aclitem: acl) {
            System.out.println(aclitem.toString());
        }
    }
}
