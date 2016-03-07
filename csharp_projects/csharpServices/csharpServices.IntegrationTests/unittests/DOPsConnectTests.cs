using System;
using NUnit.Framework;
using DED;
using WebSocketClient;
using System.Threading;
using System.Threading.Tasks;
using System.Net.WebSockets;

namespace Fleck.IntegrationTests
{
	[TestFixture]
	public class DOPsWebSocketConnectionTests
	{

		[SetUp]
		public void Setup()
		{
		}

		[Test]
		public void connectToDOPsServer()
		{
			// connect to DOPs Server
//			Client.wshandles _handles = Client.WSConnect ("ws://backend.scanva.com:7777");
			Client.wshandles _handles = Client.WSConnect ("ws://127.0.0.1:7778");

			/**
	         * prepare data to be send
	         */
	        short trans_id = 69;
	        bool action = true;
	        String uniqueId = "985998707DF048B2A796B44C89345494";
	        String username = "johndoe@email.com"; // TODO: find a way to safely handle retrieval of username,password - should NOT be stored in source code
	        String password = "12345";

	        /**
	         * create DED connect datapacket for DOPS for java clients
	         */
	        DEDEncoder DED = new DEDEncoder();
	        DED.PUT_STRUCT_START ( "WSRequest" );
		//		DED.PUT_METHOD   ( "Method",  "CSharpConnect" );
				DED.PUT_METHOD   ( "Method",  "JavaConnect" );
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
