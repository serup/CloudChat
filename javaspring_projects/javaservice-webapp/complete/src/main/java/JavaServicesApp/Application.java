package JavaServicesApp;

import JavaServicesApp.ClientEndpoint.JavaWebSocketClientEndpoint;
import dops.protocol.ded.DEDDecoder;
import dops.protocol.ded.DEDEncoder;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.List;

import static org.junit.Assert.assertEquals;

@SpringBootApplication
public class Application {

    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);  // https://spring.io/guides/gs/spring-boot/

        // connect to DOPS and login to DFD as HadoopJavaServiceApp
        if(isEnvironmentOK()) {
            if(connectToDOPs()) {

            }
       }
    }


    private static boolean connectToDOPs(){
        boolean bResult;
        JavaWebSocketClientEndpoint clientEndpoint = new JavaWebSocketClientEndpoint();
        clientEndpoint.connectToServer("ws://backend.scanva.com:7777");

        ByteBuffer data = prepareDataToSend();
        clientEndpoint.sendToServer(data);
        bResult = decodeIncomingData(clientEndpoint.receiveFromServer()).contains("dops.connected.status.ok");

        return bResult;
    }

    private static String decodeIncomingData(byte[] receivedData)
    {
        String Result="dops.decode.status.error";
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
                        bDecoded=true;
                        System.out.println("1_1_6_LoginProfile response packet decoded; src: "+strSource+" ; dest: "+strDestination+" ; Status: ACCEPTED");
                    }
                    else
                    {
                        bDecoded=true; // set to true since login failure is actually a correct response, since client is not registered
                        System.out.println("1_1_6_LoginProfile response packet decoded; however login failed ;  Status: "+strStatus);
                    }
                }
            }
            else
            { // When DFD is offline, then this type of packet is received
                if (
                        (strFunctionName    = DED2.GET_STDSTRING ( "functionName")).length()>0 &&
                                (strStatus  = DED2.GET_STDSTRING ( "status")).length()>0 &&
                                DED2.GET_STRUCT_END( "WSResponse" )==1)
                {
                    bDecoded=true;
                    System.out.println("DED packet decoded - now validate");

                    if(!strMethod.equals("JavaConnect")) bDecoded=false;
                }
                else
                {
                    // unknown method
                    System.out.println("DED packet could NOT be decoded - unknown Method: "+strMethod);
                }
            }
        }
        else
        {
            bDecoded=false;
        }

        if(bDecoded)
            Result="dops.connected.status.ok";
        return Result;
    }

    private static boolean isEnvironmentOK() {
        boolean bResult=true;
        try {
            System.out.println("Check if backend is running...");
            String path = new File(".").getCanonicalPath();
            path = trimOffLastFileSeperator(path, 3);
            if (!executeCommand("ping backend.scanva.com -c 1", path).contains("1 received")) {
                System.out.println("Waiting for VM to start ...");
                assertEquals(true, executeCommand("vagrant --version", path).contains("Vagrant"));
                assertEquals(true, containsAny(executeCommand("vagrant up backend", path), new String[]{"VM is already running", "Machine booted and ready"}));
                if (!executeCommand("ping backend.scanva.com -c 1", path).contains("1 received"))
                    throw new Exception("backend.scanva.com - NOT responding to ping");
                else
                    System.out.println("VM is started - Integration Environment ready");
            }
            else
                System.out.println("backend.scanva.com is running - Integration Environment ready");

        } catch (Exception e) {
            bResult=false;
            e.printStackTrace();
        }
        return bResult;
    }


   private static ByteBuffer prepareDataToSend() {
       short trans_id = 69;
       String uniqueId = "985998707DF048B2A796B44C89345494";
       String username = "johndoe@email.com"; // TODO: find a way to safely handle retrieval of username,password - should NOT be stored in source code
       String password = "12345";

       ByteBuffer data = createDEDforDOPSJavaConnect(trans_id, uniqueId, username, password);

       return data;
   }

    private static ByteBuffer createDEDforDOPSJavaConnect(short trans_id, String uniqueId, String username, String password)
    {
        DEDEncoder DED = new DEDEncoder();
        DED.PUT_STRUCT_START ( "WSRequest" );
        DED.PUT_METHOD   ( "Method",  "JavaConnect" );
        DED.PUT_USHORT   ( "TransID",  trans_id);
        DED.PUT_STDSTRING( "protocolTypeID", "DED1.00.00");
        DED.PUT_STDSTRING( "functionName", uniqueId );
        DED.PUT_STDSTRING( "username", username );
        DED.PUT_STDSTRING( "password", password );
        DED.PUT_STRUCT_END( "WSRequest" );

        ByteBuffer data = DED.GET_ENCODED_BYTEBUFFER_DATA();
        return data;
    }

    private static boolean containsAny(String str, String[] words)
    {
        boolean bResult=false; // if any of the words are found, this will be set true
        //String[] words = {"word1", "word2", "word3", "word4", "word5"};

        List<String> list = Arrays.asList(words);
        for (String word: list ) {
            boolean bFound = str.contains(word);
            if (bFound) {bResult=bFound; break;}
        }
        return bResult;
    }

    private static String executeCommand(String command, String path) {

        StringBuffer output = new StringBuffer();
        File dir = new File(path);

        // create a process and execute cmd and currect environment
        try {
            Process process = Runtime.getRuntime().exec(command, null, dir);

            BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            process.waitFor();

            String line = "";
            while ((line = reader.readLine())!= null) {
                output.append(line + "\n");
            }

        } catch (Exception e) {
            e.printStackTrace();
            output.append(e.getMessage());
        }

        return output.toString();
    }

    private static String trimOffLastFileSeperator(String path, int amountToTrimOff)
    {
        String separator = System.getProperty("file.separator");

        String strtmp = path;
        int lastSeparatorIndex;
        for(int i=amountToTrimOff; i>0; i--)
        {
            lastSeparatorIndex = strtmp.lastIndexOf(separator);
            strtmp = strtmp.substring(0,lastSeparatorIndex);
        }
        return strtmp;
    }
}
