package dops.websocket;

import JavaServicesApp.ClientEndpoint.JavaWebSocketClientEndpoint;
import dops.mocks.setupMockServer;
import WebSocketEchoTestEndpoints.EchoByteArrayEndpoint;
import dops.protocol.ded.DEDDecoder;
import dops.protocol.ded.DEDEncoder;
import org.glassfish.tyrus.server.Server;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;

import static junit.framework.TestCase.assertEquals;

/**
 * Created by serup on 22-12-15.
 */
public class MockDOPsServerTests {

    private static setupMockServer setupMockServer =null;
    private static Thread serverThread;

    private void runByteArrayServer() {
        serverThread = new Thread() {
            public void run() {
                Server server = new Server("localhost", 8033, "/websockets", null, EchoByteArrayEndpoint.class);
                try {
                    server.start();
                    BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
                    System.out.println("Server ready and waiting for input.");
                    reader.read();
                } catch (Exception e) {
                    e.printStackTrace();
                } finally {
                    server.stop();
                }
            }
        };
        serverThread.start();
    }


    @Before
    public void setupMockServer()
    {
        /**
         * Start and connect to a MOCK server which can act as a Server and receive a DED packet and return a DED packet with result
         */
        if(setupMockServer ==null)
            setupMockServer = new setupMockServer(8046,"MockServerEndpoint");
    }

    @Test
    @Ignore // some how it is not possible to run multiple servers even if port is different - hmm !?!?!
    public void testClientServerBinaryEcho() throws Exception {

        // no need for setupMockServer, so close it down if running - there is however issues when localhost has a running server - normally there could be more servers on seperate ports, however in Java there seems to be some issues
        if(setupMockServer !=null)
            setupMockServer.stopServer();

        /**
         * First start a server which can receive and echo back a binary array
         */
        runByteArrayServer();

        /**
         * setup client
         */
        JavaWebSocketClientEndpoint clientEndpoint = new JavaWebSocketClientEndpoint();

        /**
         * connect client
         */
        clientEndpoint.connectToServer("ws://localhost:8033/websockets/echoBinary");

        /**
         * prepare data to be send
         */
        short trans_id = 1;
        boolean action = true;

        DEDEncoder DED = new DEDEncoder();
        DED.PUT_STRUCT_START( "event" );
        DED.PUT_METHOD  ( "name",  "MusicPlayer" );
        DED.PUT_USHORT  ( "trans_id",  trans_id);
        DED.PUT_BOOL    ( "startstop", action );
        DED.PUT_STDSTRING("text", "JavaServicesApp world");
        DED.PUT_STRUCT_END( "event" );

        ByteBuffer data = DED.GET_ENCODED_BYTEBUFFER_DATA();

        /**
         * send to server with client current client session connection
         */
        clientEndpoint.sendToServer(data);

        /**
         * wait for incomming data response, then receive data
         */
        byte[] receivedData = clientEndpoint.receiveFromServer();

        /**
         * decode incomming data
         */
        boolean bDecoded=false;
        String strValue="";
        short iValue=0;
        boolean bValue=false;
        String strText="";

        // decode data ...
        DEDDecoder DED2 = new DEDDecoder();
        DED2.PUT_DATA_IN_DECODER( receivedData, receivedData.length);
        if( DED2.GET_STRUCT_START( "event" )==1 &&
                (strValue = DED2.GET_METHOD ( "name" )).length()>0 &&
                (iValue   = DED2.GET_USHORT ( "trans_id")) !=-1 &&
                (bValue   = DED2.GET_BOOL   ( "startstop")) &&
                (strText  = DED2.GET_STDSTRING ( "text")).length()>0 &&
                DED2.GET_STRUCT_END( "event" )==1)
        {
            bDecoded=true;
        }
        else
        {
            bDecoded=false;
        }

        assertEquals(true,bDecoded);

    }


    @Test
    public void testCreateProfileInMockServer()
    {
        boolean bCreatedProfile=false;
        assertEquals(true, setupMockServer.isOpen());

        /**
         * prepare data to be send
         */
        String EntityFileName = java.util.UUID.randomUUID().toString().replace("-","");
        String EntityTOASTFileName = java.util.UUID.randomUUID().toString().replace("-","");
        short trans_id = 66;

        DEDEncoder DED = new DEDEncoder();
        DED.PUT_STRUCT_START( "DFDRequest" );
        //+ fixed area start
        DED.PUT_METHOD	( "Method",  "1_1_1_CreateProfile" );
        DED.PUT_USHORT	( "TransID", trans_id);
        DED.PUT_STDSTRING	( "protocolTypeID", "DED1.00.00" );
        DED.PUT_STDSTRING	( "dest", "DFD_1.1" );
        DED.PUT_STDSTRING	( "src", "javaclient" ); // should be this client
        //- fixed area end
        //+ Profile record area start
        DED.PUT_STDSTRING	( "STARTrequest", "EmployeeRequest" );
        DED.PUT_STDSTRING	( "STARTrecord", "record" );
        DED.PUT_STDSTRING	( "profileID", EntityFileName );
        DED.PUT_STDSTRING	( "profileName", "TestProfile" );
        DED.PUT_STDSTRING	( "protocolTypeID","DED1.00.00" );
        DED.PUT_STDSTRING	( "sizeofProfileData", "0" );
        DED.PUT_STDSTRING	( "profile_chunk_id", EntityTOASTFileName );
        DED.PUT_STDSTRING   ( "AccountStatus", "1" );
        DED.PUT_STDSTRING   ( "ExpireDate", "20160101" );
        DED.PUT_STDSTRING   ( "ProfileStatus", "2" );
        DED.PUT_STDSTRING	( "STARTtoast", "elements" );
        ///+ start toast elements
        DED.PUT_ELEMENT( "profile", "lifecyclestate", "1"); /// will add profile_chunk_id and profile_chunk_data
        DED.PUT_ELEMENT( "profile", "username", "serup" );
        DED.PUT_ELEMENT( "profile", "password", "sussiskoller" );
        DED.PUT_ELEMENT( "profile", "devicelist", " " );
        DED.PUT_ELEMENT( "profile", "firstname", "Johnny" );
        DED.PUT_ELEMENT( "profile", "lastname", "Serup" );
        DED.PUT_ELEMENT( "profile", "streetname", "..." );
        DED.PUT_ELEMENT( "profile", "streetno", "..." );
        DED.PUT_ELEMENT( "profile", "postalcode", "..." );
        DED.PUT_ELEMENT( "profile", "city", "..." );
        DED.PUT_ELEMENT( "profile", "state", "..." );
        DED.PUT_ELEMENT( "profile", "country", "..." );
        DED.PUT_ELEMENT( "profile", "email", "..." );
        DED.PUT_ELEMENT( "profile", "mobilephone", "..." );
        DED.PUT_ELEMENT( "profile", "title", "..." );
        DED.PUT_ELEMENT( "profile", "about", "..." );
        DED.PUT_ELEMENT( "profile", "foto", "data:image/jpeg;base64,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" );
        ///- end toast elements
        DED.PUT_STDSTRING	( "ENDtoast", "elements" );
        //- Profile record area end
        DED.PUT_STDSTRING	( "ENDrecord", "record" );
        DED.PUT_STDSTRING	( "ENDrequest", "EmployeeRequest" );
        //- Profile request area end
        DED.PUT_STRUCT_END( "DFDRequest" );

        ByteBuffer data = DED.GET_ENCODED_BYTEBUFFER_DATA();

        /**
         * send to server with client current client session connection
         */
        setupMockServer.sendToServer(data);


        /**
         * wait for incomming data response, then receive data - or timeout
         */
        byte[] receivedData = setupMockServer.receiveFromServer();


        /**
         * decode incomming data
         */
        boolean bDecoded=false;
        String strMethod="";
        String strProtocolTypeID="";
        String strDestination="";
        String strSource="";
        String strStatus="";
        short uTrans_id=0;

        // decode data ...
        DEDDecoder DED2 = new DEDDecoder();
        DED2.PUT_DATA_IN_DECODER( receivedData, receivedData.length);
        if( DED2.GET_STRUCT_START( "DFDResponse" )==1 &&
                (DED2.GET_METHOD ( "Method" )).contains("CreateProfile") &&
                (uTrans_id = DED2.GET_USHORT ( "TransID")) !=-1 &&
                (DED2.GET_STDSTRING ( "protocolTypeID")).contains("DED1.00.00") &&
                (strDestination    = DED2.GET_STDSTRING ( "dest")).length()>0 &&
                (strSource         = DED2.GET_STDSTRING ( "src")).length()>0 &&
                (strStatus         = DED2.GET_STDSTRING ( "status")).length()>0 &&
                DED2.GET_STRUCT_END( "DFDResponse" )==1)
        {
            if(strStatus.contains("Profile Saved in database") && uTrans_id==trans_id)
                bCreatedProfile=true;
        }

        assertEquals(true,bCreatedProfile);
    }

    @Test
    public void testLoginToMockServer() throws Exception {
        assertEquals(true, setupMockServer.isOpen());

        /**
         * prepare data to be send
         */
        short trans_id = 69;
        boolean action = true;
        String uniqueId = "985998707DF048B2A796B44C89345494";
        String username = "johndoe@email.com"; // TODO: find a way to safely handle retrieval of username,password - should NOT be stored in source code
        String password = "12345";

        /**
         * create DED connect datapacket for DOPS for java clients
         */
        DEDEncoder DED = new DEDEncoder();
        DED.PUT_STRUCT_START ( "WSRequest" );
            DED.PUT_METHOD   ( "name",  "JavaConnect" );
            DED.PUT_USHORT   ( "trans_id",  trans_id);
            DED.PUT_STDSTRING( "protocolTypeID", "DED1.00.00");
            DED.PUT_STDSTRING( "functionName", uniqueId );
            DED.PUT_STDSTRING( "username", username );
            DED.PUT_STDSTRING( "password", password );
        DED.PUT_STRUCT_END( "WSRequest" );

        ByteBuffer data = DED.GET_ENCODED_BYTEBUFFER_DATA();

        /**
         * send to server with client current client session connection
         */
        setupMockServer.sendToServer(data);


        /**
         * wait for incomming data response, then receive data - or timeout
         */
        byte[] receivedData = setupMockServer.receiveFromServer();


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
                (strMethod   = DED2.GET_METHOD ( "name" )).length()>0 &&
                (uTrans_id     = DED2.GET_USHORT ( "trans_id")) !=-1 &&
                (strProtocolTypeID  = DED2.GET_STDSTRING ( "protocolTypeID")).length()>0 &&
                (strFunctionName    = DED2.GET_STDSTRING ( "functionName")).length()>0 &&
                (strStatus  = DED2.GET_STDSTRING ( "status")).length()>0 &&
            DED2.GET_STRUCT_END( "WSResponse" )==1)
        {
            bDecoded=true;
            System.out.println("DED packet decoded - now validate");

            if(!strMethod.equals("JavaConnect")) bDecoded=false;
            if(uTrans_id != trans_id) bDecoded=false;
            assertEquals(true,bDecoded);
            if(!strFunctionName.equals(uniqueId)) bDecoded=false;
            assertEquals(true,bDecoded);
            if(!strStatus.equals("ACCEPTED")) bDecoded=false;
            assertEquals(true,bDecoded);
            if(!strProtocolTypeID.equals("DED1.00.00")) bDecoded=false;

            if(bDecoded)
                System.out.println("DED packet validated - OK");

        }
        else
        {
            bDecoded=false;
        }

        assertEquals(true,bDecoded);
    }

    @Test
    @Ignore
    public void testCSharpWithLoginToMockServer() throws Exception {
        assertEquals(true, setupMockServer.isOpen());

        // Try to start a html page with javascript websocket and connect to this running test case - example:
        /*
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
    <title>websocket client</title>
    <script type="text/javascript">
        var start = function () {
            var inc = document.getElementById('incomming');
            var wsImpl = window.WebSocket || window.MozWebSocket;
            var form = document.getElementById('sendForm');
            var input = document.getElementById('sendText');

            inc.innerHTML += "connecting to server ..<br/>";

            // create a new websocket and connect
            window.ws = new wsImpl('ws://127.0.0.1:8046/websockets/MockServerEndpoint');

            // when data is comming from the server, this metod is called
            ws.onmessage = function (evt) {
                inc.innerHTML += evt.data + '<br/>';
            };

            // when the connection is established, this method is called
            ws.onopen = function () {
                inc.innerHTML += '.. connection open<br/>';
            };

            // when the connection is closed, this method is called
            ws.onclose = function () {
                inc.innerHTML += '.. connection closed<br/>';
            }

			form.addEventListener('submit', function(e){
				e.preventDefault();
				var val = input.value;
				ws.send(val);
				input.value = "";
			});

        }
        window.onload = start;
    </script>
</head>
<body>
	<form id="sendForm">
		<input id="sendText" placeholder="Text to send" />
	</form>
    <pre id="incomming"></pre>
</body>
</html>
         */

        while(true)
        {
            Thread.sleep(1000);
        }
    }

}
