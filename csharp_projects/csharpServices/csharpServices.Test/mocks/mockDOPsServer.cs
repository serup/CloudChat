
using System.Net.Sockets;
using System.Net;
using System;
using Fleck;
using System.Linq;
using System.Threading;
using DED;

class MockDOPsServer {
	Worker workerObject;
	Thread workerThread;
	public volatile bool bDone;

	public void WaitSomeTime(int additioncycles)
	{
		int c = 0;
		while (!bDone) {
			Console.WriteLine ("busy..");
			c++;
			// finished with work
			//if (c > 100000) {
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
			server = new WebSocketServer("ws://127.0.0.1:8046/websockets/MockServerEndpoint"); // NB! running as mockserver in java spring project

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

						short trans_id = 1;
						bool bAction = true;
						bool bDecoded = false;
						DEDDecoder DED2 = DEDDecoder.DED_START_DECODER();
						DED2.PUT_DATA_IN_DECODER (blob, blob.Length);
						if ((DED2.GET_STRUCT_START ("event")).Equals(1) &&
							(	DED2.GET_METHOD ("Method")).Contains("MusicPlayer") &&
							(	DED2.GET_USHORT ("trans_id")).Equals(trans_id) &&
							(	DED2.GET_BOOL   ("startstop")).Equals(bAction) &&
							(DED2.GET_STRUCT_END("event")).Equals(1)) 
						{
							bDecoded = true;
							Console.WriteLine("mockDOPsServer Received DED and decoded it from WebSocketClient");
						}
						else
							bDecoded = false;


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
			allSockets.ToList().ForEach(s => s.Close());

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
