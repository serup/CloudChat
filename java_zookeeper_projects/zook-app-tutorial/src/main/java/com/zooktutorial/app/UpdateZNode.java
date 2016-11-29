package com.zooktutorial.app;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;

import java.io.IOException;

/**
 * Created by serup on 11/21/16.
 */
public class UpdateZNode {
    public static ZooKeeper zk;
    private static ZkConnector zkc;

    public static void update(String path, byte[] data) throws KeeperException, InterruptedException {
        zk.setData(path,data, zk.exists(path,true).getVersion());
    }

    public static void main(String[] args) throws IOException, KeeperException, InterruptedException {
        String path = "/sampleznode";
        byte[] data = "I have been updated by my Master!".getBytes();

        zkc = new ZkConnector();
        zk = zkc.connect("localhost");

        update(path, data);

        System.out.println("I changed the data in sampleznode successfully! ");

    }
}
