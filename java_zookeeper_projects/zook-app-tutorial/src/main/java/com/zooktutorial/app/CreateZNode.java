package com.zooktutorial.app;

import org.apache.zookeeper.CreateMode;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooDefs;
import org.apache.zookeeper.ZooKeeper;

import java.io.IOException;

/**
 * Created by serup on 11/21/16.
 */
public class CreateZNode {

    private static ZooKeeper zk;
    private static ZkConnector zkc;
    public static void create(String path, byte[] data) throws KeeperException, InterruptedException {
        zk.create(path, data, ZooDefs.Ids.OPEN_ACL_UNSAFE, CreateMode.PERSISTENT);
    }
    public static void main(String[] args) throws KeeperException, InterruptedException, IOException {
        String path = "/sampleznode";
        byte[] data = "sample znode data".getBytes();

        zkc = new ZkConnector();
        zk = zkc.connect("localhost");
        create(path, data);

        System.out.println("I created sampleznode successfully! ");
    }
}
