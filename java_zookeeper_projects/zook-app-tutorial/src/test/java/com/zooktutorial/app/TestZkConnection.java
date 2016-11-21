package com.zooktutorial.app;

import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.ZooKeeper;
import org.junit.Test;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

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

        for (String znode : znodeList) {
            System.out.println(znode);
        }
    }

    @Test
    public void createZNode() throws Exception {
        String path = "/sampleznode";
        byte[] data = "sample znode data".getBytes();

        CreateZNode znode = new CreateZNode();
        zkc = new ZkConnector();
        znode.zk = zkc.connect("localhost");
        znode.create(path, data);

        System.out.println("I created sampleznode successfully! ");
    }
}
