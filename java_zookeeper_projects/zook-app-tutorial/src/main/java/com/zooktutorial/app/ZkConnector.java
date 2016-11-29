package com.zooktutorial.app;

import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.ZooKeeper;

import java.io.IOException;

/**
 * Created by serup on 11/21/16.
 */
public class ZkConnector {
    private ZooKeeper zk;
    private java.util.concurrent.CountDownLatch connSignal = new java.util.concurrent.CountDownLatch(1);
    public ZooKeeper connect(String host) throws IOException, InterruptedException, IllegalStateException {
        zk = new ZooKeeper(host, 5000, new Watcher() {
           public void process(WatchedEvent event) {
               if (event.getState() == Event.KeeperState.SyncConnected) {
                   connSignal.countDown();
               }
           }
        });
        connSignal.await();
        return zk;
    }

    public void close() throws InterruptedException {
        zk.close();
    }
}
