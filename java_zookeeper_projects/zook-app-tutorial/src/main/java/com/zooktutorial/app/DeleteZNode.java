package com.zooktutorial.app;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;

import java.io.IOException;

/**
 * Created by serup on 11/21/16.
 */
public class DeleteZNode {
    public static ZooKeeper zk;
    private static ZkConnector zkc;

    public static void delete(String path) throws KeeperException, InterruptedException {
        zk.delete(path, zk.exists(path,true).getVersion());
    }

    public static void main(String[] args) throws IOException, InterruptedException, KeeperException {
        String path = "/sampleznode";
        zkc = new ZkConnector();
        zk = zkc.connect("localhost");

        delete(path);

        System.out.println("I deleted sampleznode successfully! ");
    }
}
