package JavaServicesApp.ProtocolHandlings;

import java.util.List;

/**
 * Created by serup on 6/14/16.
 */
public class dedAnalyzed {

    public String type;
    public boolean bDecoded;
    public Object elements; // will contain an object of following below type:
    private byte[] originalDED = null;

    public byte[] getDED() throws Exception {
        if(originalDED == null)
            throw new Exception("ERROR: dedAnalyzed did NOT contain the original ded - FATAL");
        return originalDED;
    }

    public void setDED(byte[] ded)
    {
        originalDED = ded;
    }


}

class element {
    public String name;
    public String value;
}

// object types
// Objects for elements in dedAnalyzed
class ForwardInfoRequestObj {
    public short transactionsID;
    public String protocolTypeID;
    public String dest;
    public String src;
    public String srcAlias;
}

class ChatInfoObj {
    public short transactionsID;
    public String protocolTypeID;
    public String dest;
    public String src;
    public String srcAlias;
    public String srcHomepageAlias;
    public String lastEntryTime;
}


