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
        // permission sets is in binary as follows:
        // Read, Write, Create, Delete, Admin
        // 2^0 , 2^1  , 2^2   , 2^3   , 2^4
        // example:
        // permission set cdaw = 2^2 + 2^3 + 2^4 + 2^1 = 4 + 8 +16 + 2 = 12 + 18 = 30
        // result example:
        // 30, s{'world,'anyone}
        // scheme can be following:
        // - world    ; represents anyone in the world who has access to zookeeper ensemble
        // - digest   ; represents one single individual with their own unique id and password
        // - host     ; represents group of users within that host (myhost.com)
        // - ip       ; represents group of users within that same ip address
        // if scheme is other than world, then scheme zookeeper handle it as follows:
        // <username>:<passworld>  --> convert to hexadecimal using SHA-1 (Secure Hashing Algorithm)
        // then again --> encode with Base64, to get a value which zookeeper uses to authenticate user trying to access
        // a znode ( a path to a value inside the zookeeper tree structure )
        // example:
        // first add an authentication digest
        //  addauth digest datanotfound:channel123
        // then set authentication on znode with username = datanotfound, and password = channel123
        //  setAcl /sampleznode auth:datanotfound:channel123:card
        // Now you can get authentication :
        // [zk: localhost:2181(CONNECTED) 7] getAcl /sampleznode
        // 'digest,'datanotfound:gl+StP2pWGs7qLgwPXvcO8IQWc0=
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
