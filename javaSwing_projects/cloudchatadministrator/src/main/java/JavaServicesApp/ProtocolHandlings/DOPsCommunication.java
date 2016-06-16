package JavaServicesApp.ProtocolHandlings;

import JavaServicesApp.ClientEndpoint.DOPSClientEndpoint;
import dops.protocol.ded.DEDDecoder;
import dops.protocol.ded.DEDEncoder;

import java.io.IOException;
import java.nio.ByteBuffer;
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

    public boolean connectToDOPs(String uniqueId, String username, String password) {
        boolean bResult;
        BiFunction<String, dedAnalyzed, String> customHandlerFunction  = this::handleCommunication;
        clientEndpoint = new DOPSClientEndpoint();
        clientEndpoint.addHandlerFunction(customHandlerFunction);
        clientEndpoint.connectToServer("ws://backend.scanva.com:7777");

        ByteBuffer data = prepareConnectDEDToSend(uniqueId, username, password);
        clientEndpoint.sendToServer(data);
        bResult = decodeLoginResponse(clientEndpoint.receiveMessageFromServer()).contains("dops.connected.status.ok");

        return bResult;
    }

    public boolean disconnectFromDOPs() throws IOException {
        boolean bResult = false;
        if (clientEndpoint != null) {
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
                    System.out.println("DED packet decoded - now validate");

                    if (!strMethod.equals("JavaConnect")) bDecoded = false;
                } else {
                    // unknown method
                    System.out.println("DED packet could NOT be decoded - unknown Method: " + strMethod);
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
                                dana.type = "ForwardInfoRequest";
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
                            dana.type = "<unknown>";
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

        public void setDED(byte[] ded) {
            originalDED = ded;
        }
    }

    /**
     * DED packages can contain different types of object
     * here is a few
     *
     */
    private static class ForwardInfoRequestObj {
        short transactionsID;
        String protocolTypeID;
        String dest;
        String src;
        String srcAlias;
    }
    private static class ChatInfoObj {
        short transactionsID;
        String protocolTypeID;
        String dest;
        String src;
        String srcAlias;
        String srcHomepageAlias;
        String lastEntryTime;
    }

    /**
     * Main handler function for incoming data traffic from server
     * traffic should be DED datapackages
     *
     * This function is added as a handlerfunction for internal client connection
     *
     * @param type  -- denotes the type of the analysed DED package - fx. 'ChatInfo'
     * @param dana  -- this is the decoded DED package
     */
    private String handleCommunication(String type, dedAnalyzed dana)
    {
        String strResult="Error in communication";
        System.out.println("- handleCommunication handler function called ");
        return strResult;
    }
}




