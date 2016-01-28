
using System.Net.Sockets;
using System.Net;
using System;
using Fleck;
using System.Linq;
using System.Threading;
using DED;

class handleDOPsProtocol
{
	// Decoding of DED packets with regards to mock DOPs Server
	public byte[] ReceiveDEDandResponse(byte[] dedpacket)
	{
		bool bDecoded = false;
		string strMethod="", strProtocolTypeID="", strUsername="", strPassword="", strFunctionName="", strStatus="";
		short uTrans_id=0;
		// 1. decode login packet
		DEDDecoder DED = new DEDDecoder();
		DED.PUT_DATA_IN_DECODER( dedpacket, dedpacket.Length);
		if( DED.GET_STRUCT_START( "WSRequest" )==1 &&
				(strMethod = DED.GET_METHOD ( "name" )).Length>0 &&
				(uTrans_id = DED.GET_USHORT ( "trans_id")) !=-1 &&
				(DED.GET_STDSTRING ( "protocolTypeID")).Length>0 &&
				(DED.GET_STDSTRING ( "functionName")).Length>0 &&
				(DED.GET_STDSTRING ( "username")).Length>0 &&
				(DED.GET_STDSTRING ( "password")).Length>0 &&
			DED.GET_STRUCT_END( "WSRequest" )==1)
		{
			bDecoded=true;

			// expected values
			short trans_id = 69;
			String uniqueId = "985998707DF048B2A796B44C89345494";
			String username = "johndoe@email.com";
			String password = "12345";

			if(!strMethod.Equals("CSharpConnect")) bDecoded=false;
			if(uTrans_id != trans_id) bDecoded=false;
			if(!strFunctionName.Equals(uniqueId)) bDecoded=false;
			if(!strProtocolTypeID.Equals("DED1.00.00")) bDecoded=false;
			if(!strUsername.Equals(username)) bDecoded=false;
			if(!strPassword.Equals(password)) bDecoded=false;
		}
		else
		{
			if( DED.GET_STRUCT_START( "DFDRequest" )==1 &&
				(strMethod          = DED.GET_METHOD ( "Method" )).Length>0 &&
				(uTrans_id          = DED.GET_USHORT ( "TransID")) !=-1 &&
				(strProtocolTypeID  = DED.GET_STDSTRING ( "protocolTypeID")).Length>0 )
			{
				Console.WriteLine("DFDRequest - received - now parse");
				String strClientSrc="<unknown>";
				if((DED.GET_STDSTRING("dest")).Contains("DFD_1.1") &&
					(strClientSrc = DED.GET_STDSTRING("src")).Length>0 &&
					(DED.GET_STDSTRING("STARTrequest")).Contains("EmployeeRequest") &&
					(DED.GET_STDSTRING("STARTrecord")).Contains("record") )
				{
					Console.WriteLine("- EmployeeRequest - received - now parse");
					String strProfileID = "", strProfileName = "", strSizeOfProfileData = "", strProfile_chunk_id = "", strAccountStatus = "", strExpireDate = "", strProfileStatus = "";
					if((strProfileID 			= DED.GET_STDSTRING("profileID")).Length>0 &&
						(strProfileName 		= DED.GET_STDSTRING("profileName")).Length>0 &&
						(strProtocolTypeID 		= DED.GET_STDSTRING("protocolTypeID")).Length>0 &&
						(strSizeOfProfileData 	= DED.GET_STDSTRING("sizeofProfileData")).Length>0 &&
						(strProfile_chunk_id 	= DED.GET_STDSTRING("profile_chunk_id")).Length>0 &&
						(strAccountStatus 		= DED.GET_STDSTRING("AccountStatus")).Length>0 &&
						(strExpireDate 			= DED.GET_STDSTRING("ExpireDate")).Length>0 &&
						(strProfileStatus 		= DED.GET_STDSTRING("ProfileStatus")).Length>0)
					{
						Console.WriteLine("-- Employee record received - now validate TOAST ");
						if((DED.GET_STDSTRING("STARTtoast")).Length>0 )
						{
							// TOAST area found, now iterate thru all elements
							Console.WriteLine("--- TOAST area found, now iterate thru all elements");
							DED._Elements elementvalue = null;
							while((elementvalue = DED.GET_ELEMENT("profile"))!=null)
							{
								Console.WriteLine("--- TOAST element : " + elementvalue.strElementID);
							}
							if((DED.GET_STDSTRING("elements-ignore").Length<=0) &&
								(DED.GET_STDSTRING("ENDrecord")).Contains("record") &&
								(DED.GET_STDSTRING("ENDrequest")).Contains("EmployeeRequest") &&
								(DED.GET_STDSTRING("DFDRequest")).Length<=0)
							{
								Console.WriteLine("-- END Employee record");
								Console.WriteLine("- END EmployeeRequest");
								Console.WriteLine("END DFDRequest");
								Console.WriteLine("DFDRequest parsed correct");
								bDecoded=true;
							}

							// 2. determine what to respond
							if(bDecoded)
								strStatus="Profile Saved in database";
							else
								strStatus="Error in creating profile";

							// 3. create response packet
							DEDEncoder DED2 = new DEDEncoder();
							DED2.PUT_STRUCT_START( "DFDResponse" );
								DED2.PUT_METHOD   ( "Method", "CreateProfile" );
								DED2.PUT_USHORT   ( "TransID", uTrans_id);
								DED2.PUT_STDSTRING( "protocolTypeID", "DED1.00.00");
								DED2.PUT_STDSTRING( "dest", strClientSrc );
								DED2.PUT_STDSTRING( "src", "DFD_1.1" );
								DED2.PUT_STDSTRING( "status", strStatus );
							DED2.PUT_STRUCT_END( "DFDResponse" );

							byte[] _dedResponsePacket = DED2.GET_ENCODED_BYTEARRAY_DATA();

							// 4. send response packet
							if(_dedResponsePacket==null) {
								_dedResponsePacket = dedpacket; // echo back original packet, since creation of response packet went wrong!!
								Console.WriteLine("Internal ERROR [MockServer] - was not capable of creating a DED response packet, thus echoing received back");
							}
							return _dedResponsePacket;
						}
						else
						{
							// NO TOAST area found
							Console.WriteLine("No TOAST area found in request, meaning NO elements added to profile info");
						}
					}
				}
				else
				{
					Console.WriteLine("Warning - unknown DFDRequest - accepting basic parsing - header of packet was correct");
					bDecoded=true;
				}
			}
			if(!bDecoded)
				Console.WriteLine("WARNING [MockServer] - was not capable of decoding incoming DED datapacket - could be unknown DED method");
		}

		// 2. determine what to respond
		if(bDecoded)
			strStatus="ACCEPTED";
		else
			strStatus="NOT ACCEPTED USER";

		// 3. create response packet
		DEDEncoder DED3 = new DEDEncoder();
		DED3.PUT_STRUCT_START( "WSResponse" );
			DED3.PUT_METHOD   ( "name", strMethod );
			DED3.PUT_USHORT   ( "trans_id", uTrans_id);
			DED3.PUT_STDSTRING( "protocolTypeID", "DED1.00.00");
			DED3.PUT_STDSTRING( "functionName", strFunctionName );
			DED3.PUT_STDSTRING( "status", strStatus );
		DED3.PUT_STRUCT_END( "WSResponse" );

		byte[] dedResponsePacket = DED3.GET_ENCODED_BYTEARRAY_DATA();

		// 4. send response packet
		if(dedResponsePacket==null) {
			dedResponsePacket = dedpacket; // echo back original packet, since creation of response packet went wrong!!
			Console.WriteLine("Internal ERROR [MockServer] - was not capable of creating a DED response packet, thus echoing received back");
		}
		return dedResponsePacket;
	}

}

class MockDOPsServer {
	Worker workerObject;
	Thread workerThread;
	public volatile bool bDone;
	private static int _port;
	private static string _endpoint;

	public MockDOPsServer(int port)
	{
		_port = port;
	}

	public MockDOPsServer(int port, string endpoint)
	{
		_port = port;
		_endpoint = endpoint;
	}

	public void WaitSomeTime(int additioncycles)
	{
		int c = 0;
		while (!bDone) {
			Console.WriteLine ("busy..");
			c++;
			if (c > additioncycles) {
				StopDOPsServer (); bDone=true;
			}
		}
	}

	class Worker {
		private volatile bool _shouldStop;
		WebSocketServer server;
		System.Collections.Generic.List<IWebSocketConnection> allSockets;

		public void DoWork()
		{
			allSockets = new System.Collections.Generic.List<IWebSocketConnection>();
			server = new WebSocketServer("ws://127.0.0.1:" + _port.ToString() + _endpoint.ToString()); // NB! running as mockserver in java spring project
			server.Start(socket =>
				{
					socket.OnOpen = () =>
					{
						Console.WriteLine("mockDOPsServer Client Socket Open!");
						allSockets.Add(socket);
					};
					socket.OnClose = () =>
					{
						Console.WriteLine("mockDOPsServer Client Socket Close!");
						allSockets.Remove(socket);
					};
					socket.OnMessage = message =>
					{
						Console.WriteLine(message);
						if(message.Equals("exit")){
							socket.Close();
							_shouldStop = true;
						}
						else
							allSockets.ToList().ForEach(s => s.Send("Echo: " + message));
					};
					socket.OnBinary = blob =>
					{
						Console.WriteLine("mockDOPsServer Received blob from WebSocketClient");
						try {

							handleDOPsProtocol hDOPs = new handleDOPsProtocol();
							byte[] dedpacket = hDOPs.ReceiveDEDandResponse(blob);
								
							// Send it back as echo - until unittest have changed to handle response DED from above ReceiveDEDandResponse function
							socket.Send(blob).Wait(CancellationToken.None);
							//Thread.Sleep(1000);
						}
						catch(Exception e){
							Console.WriteLine("WARNING: Exception while trying to decode possible DED blob");
							// Send it back as echo
							socket.Send(blob).Wait(CancellationToken.None);
							Thread.Sleep(1000);
						}

					};
				});

			while (!_shouldStop)
			{
				//Console.WriteLine("worker thread: working...");
				Console.Title = "WORKING";
			}
			Console.WriteLine("worker thread: terminating gracefully.");
			Console.Title = "DONE";
		}

		public void RequestStop()
		{
			// close all sockets, if any
			//allSockets.ToList().ForEach(s => s.Close());

			_shouldStop = true;
		}

	}

	public void Start()
	{
		workerObject = new Worker ();
		workerThread = new Thread(workerObject.DoWork);

		// Start the worker thread.
		workerThread.Start();
		Console.WriteLine("main thread: Starting worker thread...");

		// Loop until worker thread activates. 
		while (!workerThread.IsAlive);

		// Put the main thread to sleep for minTimeout millisecond to 
		// allow the worker thread to do some work:
		Thread.Sleep(1);

	}


	public void StopDOPsServer()
	{
		// Request that the worker thread stop itself:
		workerObject.RequestStop();
		bDone = true;
	}


	public void WaitForStop(int additionCycles)
	{
		WaitSomeTime (additionCycles);
		// Use the Join method to block the current thread  
		// until the object's thread terminates.
		workerThread.Join();
		Console.WriteLine("main thread: Worker thread has terminated.");
	}


}
