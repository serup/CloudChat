using System;
using NUnit.Framework;
using DED;
using WebSocketClient;
//using System.Threading;
//using System.Threading.Tasks;
//using System.Net.WebSockets;
using csharpServices;

namespace DOPSTests
{
	[TestFixture]
	public class DOPsWebSocketConnectionTests
	{

		[SetUp]
		public void Setup()
		{
		}


		[Test]
		public void testDOPsHandler()
		{
			DOPSHandler dopsHandler = new DOPSHandler();
			Assert.IsTrue (dopsHandler.connectToDOPsServer());
			dopsHandler.disconnectFromDOPsServer();
		}

		[Test]
		public void connectToDOPsServer()
		{
			Client nClient = new Client();
			Assert.IsTrue(nClient.Connect("ws://backend.scanva.com:7777"));

			/**
	         	* prepare data to be send
	         	*/
			short trans_id = 69;
			//bool action = true;
			String uniqueId = "985998707DF048B2A796B44C89345494";
			String username = "johndoe@email.com"; // TODO: find a way to safely handle retrieval of username,password - should NOT be stored in source code
			String password = "12345";

			/**
	        	* create DED connect datapacket for DOPS for java clients
	         	*/
			DEDEncoder DED = new DEDEncoder ();
			DED.PUT_STRUCT_START ("WSRequest");
			DED.PUT_METHOD ("Method", "CSharpConnect");
			DED.PUT_USHORT ("TransID", trans_id);
			DED.PUT_STDSTRING ("protocolTypeID", "DED1.00.00");
			DED.PUT_STDSTRING ("functionName", uniqueId);
			DED.PUT_STDSTRING ("username", username);
			DED.PUT_STDSTRING ("password", password);
			DED.PUT_STRUCT_END ("WSRequest");

			byte[] byteArray = DED.GET_ENCODED_BYTEARRAY_DATA ();
			Assert.IsNotNull (byteArray);

			// Send the DED packet to Server
			nClient.SendBLOB (byteArray);

			// Fetch the return data from the Server
			byte[] receivedData = null;
			receivedData = nClient.FetchReceived (true);

			/**
	         	* decode incomming data
	         	*/
			bool bDecoded = false;
			String strMethod = "";
			String strProtocolTypeID = "";
			String strFunctionName = "";
			String strStatus = "";
			String strDest;
			String strSrc;
			short uTrans_id = 0;

			// decode data ...
			DEDDecoder DED2 = new DEDDecoder ();
			DED2.PUT_DATA_IN_DECODER (receivedData, receivedData.Length);
			if (DED2.GET_STRUCT_START ("WSResponse") == 1 &&
				(strMethod = DED2.GET_METHOD ("Method")).Equals ("1_1_6_LoginProfile")) {
				// DFD was online and responded - now validate the response
				if ((uTrans_id = DED2.GET_USHORT ("TransID")) != -1 &&
					(strProtocolTypeID = DED2.GET_STDSTRING ("protocolTypeID")).Length > 0 &&
					(strDest = DED2.GET_STDSTRING ("dest")).Length > 0 &&
					(strSrc = DED2.GET_STDSTRING ("src")).Equals ("DFD_1.1") &&
					(strStatus = DED2.GET_STDSTRING ("status")).Length > 0 &&
					DED2.GET_STRUCT_END ("DFDResponse") == 1) {
					if (strDest.Equals (uniqueId) && strSrc.Equals ("DFD_1.1"))
						Console.WriteLine ("DED packet validated - OK");
					else {
						bDecoded = false;
						Console.WriteLine ("DED packet validated - FAILED - DED was NOT a valid DFDResponse");
						Assert.IsTrue (bDecoded);
					}
				}
			} 
			else 
			{
				if ((uTrans_id = DED2.GET_USHORT ("TransID")) != -1 &&
					(strProtocolTypeID = DED2.GET_STDSTRING ("protocolTypeID")).Length > 0 &&
					(strFunctionName = DED2.GET_STDSTRING ("functionName")).Length > 0 &&
					(strStatus = DED2.GET_STDSTRING ("status")).Length > 0 &&
					DED2.GET_STRUCT_END ("WSResponse") == 1) {
					bDecoded = true;
					Console.WriteLine ("DED packet decoded - now validate");

					if (!strMethod.Equals ("CSharpConnect"))
						bDecoded = false;
					if (uTrans_id != trans_id)
						bDecoded = false;
					Assert.IsTrue (bDecoded);
					if (!strFunctionName.Equals (uniqueId))
						bDecoded = false;
					Assert.IsTrue (bDecoded);
					if (!strStatus.Equals ("ACCEPTED"))
						bDecoded = false;
					Assert.IsTrue (bDecoded);
					if (!strProtocolTypeID.Equals ("DED1.00.00"))
						bDecoded = false;

					if (bDecoded)
						Console.WriteLine ("DED packet validated - OK");
				} else {
					bDecoded = false;
					Console.WriteLine ("DED packet validated - FAILED - DED was NOT a valid WSResponse");
					Assert.IsTrue (bDecoded);
				}
			}

			// Send the DED packet to Server
			nClient.SendBLOB (byteArray);

			// Fetch the return data from the Server
			receivedData = null;
			receivedData = nClient.FetchReceived ();

			nClient.stopReceivingTask();

			// Gracefully close socket
			Console.WriteLine("Close Client connection");
			nClient.Dispose();
		}
	}
}
