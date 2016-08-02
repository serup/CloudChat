package ProtocolHandlings;

import ClientEndpoint.DOPSClientEndpoint;
import dops.protocol.ded.DEDDecoder;
import dops.protocol.ded.DEDEncoder;

import java.io.IOException;
import java.lang.reflect.Field;
import java.nio.ByteBuffer;
import java.util.*;
import java.util.function.BiFunction;

/**
 * This class handles DED packet data communication between client and DOPS server
 * It has methods for login to DOPS DFD server and it handles basic communication
 * it is also a wrapper class for JavaWebSocketClientEndpoint
 * <p>
 * Created by serup on 09-05-16.
 */

public class DOPsCommunication {

    private DOPSClientEndpoint clientEndpoint = null;

    private BiFunction<String, dedAnalyzed, String> customHandlerFunction;
    private List<ActionHandlerObject> listOfActionHandlers = new ArrayList<>();
    private String serverURI = "ws://backend.scanva.com:7777";

    private class ActionHandlerObject
    {
        String type;
        BiFunction<String, dedAnalyzed, String> function;
    }

    public DOPsCommunication()
    {
        customHandlerFunction  = this::handleCommunication;
    }

    public boolean isOpen()
    {
        return clientEndpoint.clientSession.isOpen();
    }

    private ByteBuffer createDEDLogin(short trans_id, String uniqueId, String username, String password)
    {
        ByteBuffer data = null;
        /**
         * create DED connect datapacket for DOPS for java clients
         */
        DEDEncoder DED = new DEDEncoder();
        DED.PUT_STRUCT_START ( "WSRequest" );
        DED.PUT_METHOD   ( "Method",  "JavaConnect" );
        DED.PUT_USHORT   ( "TransID",  trans_id);
        DED.PUT_STDSTRING( "protocolTypeID", "DED1.00.00");
        DED.PUT_STDSTRING( "functionName", uniqueId );
        DED.PUT_STDSTRING( "username", username ); //TODO: SUGGESTION: use username to lookup in secure file, then use content in file as username and password
        DED.PUT_STDSTRING( "password", password );
        DED.PUT_STRUCT_END( "WSRequest" );
        data = DED.GET_ENCODED_BYTEBUFFER_DATA();

        return data;
    }

    public boolean loginToServer(String strURL, String port, String uniqueId, String username, String password)
    {
        boolean bResult=false;

        clientEndpoint = new DOPSClientEndpoint();
        clientEndpoint.addHandlerFunction(customHandlerFunction);

        /**
         * connect client
         */
        //clientEndpoint.connectToServer("ws://backend.scanva.com:7777");
        if(port.isEmpty()) port = "7777";
        clientEndpoint.connectToServer("ws://"+strURL+":"+port);

        /**
         * prepare data to be send
         */
        short trans_id = 69; // trans id for java clients
        String _uniqueId = uniqueId; // example : "985998707DF048B2A796B44C89345494";
        String _username = username; //           "johndoe@email.com";
        String _password = password; //           "12345";

        /**
         * create DED connect datapacket for DOPS for java clients
         */
        ByteBuffer data = createDEDLogin(trans_id,_uniqueId,_username,_password);

        /**
         * send to server with client current client session connection
         */
        clientEndpoint.sendToServer(data);

        /**
         * wait for incomming data response, then receive data
         */
        byte[] receivedData = clientEndpoint.receiveMessageFromServer();

        /**
         * decode incomming data
         */
        boolean bDecoded=false;
        String strMethod="";
        String strProtocolTypeID="";
        String strFunctionName="";
        String strStatus="";
        short uTrans_id=0;

        // decode data ...
        DEDDecoder DED2 = new DEDDecoder();
        DED2.PUT_DATA_IN_DECODER( receivedData, receivedData.length);
        if( DED2.GET_STRUCT_START( "WSResponse" )==1 &&
                (strMethod   = DED2.GET_METHOD ( "Method" )).length()>0 &&
                (uTrans_id     = DED2.GET_USHORT ( "TransID")) !=-1 &&
                (strProtocolTypeID  = DED2.GET_STDSTRING ( "protocolTypeID")).length()>0 )
        {
            if(strMethod.equals("1_1_6_LoginProfile"))
            {
                String strDestination="";
                String strSource="";
                // login response from DFD was received, now decode response
                if ( (strDestination = DED2.GET_STDSTRING ( "dest")).length()>0 &&
                        (strSource      = DED2.GET_STDSTRING ( "src")).length()>0 &&
                        (strStatus      = DED2.GET_STDSTRING ( "status")).length()>0 &&
                        DED2.GET_STRUCT_END( "WSResponse" )==1)
                {
                    // response from login to profile in DFD was received - now validate status
                    if(strStatus.equals("ACCEPTED")) {
                        // login success
                        bDecoded=true;
                        System.out.println("1_1_6_LoginProfile response packet decoded; src: "+strSource+" ; dest: "+strDestination+" ; Status: ACCEPTED");
                    }
                    else
                    {
                        // login failure is actually a correct response, since client is not registered
                        System.out.println("1_1_6_LoginProfile response packet decoded; however login failed ;  Status: "+strStatus);
                    }
                }
            }
            else
            { // When DFD is offline, then this type of packet is received
                if ( (strFunctionName    = DED2.GET_STDSTRING ( "functionName")).length()>0 &&
                        (strStatus  = DED2.GET_STDSTRING ( "status")).length()>0 &&
                        DED2.GET_STRUCT_END( "WSResponse" )==1)
                {
                    System.out.println("[loginToServer] Profile on server is OFF line: ");
                }
                else
                {
                    // unknown response
                    System.out.println("[loginToServer] unknown response from server");
                }
            }
        }
        bResult = bDecoded;
        return bResult;
    }

    public void setServerURI(String serverURI)
    {
        this.serverURI = serverURI;
    }

    public boolean connectToDOPs(String uniqueId, String username, String password) {
        boolean bResult=false;

        try {
            clientEndpoint = new DOPSClientEndpoint();
            clientEndpoint.addHandlerFunction(customHandlerFunction);
            clientEndpoint.connectToServer(serverURI);
            clientEndpoint.sendToServer(prepareConnectDEDToSend(uniqueId, username, password));
            bResult = decodeLoginResponse(clientEndpoint.receiveMessageFromServer()).contains("dops.connected.status.ok");
        } catch (Exception e) { e.printStackTrace(); }

        return bResult;
    }

    public boolean sendToServer(ByteBuffer data)
    {
        return (clientEndpoint.sendToServer(data) != -1);
    }

    public boolean disconnectFromDOPs() throws IOException {
        boolean bResult = false;
        if (clientEndpoint != null) {
            clientEndpoint.stopMessageHandler(); // make sure internal thread handling incoming data, is stopped
            clientEndpoint.clientSession.close();
            bResult = true;
        }
        return bResult;
    }

    private static ByteBuffer prepareConnectDEDToSend(String uniqueId, String username, String password) {
        short trans_id = 69;
        return createDEDforDOPSJavaConnect(trans_id, uniqueId, username, password);
    }

    private static String decodeLoginResponse(byte[] receivedData) {
        String Result = "dops.decode.status.error";
        boolean bDecoded = false;
        String strMethod;
        String strStatus;

        // decode data ...
        DEDDecoder DED2 = new DEDDecoder();
        DED2.PUT_DATA_IN_DECODER(receivedData, receivedData.length);
        if (DED2.GET_STRUCT_START("WSResponse") == 1 &&
                (strMethod = DED2.GET_METHOD("Method")).length() > 0 &&
                (DED2.GET_USHORT("TransID")) != -1 &&
                (DED2.GET_STDSTRING("protocolTypeID")).length() > 0) {
            if (strMethod.equals("1_1_6_LoginProfile")) {
                String strDestination;
                String strSource;
                // login response from DFD was received, now decode response
                if ((strDestination = DED2.GET_STDSTRING("dest")).length() > 0 &&
                        (strSource = DED2.GET_STDSTRING("src")).length() > 0 &&
                        (strStatus = DED2.GET_STDSTRING("status")).length() > 0 &&
                        DED2.GET_STRUCT_END("WSResponse") == 1) {
                    // response from login to profile in DFD was received - now validate status
                    if (strStatus.equals("ACCEPTED")) {
                        bDecoded = true;
                        System.out.println("1_1_6_LoginProfile response packet decoded; src: " + strSource + " ; dest: " + strDestination + " ; Status: ACCEPTED");
                    } else {
                        bDecoded = true; // set to true since login failure is actually a correct response, since client is not registered
                        System.out.println("1_1_6_LoginProfile response packet decoded; however login failed ;  Status: " + strStatus);
                    }
                }
            } else { // When DFD is offline, then this type of packet is received
                if (
                        (DED2.GET_STDSTRING("functionName")).length() > 0 &&
                                (DED2.GET_STDSTRING("status")).length() > 0 &&
                                DED2.GET_STRUCT_END("WSResponse") == 1) {
                    bDecoded = true;
                    System.out.println("- DED packet decoded - now validate");

                    if (!strMethod.equals("JavaConnect")) bDecoded = false;
                } else {
                    // unknown method
                    System.out.println("- DED packet could NOT be decoded - unknown Method: " + strMethod);
                }
            }
        } else {
            bDecoded = false;
        }

        if (bDecoded)
            Result = "dops.connected.status.ok";
        return Result;
    }

    private static ByteBuffer createDEDforDOPSJavaConnect(short trans_id, String uniqueId, String username, String password) {
        DEDEncoder DED = new DEDEncoder();
        DED.PUT_STRUCT_START("WSRequest");
        DED.PUT_METHOD("Method", "JavaConnect");
        DED.PUT_USHORT("TransID", trans_id);
        DED.PUT_STDSTRING("protocolTypeID", "DED1.00.00");
        DED.PUT_STDSTRING("functionName", uniqueId);
        DED.PUT_STDSTRING("username", username);
        DED.PUT_STDSTRING("password", password);
        DED.PUT_STRUCT_END("WSRequest");

        return DED.GET_ENCODED_BYTEBUFFER_DATA();
    }

     /**
     * Decodes the DED package and creates a result object containing type and fields
     * this function is called on client connection level and helps with determining the
     * incoming data traffic
     *
     * @param DED
     * @return dedAnalyzed object -- contains decoded values of the DED package
     * @throws Exception
     */
    public static dedAnalyzed decodeIncomingDED(byte[] DED) {
        DOPsCommunication.dedAnalyzed dana = new DOPsCommunication.dedAnalyzed();

        if (DED == null) {
            dana.type = "<unknown>";
            dana.bDecoded = false;
        } else {
            // decode data ...
            DEDDecoder DED2 = new DEDDecoder();
            DED2.PUT_DATA_IN_DECODER(DED, DED.length);
            if ((DED2.GET_STRUCT_START("CloudManagerRequest") == 1)) {
                String method = DED2.GET_METHOD("Method");
                try {
                    switch (method) {
                        case "JSCForwardInfo":
                            ForwardInfoRequestObj fio = new DOPsCommunication.ForwardInfoRequestObj();
                            if ((fio.transactionsID = DED2.GET_USHORT("TransID")) != -1 &&
                                    (fio.protocolTypeID = DED2.GET_STDSTRING("protocolTypeID")).contains("DED1.00.00") &&
                                    (fio.dest = DED2.GET_STDSTRING("dest")).length() > 0 &&
                                    (fio.src = DED2.GET_STDSTRING("src")).length() > 0 &&
                                    (fio.srcAlias = DED2.GET_STDSTRING("srcAlias")).length() > 0 &&
                                    (DED2.GET_STRUCT_END("CloudManagerRequest")) == 1) {
                                dana.bDecoded = true;
                                dana.type = "ChatForwardInfoRequest";
                                dana.setDED(DED);
                                dana.elements = fio;
                            }
                            break;
                        case "JSCChatInfo":
                            ChatInfoObj cio = new DOPsCommunication.ChatInfoObj();
                            if ((cio.transactionsID = DED2.GET_USHORT("TransID")) != -1 &&
                                    (cio.protocolTypeID = DED2.GET_STDSTRING("protocolTypeID")).contains("DED1.00.00") &&
                                    (cio.dest = DED2.GET_STDSTRING("dest")).length() > 0 &&
                                    (cio.src = DED2.GET_STDSTRING("src")).length() > 0 &&
                                    (cio.srcAlias = DED2.GET_STDSTRING("srcAlias")).length() > 0 &&
                                    (cio.srcHomepageAlias = DED2.GET_STDSTRING("srcHomepageAlias")).length() > 0 &&
                                    (cio.lastEntryTime = DED2.GET_STDSTRING("lastEntryTime")).length() > 0 &&
                                    (DED2.GET_STRUCT_END("ClientChatRequest")) == 1) {
                                dana.bDecoded = true;
                                dana.type = "ChatInfo";
                                dana.setDED(DED);
                                dana.elements = cio;
                            }
                            break;

                        default:
                            dana.bDecoded = false;
                            dana.type = "<unknown> : " + method;
                            break;
                    }
                } catch (Exception e) {
                    dana.bDecoded = false;
                    dana.type = method;
                    dana.elements = null;
                    System.out.println("- Exception happened trying to decode ded of type : " + dana.type);
                }
            }
        }
        return dana;
    }

    public static ByteBuffer createDEDpackage(dedAnalyzed dana)
    {
        ByteBuffer blob = null;
        if(dana.elements instanceof ForwardInfoRequestObj) {
            DEDEncoder DED = new DEDEncoder();
            DED.PUT_STRUCT_START ("CloudManagerRequest");
                DED.PUT_METHOD ("Method", "JSCForwardInfo");
                DED.PUT_USHORT ("TransID", (short)dana.getElement("transactionsID"));
                DED.PUT_STDSTRING("protocolTypeID", dana.getElement("protocolTypeID").toString());
                DED.PUT_STDSTRING("dest", dana.getElement("dest").toString());  // This Admins uniqueID, since this is a test it really does not matter
                DED.PUT_STDSTRING("src", dana.getElement("src").toString()); // uniqueID
                DED.PUT_STDSTRING("srcAlias", dana.getElement("srcAlias").toString());
            DED.PUT_STRUCT_END("CloudManagerRequest");
            blob = DED.GET_ENCODED_BYTEBUFFER_DATA();
        }
        if(dana.elements instanceof ChatInfoObj) {
            DEDEncoder DED = new DEDEncoder();
            DED.PUT_STRUCT_START ("ClientChatRequest");
                DED.PUT_METHOD ("Method", "JSCChatInfo");
                DED.PUT_USHORT ("TransID", (short)dana.getElement("transactionsID"));
                DED.PUT_STDSTRING("protocolTypeID", dana.getElement("protocolTypeID").toString());
                DED.PUT_STDSTRING("dest", dana.getElement("dest").toString());  // This Admins uniqueID, since this is a test it really does not matter
                DED.PUT_STDSTRING("src", dana.getElement("src").toString()); // uniqueID
                DED.PUT_STDSTRING("srcAlias", dana.getElement("srcAlias").toString());
                DED.PUT_STDSTRING("srcHomepageAlias", dana.getElement("srcHomepageAlias").toString());
                DED.PUT_STDSTRING("lastEntryTime", dana.getElement("lastEntryTime").toString());
            DED.PUT_STRUCT_END("ClientChatRequest");
            blob = DED.GET_ENCODED_BYTEBUFFER_DATA();
        }
        return blob;
    }

    /**
     * The main object for decoded DED package
     */
    public static class dedAnalyzed {

        public String type;
        public boolean bDecoded;
        public Object elements; // will contain an object of following below type:
        private byte[] originalDED = null;

        public byte[] getDED() throws Exception {
            if (originalDED == null)
                throw new Exception("ERROR: dedAnalyzed did NOT contain the original ded - FATAL");
            return originalDED;
        }

        public ByteBuffer getByteBuffer() throws Exception {
            return ByteBuffer.wrap(this.getDED());
        }

        public void setDED(byte[] ded) {
            originalDED = ded;
        }

        public Object getElement(String name) {
            Object objResult;
            try {
                Field field = elements.getClass().getDeclaredField(name);
                field.setAccessible(true);
                objResult = field.get(elements);
            } catch (Exception e) {
                System.out.printf("- [getElement] ERROR : java.lang.NoSuchFieldException : %s\n", name );
                e.printStackTrace();
                objResult = "<errorInElement>";
            }
            if(objResult == null) {
                objResult = "<errorInElement>";
                System.out.printf("- [getElement] ERROR : element was NOT found : %s\n", name);
            }
            return objResult;
        }

        public Object getElements()
        {
            return elements;
        }

    }

    /**
     * DED packages can contain different types of object
     * here is a few
     *
     */
    public static class ForwardInfoRequestObj {
        public short transactionsID;
        public String protocolTypeID;
        public String dest;
        public String src;
        public String srcAlias;
    }
    public static class ChatInfoObj {
        public short transactionsID;
        public String protocolTypeID;
        public String dest;
        public String src;
        public String srcAlias;
        public String srcHomepageAlias;
        public String lastEntryTime;
    }

    /**
     * Function for adding handlers for retrieval of different types of DED analyzed objects
     * fx. add a handlerfunction for retrieving 'ChatInfo' objects, so it can add to a viewcontroller listbox
     */
    public void addActionHandler(String type, BiFunction<String, dedAnalyzed, String> customHandlerFunction)
    {
        ActionHandlerObject actionHandlerObject = new ActionHandlerObject();
        actionHandlerObject.type = type;
        actionHandlerObject.function = customHandlerFunction;
        listOfActionHandlers.add(actionHandlerObject);
    }

    /**
     * Main handler function for incoming data traffic from server
     * traffic should be DED datapackages
     *
     * This function is added as a 'handler' function for internal client connection
     * distributing incoming data objects to assigned handler functions
     *
     * @param type  -- denotes the type of the analysed DED package - fx. 'ChatInfo'
     * @param dana  -- this is the decoded DED package
     */
    private String handleCommunication(String type, dedAnalyzed dana)
    {
        System.out.println("- [handleCommunication] transfer received object to custom action handlers based on type ");

        if(listOfActionHandlers.stream().filter(d -> d.type.contains(type)).count() > 0) {
            listOfActionHandlers.stream()
                    .filter(d -> d.type.contains(type))
                    .map(d -> d.function)
                    .forEach(d -> d.apply(type, dana));
        }
        else
        {
            System.out.printf("- [handleCommunication] WARNING NO handlers for this type : %s  - please check type where code is adding handlers, it could be a spelling mistake\n", dana.type);
            System.out.printf("- [handleCommunication] Here are the following registered handlers : \n");
            listOfActionHandlers.stream().forEach(d -> System.out.printf("- [handleCommunication] * %s\n",d.type));
        }
        return "OK";
    }
}




