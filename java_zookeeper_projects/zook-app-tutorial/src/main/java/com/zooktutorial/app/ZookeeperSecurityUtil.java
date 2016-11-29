package com.zooktutorial.app;

import org.apache.zookeeper.ZooDefs;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Base64;
import java.util.List;

/**
 * Created by serup on 11/22/16.
 */
public class ZookeeperSecurityUtil {

    private static final String SHA1 = "SHA1";
    private static final String COLON = ":";
    private static final String DIGEST_SCHEME = "digest";

    public static int getPermFromString(String permString) {
        int perm = 0;
        for (int i = 0; i < permString.length(); i++) {
            switch (permString.charAt(i)) {
                case 'r':
                    perm |= ZooDefs.Perms.READ;
                    break;
                case 'w':
                    perm |= ZooDefs.Perms.WRITE;
                    break;
                case 'c':
                    perm |= ZooDefs.Perms.CREATE;
                    break;
                case 'd':
                    perm |= ZooDefs.Perms.DELETE;
                    break;
                case 'a':
                    perm |= ZooDefs.Perms.ADMIN;
                    break;
                default:
                    System.err.println("Unknown perm type: " + permString.charAt(i));
            }
        }
        return perm;
    }

    public static List<AuthInfo> getCredentialsFromSystemProperties() {
        final List<AuthInfo> authInfo = new ArrayList<>();
        final String user = System.getProperty("zookeeper.security.user");
        final String password = System.getProperty("zookeeper.security.password");
        authInfo.add(new AuthInfo(DIGEST_SCHEME, new String(user + COLON + password).getBytes()));
        return authInfo;
    }

    public static String generateDigest(final String idPassword) throws NoSuchAlgorithmException {
        final String parts[] = idPassword.split(COLON, 2);
        final byte digest[] = MessageDigest.getInstance(SHA1).digest(idPassword.getBytes());
        return parts[0] + COLON + base64Encode(digest);
    }

    private static String base64Encode(final byte byteDigest[]) {
        return new String(Base64.getEncoder().encode(byteDigest));
    }

    private static class AuthInfo {
        String scheme;
        byte[] data;

        AuthInfo(String scheme, byte[] data) {
            this.scheme = scheme;
            this.data = data;
        }
     }
}