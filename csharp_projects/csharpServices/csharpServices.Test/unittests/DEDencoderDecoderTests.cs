﻿using System;
using NUnit.Framework;
using DED;
using WebSocketClient;

// all test namespaces start with "MonoTests."  Append the Namespace that
// contains the class you are testing, e.g. MonoTests.System.Collections
using System.Threading;
using System.Threading.Tasks;
using System.Net.WebSockets;


namespace DED.UnitTests
{

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
			byte[] ReceiveBuffer = null;

			// connect to DOPs Server
			Client.wshandles _handles = Client.WSConnect ("ws://localhost:8046/websockets/MockServerEndpoint");

			// setup DED packet to send to Server
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

			// Send the DED packet to Server
			Client.SendBLOB (byteArray, _handles.webSocket).Wait(); // NB! Not really necessary to add Wait() in this case since FetchReceived() will wait until data is ready

			// Fetch the return data from the Server
			ReceiveBuffer = Client.FetchReceived (_handles);

			// Decode the DED
			DEDDecoder DED2 = DEDDecoder.DED_START_DECODER ();
			DED2.PUT_DATA_IN_DECODER (ReceiveBuffer, ReceiveBuffer.Length);
			if ((DED2.GET_STRUCT_START ("event")).Equals (1) &&
				(DED2.GET_METHOD ("Method")).Contains ("MusicPlayer") &&
				(DED2.GET_USHORT ("trans_id")).Equals (trans_id) &&
				(DED2.GET_BOOL ("startstop")).Equals (bAction) &&
			(DED2.GET_STRUCT_END ("event")).Equals (1)) 
			{
				Console.WriteLine ("SUCCESS - unittest received from mockDOPsServer a DED blob, and decoded it");
			} 
			else
			{
				Console.WriteLine ("FAILURE - unittest could NOT decode DED blob");
			}

			// Disconnect from server
			Client.WSDisconnect(_handles);
		}

		[Test]
		public void SendReceiveDEDMockDOPsServer2()
		{
			byte[] ReceiveBuffer = null;

			// connect to DOPs Server
			Client.wshandles _handles = Client.WSConnect ("ws://localhost:8046/websockets/MockServerEndpoint");

			// setup DED packet to send to Server
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

			// Send the DED packet to Server
			Client.SendBLOB (byteArray, _handles.webSocket).Wait(); // NB! Not really necessary to add Wait() in this case since FetchReceived() will wait until data is ready

			// Fetch the return data from the Server
			ReceiveBuffer = Client.FetchReceived (_handles);

			// Decode the DED
			DEDDecoder DED2 = DEDDecoder.DED_START_DECODER ();
			DED2.PUT_DATA_IN_DECODER (ReceiveBuffer, ReceiveBuffer.Length);
			if ((DED2.GET_STRUCT_START ("event")).Equals (1) &&
				(DED2.GET_METHOD ("Method")).Contains ("MusicPlayer") &&
				(DED2.GET_USHORT ("trans_id")).Equals (trans_id) &&
				(DED2.GET_BOOL ("startstop")).Equals (bAction) &&
			(DED2.GET_STRUCT_END ("event")).Equals (1)) 
			{
				Console.WriteLine ("SUCCESS - unittest received from mockDOPsServer a DED blob, and decoded it");
			} 
			else
			{
				Console.WriteLine ("FAILURE - unittest could NOT decode DED blob");
			}

			// Disconnect from server
			Client.WSDisconnect(_handles);
		}


//		[Test]
//		public void loginToDOPsServer ()
//		{
//			Assert.IsTrue(mockTestServer.isOpen());
//
//	        /**
//	         * prepare data to be send
//	         */
//	        short trans_id = 69;
//	        bool action = true;
//	        String uniqueId = "985998707DF048B2A796B44C89345494";
//	        String username = "johndoe@email.com"; // TODO: find a way to safely handle retrieval of username,password - should NOT be stored in source code
//	        String password = "12345";
//
//	        /**
//	         * create DED connect datapacket for DOPS for java clients
//	         */
//	        DEDEncoder DED = new DEDEncoder();
//	        DED.PUT_STRUCT_START ( "WSRequest" );
//	            DED.PUT_METHOD   ( "name",  "JavaConnect" );
//	            DED.PUT_USHORT   ( "trans_id",  trans_id);
//	            DED.PUT_STDSTRING( "protocolTypeID", "DED1.00.00");
//	            DED.PUT_STDSTRING( "functionName", uniqueId );
//	            DED.PUT_STDSTRING( "username", username );
//	            DED.PUT_STDSTRING( "password", password );
//	        DED.PUT_STRUCT_END( "WSRequest" );
//
//	        byte[] data = DED.GET_ENCODED_BYTEARRAY_DATA();
//
//	        /**
//	         * send to server with client current client session connection
//	         */
//	        mockTestServer.sendToServer(data);
//
//
//	        /**
//	         * wait for incomming data response, then receive data - or timeout
//	         */
//	        byte[] receivedData = mockTestServer.receiveFromServer();
//
//
//	        /**
//	         * decode incomming data
//	         */
//	        boolean bDecoded=false;
//	        String strMethod="";
//	        String strProtocolTypeID="";
//	        String strFunctionName="";
//	        String strStatus="";
//	        short uTrans_id=0;
//
//	        // decode data ...
//	        DEDDecoder DED2 = new DEDDecoder();
//	        DED2.PUT_DATA_IN_DECODER( receivedData, receivedData.length);
//	        if( DED2.GET_STRUCT_START( "WSResponse" )==1 &&
//	                (strMethod   = DED2.GET_METHOD ( "name" )).length()>0 &&
//	                (uTrans_id     = DED2.GET_USHORT ( "trans_id")) !=-1 &&
//	                (strProtocolTypeID  = DED2.GET_STDSTRING ( "protocolTypeID")).length()>0 &&
//	                (strFunctionName    = DED2.GET_STDSTRING ( "functionName")).length()>0 &&
//	                (strStatus  = DED2.GET_STDSTRING ( "status")).length()>0 &&
//	            DED2.GET_STRUCT_END( "WSResponse" )==1)
//	        {
//	            bDecoded=true;
//	            System.out.println("DED packet decoded - now validate");
//
//	            if(!strMethod.equals("JavaConnect")) bDecoded=false;
//	            if(uTrans_id != trans_id) bDecoded=false;
//	            assertEquals(true,bDecoded);
//	            if(!strFunctionName.equals(uniqueId)) bDecoded=false;
//	            assertEquals(true,bDecoded);
//	            if(!strStatus.equals("ACCEPTED")) bDecoded=false;
//	            assertEquals(true,bDecoded);
//	            if(!strProtocolTypeID.equals("DED1.00.00")) bDecoded=false;
//
//	            if(bDecoded)
//	                System.out.println("DED packet validated - OK");
//
//	        }
//	        else
//	        {
//	            bDecoded=false;
//	        }
//
//	        assertEquals(true,bDecoded);
//	 
//		}
//	
	}
}
