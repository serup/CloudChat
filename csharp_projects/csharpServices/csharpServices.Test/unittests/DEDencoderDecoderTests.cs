using System;
using NUnit.Framework;
using DED;
using WebSocketClient;
using csharpServices;
using System.Collections;
using System.IO;
using System.Text;

// all test namespaces start with "MonoTests."  Append the Namespace that
// contains the class you are testing, e.g. MonoTests.System.Collections
using System.Threading;
using System.Threading.Tasks;
using System.Net.WebSockets;


namespace DED.UnitTests
{
	[TestFixture]
	public class lzssTests : Assert {

		// this method is run before each [Test] method is called. You can put
		// variable initialization, etc. here that is common to each test.
		// Just leave the method empty if you don't need to use it.
		// The name of the method does not matter; the attribute does.
		[SetUp]
		public void GetReady() {}

		// this method is run after each Test* method is called. You can put
		// clean-up code, etc. here.  Whatever needs to be done after each test.
		// Just leave the method empty if you don't need to use it.
		// The name of the method does not matter; the attribute does.
		[TearDown]
		public void Clean() {}

		[Test]
		public void lzssCompression() {
			String originaltxt = "ATATAAAFFFF";
			InputStream input = new ByteArrayInputStream(UTF8Encoding.ASCII.GetBytes(originaltxt.ToCharArray()));
			LZSS lzssCompress = new LZSS(input);
			ByteArrayOutputStream _out = lzssCompress.compress();
			Assert.IsNotNull(_out);
		}

		[Test]
		public void lzssDeCompression() {
			String originaltxt = "ATATAAAFFFF";
			InputStream input = new ByteArrayInputStream(UTF8Encoding.ASCII.GetBytes(originaltxt.ToCharArray()));
			LZSS lzssCompress = new LZSS(input);
			ByteArrayOutputStream _out = lzssCompress.compress();
			Assert.IsNotNull(_out);

			//... 

			InputStream data = new ByteArrayInputStream(_out.toByteArray());
			LZSS lzssDecompress = new LZSS(data);
			ByteArrayOutputStream result = lzssDecompress.uncompress();
			Assert.IsNotNull(result);


			String decodedTxt = result.toString();
			Assert.IsTrue((originaltxt == decodedTxt));


		}

	}

	[TestFixture]
	public class EncodeDecodeTest : Assert {

		// this method is run before each [Test] method is called. You can put
		// variable initialization, etc. here that is common to each test.
		// Just leave the method empty if you don't need to use it.
		// The name of the method does not matter; the attribute does.
		[SetUp]
		public void GetReady() {}

		// this method is run after each Test* method is called. You can put
		// clean-up code, etc. here.  Whatever needs to be done after each test.
		// Just leave the method empty if you don't need to use it.
		// The name of the method does not matter; the attribute does.
		[TearDown]
		public void Clean() {}

		[Test]
		public void EncoderInstantiation() {
			DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
			Assert.IsNotNull(DED);
		}

		[Test]
		public void StartEncoder() {
			DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
			DED.PUT_STRUCT_START ("event");
			Assert.IsNotNull(DED);
		}

		[Test]
		public void PUT_METHOD() {
			DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
			DED.PUT_STRUCT_START ("event");
				DED.PUT_METHOD ("Method", "MusicPlayer");
			Assert.IsNotNull(DED);
		}

		[Test]
		public void PUT_USHORT() {
			short trans_id = 1;
			DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
			DED.PUT_STRUCT_START ("event");
				DED.PUT_METHOD ("Method", "MusicPlayer");
				DED.PUT_USHORT ("trans_id", trans_id);
			Assert.IsNotNull(DED);
		}

		[Test]
		public void PUT_BOOL() {
			short trans_id = 1;
			bool bAction = true;
			DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
			DED.PUT_STRUCT_START ("event");
				DED.PUT_METHOD ("Method", "MusicPlayer");
				DED.PUT_USHORT ("trans_id", trans_id);
				DED.PUT_BOOL ("startstop", bAction);
			Assert.IsNotNull(DED);
		}

		[Test]
		public void EndEncoder() {
			short trans_id = 1;
			bool bAction = true;
			DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
			DED.PUT_STRUCT_START ("event");
				DED.PUT_METHOD ("Method", "MusicPlayer");
				DED.PUT_USHORT ("trans_id", trans_id);
				DED.PUT_BOOL ("startstop", bAction);
			DED.PUT_STRUCT_END ("event");
			Assert.IsNotNull(DED);
		}

		[Test]
		public void GET_ENCODED_DATA() {
			short trans_id = 1;
			bool bAction = true;
			DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
			Assert.IsNotNull(DED);
			DED.PUT_STRUCT_START ("event");
				DED.PUT_METHOD ("Method", "MusicPlayer");
				DED.PUT_USHORT ("trans_id", trans_id);
				DED.PUT_BOOL ("startstop", bAction);
			DED.PUT_STRUCT_END ("event");

			byte[] byteArray = DED.GET_ENCODED_BYTEARRAY_DATA ();
			Assert.IsNotNull (byteArray);
		}

		[Test]
		public void PUT_DATA_IN_DECODER() {
			short trans_id = 1;
			bool bAction = true;
			DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
			Assert.IsNotNull(DED);
			DED.PUT_STRUCT_START ("event");
				DED.PUT_METHOD ("Method", "MusicPlayer");
				DED.PUT_USHORT ("trans_id", trans_id);
				DED.PUT_BOOL ("startstop", bAction);
			DED.PUT_STRUCT_END ("event");

			byte[] byteArray = DED.GET_ENCODED_BYTEARRAY_DATA ();
			Assert.IsNotNull (byteArray);

			//..transmitting :-) simulation
			//..receiving :-) simulation

			DEDDecoder DED2 = DEDDecoder.DED_START_DECODER();
			DED2.PUT_DATA_IN_DECODER (byteArray, byteArray.Length);

		}

		[Test]
		public void EncodeCompressDecompressDecode() {
			short trans_id = 1;
			bool bAction = true;
			DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
			Assert.IsNotNull(DED);
			DED.PUT_STRUCT_START ("event");
				DED.PUT_METHOD ("Method", "MusicPlayer");
				DED.PUT_USHORT ("trans_id", trans_id);
				DED.PUT_BOOL ("startstop", bAction);
				DED.PUT_STDSTRING( "text", "hello");
			DED.PUT_STRUCT_END ("event");

			byte[] byteArray = DED.GET_ENCODED_BYTEARRAY_DATA ();
			Assert.IsNotNull (byteArray);

			//..transmitting :-) simulation
			//..receiving :-) simulation

			bool bDecoded = false;
			DEDDecoder DED2 = DEDDecoder.DED_START_DECODER();
			DED2.PUT_DATA_IN_DECODER (byteArray, byteArray.Length);
			if ((DED2.GET_STRUCT_START ("event")).Equals(1) &&
				(	DED2.GET_METHOD ("Method")).Contains("MusicPlayer") &&
				(	DED2.GET_USHORT ("trans_id")).Equals(trans_id) &&
				(	DED2.GET_BOOL   ("startstop")).Equals(bAction) &&
				(	DED2.GET_STDSTRING   ("text")).Contains("hello") &&
			(DED2.GET_STRUCT_END("event")).Equals(1)) 
			{
				bDecoded = true;
			}
			else
				bDecoded = false;

			Assert.IsTrue (bDecoded);
		}


	}

	[TestFixture]
	public class mockDOPsServerTest : Assert
	{
		MockDOPsServer mockDOPsServer = null;
		Client.wshandles _handles = null;

		// this method is run before first [Test] method is called. You can put
		// variable initialization, etc. here that is common to all tests.
		// Just leave the method empty if you don't need to use it.
		// The name of the method does not matter; the attribute does.
		[TestFixtureSetUp]
		public void Initialize() 
		{
			// instantiate mockDOPsServer
			mockDOPsServer = new MockDOPsServer (8046,"/websockets/MockServerEndpoint");
			// start mock DOPs Server
			mockDOPsServer.Start();
		}

		[SetUp]
		public void GetReady()
		{
			// connect to DOPs Server
			//_handles = Client.WSConnect ("ws://localhost:8046/websockets/MockServerEndpoint");
		}

		// this method is run after each Test* method is called. You can put
		// clean-up code, etc. here.  Whatever needs to be done after each test.
		// Just leave the method empty if you don't need to use it.
		// The name of the method does not matter; the attribute does.
		[TearDown]
		public void Clean() 
		{
			// Disconnect from server
			//Client.WSDisconnect(_handles);
		}

		[TestFixtureTearDown]
		public void Close ()
		{
			// wait for release
			mockDOPsServer.WaitForStop (0); 
		}

		[Test]
		public void SendReceiveDEDMockDOPsServer()
		{
			// connect to DOPs Server
			Client.wshandles _handles = Client.WSConnect ("ws://localhost:8046/websockets/MockServerEndpoint");

			// setup DED packet to send to Server
			short trans_id = 1;
			bool bAction = true;
			DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
			Assert.IsNotNull(DED);
			DED.PUT_STRUCT_START ("event"); // Start struct is NOT validated in server
				DED.PUT_METHOD ("Method", "MusicPlayer");
				DED.PUT_USHORT ("trans_id", trans_id);
				DED.PUT_BOOL ("startstop", bAction);
			DED.PUT_STRUCT_END ("event");

			byte[] byteArray = DED.GET_ENCODED_BYTEARRAY_DATA ();
			Assert.IsNotNull (byteArray);

			// Send the DED packet to Server
			Client.SendBLOB (byteArray, _handles.webSocket).Wait(); // NB! Not really necessary to add Wait() in this case since FetchReceived() will wait until data is ready

			// Fetch the return data from the Server
			byte[] ReceiveBuffer = null;
			ReceiveBuffer = Client.FetchReceived (_handles);

			/**
	         * decode incomming data
	         */
	        bool bDecoded=false;
	        String strMethod="";
	        String strProtocolTypeID="";
	        String strFunctionName="";
	        String strStatus="";
	        short uTrans_id=0;

	        // decode data ...
	        DEDDecoder DED2 = new DEDDecoder();
			DED2.PUT_DATA_IN_DECODER( ReceiveBuffer, ReceiveBuffer.Length);
	        if( DED2.GET_STRUCT_START( "WSResponse" )==1 && 
	                (strMethod   		= DED2.GET_METHOD ( "name" )).Contains("MusicPlayer") && // this means that ded was echo'd back
	                (uTrans_id     		= DED2.GET_USHORT ( "trans_id")) == -1 &&  // server does NOT know this dedpacket, thus not capable of decoding its trans_id, hence the -1
	                (strProtocolTypeID  = DED2.GET_STDSTRING ( "protocolTypeID")).Length>0 &&
	                (strFunctionName    = DED2.GET_STDSTRING ( "functionName")).Length>0 &&
	                (strStatus  = DED2.GET_STDSTRING ( "status")).Length>0 &&
	            DED2.GET_STRUCT_END( "WSResponse" )==1)
			{
				Console.WriteLine ("SUCCESS - unittest received from mockDOPsServer a DED blob, and decoded it");
				bDecoded=true;
			} 
			else
			{
				Console.WriteLine ("FAILURE - unittest could NOT decode DED blob");
				Assert.IsTrue(bDecoded);
			}

			// Disconnect from server
			Client.WSDisconnect(_handles);
		}

		[Test]
		public void loginToMockDOPsServer()
		{
			// connect to DOPs Server
			Client.wshandles _handles = Client.WSConnect ("ws://localhost:8046/websockets/MockServerEndpoint");

			/**
	         * prepare data to be send
	         */
	        short trans_id = 69;
	        bool action = true;
	        String uniqueId = "985998707DF048B2A796B44C89345494";
	        String username = "johndoe@email.com"; // TODO: find a way to safely handle retrieval of username,password - should NOT be stored in source code
	        String password = "12345";

	        /**
	         * create DED connect datapacket for DOPS for csharp clients
	         */
	        DEDEncoder DED = new DEDEncoder();
	        DED.PUT_STRUCT_START ( "WSRequest" );
	            DED.PUT_METHOD   ( "name",  "CSharpConnect" );
	            DED.PUT_USHORT   ( "trans_id",  trans_id);
	            DED.PUT_STDSTRING( "protocolTypeID", "DED1.00.00");
	            DED.PUT_STDSTRING( "functionName", uniqueId );
				DED.PUT_STDSTRING( "username", username );
	            DED.PUT_STDSTRING( "password", password );
	        DED.PUT_STRUCT_END( "WSRequest" );

			byte[] byteArray = DED.GET_ENCODED_BYTEARRAY_DATA ();
			Assert.IsNotNull (byteArray);

			// Send the DED packet to Server
			Client.SendBLOB (byteArray, _handles.webSocket).Wait(); // NB! Not really necessary to add Wait() in this case since FetchReceived() will wait until data is ready

			// Fetch the return data from the Server
			byte[] receivedData = null;
			receivedData = Client.FetchReceived (_handles);

	        /**
	         * decode incomming data
	         */
	        bool bDecoded=false;
	        String strMethod="";
	        String strProtocolTypeID="";
	        String strFunctionName="";
	        String strStatus="";
	        short uTrans_id=0;

	        // decode data ...
	        DEDDecoder DED2 = new DEDDecoder();
	        DED2.PUT_DATA_IN_DECODER( receivedData, receivedData.Length);
	        if( DED2.GET_STRUCT_START( "WSResponse" )==1 &&
	                (strMethod   		= DED2.GET_METHOD ( "name" )).Length>0 &&
	                (uTrans_id     		= DED2.GET_USHORT ( "trans_id")) != -1 &&
	                (strProtocolTypeID  = DED2.GET_STDSTRING ( "protocolTypeID")).Length>0 &&
	                (strFunctionName    = DED2.GET_STDSTRING ( "functionName")).Length>0 &&
	                (strStatus  = DED2.GET_STDSTRING ( "status")).Length>0 &&
	            DED2.GET_STRUCT_END( "WSResponse" )==1)
	        {
	            bDecoded=true;
	            Console.WriteLine("DED packet decoded - now validate");

	            if(!strMethod.Equals("CSharpConnect")) bDecoded=false;
	            if(uTrans_id != trans_id) bDecoded=false;
				Assert.IsTrue(bDecoded);
	            if(!strFunctionName.Equals(uniqueId)) bDecoded=false;
				Assert.IsTrue(bDecoded);
	            if(!strStatus.Equals("ACCEPTED")) bDecoded=false;
				Assert.IsTrue(bDecoded);
	            if(!strProtocolTypeID.Equals("DED1.00.00")) bDecoded=false;

	            if(bDecoded)
					Console.WriteLine("DED packet validated - OK");
	        }
	        else
	        {
	            bDecoded=false;
				Console.WriteLine("DED packet validated - FAILED - DED was NOT a valid WSResponse");
				Assert.IsTrue(bDecoded);
	        }

			// Disconnect from server
			Client.WSDisconnect(_handles);
		}
	}
}
