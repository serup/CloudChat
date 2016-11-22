package com.zooktutorial.app;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;

import java.io.IOException;

/**
 * Created by serup on 11/21/16.
 */
public class ReadZNode {
    public static ZooKeeper zk;
    private static ZkConnector zkc;

    public static byte[] read(String path) throws KeeperException, InterruptedException {
        return zk.getData(path, true, zk.exists(path,true));
    }
    public static void main(String[] args) throws KeeperException, IOException, InterruptedException {
        String path = "/sampleznode";
        byte[] data;
        zkc = new ZkConnector();
        zk = zkc.connect("localhost");

        data = read(path);

        for (byte b: data) {
            System.out.print((char)b);

        }
    }
}
