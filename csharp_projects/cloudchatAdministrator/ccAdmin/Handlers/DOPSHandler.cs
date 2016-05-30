using System;
using System.Collections.Generic;
using WebSocketClient;
using DED;
using System.Threading;

namespace csharpServices
{
	public class DOPSHandler
	{
		private Client.wshandles _handles;
		private short trans_id;
		private String uniqueId;
		private bool bConnected = false;

		public bool connectToDOPsServer()
		{
			bool bResult = false;
			if (connectToDOPsServer("ws://backend.scanva.com:7777")) {
				bConnected = true;
				sendRequestToServer(createLoginRequest());
				switch (handleServerReply(waitForServerReply())) {
				case "Request accepted":
					bResult = true;
					break;
				case "Request denied":
					break;
				}
			}
			return bResult;
		}

		private bool connectToDOPsServer(string url)
		{
			bool bResult = false;
			_handles = Client.WSConnect ("ws://backend.scanva.com:7777");
			if(_handles != null) {
				if(_handles.webSocket.State == System.Net.WebSockets.WebSocketState.Connecting) {
					Thread.Sleep(2000); // wait a little time
					if(_handles.webSocket.State == System.Net.WebSockets.WebSocketState.Connecting) {
						// Something is wrong - disconnect to try again later
						disconnectFromDOPsServer();
						bResult = false;
					}
					else
						bResult = true;
				} else
					bResult = true;
			}

			return bResult;
		}

		private void sendRequestToServer(byte[] request)
		{
			Client.SendBLOB (request, _handles.webSocket).Wait (); 
		}

		public void setUniqueID(string ID)
		{
			this.uniqueId = ID;
		}

		private byte[] createLoginRequest()
		{
			/**
         	* prepare data for the request
         	*/
			trans_id = 69;
			if(uniqueId == "")
				uniqueId = "985998707DF048B2A796B44C89345494";

			String username = "johndoe@email.com"; // TODO: find a way to safely handle retrieval of username,password - should NOT be stored in source code
			String password = "12345";

			/**
         	* create DED request datapacket for DOPS for csharp clients
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

			/**
			 * return the constructed request
			 */ 
			byte[] byteArray = DED.GET_ENCODED_BYTEARRAY_DATA ();
			return byteArray;
		}

		private byte[] waitForServerReply()
		{
			byte[] receivedData = null;
			receivedData = Client.FetchReceived (_handles);
			return receivedData;
		}

		/**
		 * handleServerReply 
		 * 
		 * return values :
		 * 	"Request accepted"
		 * 	"Request denied"
		 */
		private string handleServerReply(byte[] receivedData)
		{
			string returnResult = "";

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
					if (strDest.Equals (uniqueId) && strSrc.Equals ("DFD_1.1")) {
						Console.WriteLine ("DED packet validated - OK");
						returnResult = "Request accepted";
					} else {
						bDecoded = false;
						Console.WriteLine ("DED packet validated - FAILED - DED was NOT a valid DFDResponse");
						returnResult = "Request denied";
					}
				}
			} else {
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
					if (!strFunctionName.Equals (uniqueId))
						bDecoded = false;
					if (!strStatus.Equals ("ACCEPTED"))
						bDecoded = false;
					if (!strProtocolTypeID.Equals ("DED1.00.00"))
						bDecoded = false;

					if (bDecoded) {
						Console.WriteLine ("DED packet validated - OK");
						returnResult = "Request accepted";
					}
				} else {
					bDecoded = false;
					Console.WriteLine ("DED packet validated - FAILED - DED was NOT a valid WSResponse");
					returnResult = "Request denied";
				}
			}

			return returnResult;
		}

		public bool isConnected()
		{
			return bConnected;
		}
		
		public void disconnectFromDOPsServer()
		{
			// Disconnect from server
			Client.WSDisconnect (_handles);
			bConnected = false;
		}



	}

}