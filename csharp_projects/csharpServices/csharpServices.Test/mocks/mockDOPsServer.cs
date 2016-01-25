
using System.Net.Sockets;
using System.Net;
using System;
using Fleck;
using System.Linq;
using System.Threading;

class MockDOPsServer {


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
						Console.WriteLine("Open!");
						allSockets.Add(socket);
					};
					socket.OnClose = () =>
					{
						Console.WriteLine("Close!");
						allSockets.Remove(socket);
					};
					socket.OnMessage = message =>
					{
						Console.WriteLine(message);
						if(message.Equals("exit")){
							//allSockets.Remove(socket);
							socket.Close();
							_shouldStop = true;
						}
						else
							allSockets.ToList().ForEach(s => s.Send("Echo: " + message));
					};
				});

			while (!_shouldStop)
			{
				Console.WriteLine("worker thread: working...");
			}
			Console.WriteLine("worker thread: terminating gracefully.");
		}

		public void RequestStop()
		{
			_shouldStop = true;
		}

	}

	public void Start()
	{
		Worker workerObject = new Worker ();
		Thread workerThread = new Thread(workerObject.DoWork);

		// Start the worker thread.
		workerThread.Start();
		Console.WriteLine("main thread: Starting worker thread...");

		// Loop until worker thread activates. 
		while (!workerThread.IsAlive);

		// Put the main thread to sleep for minTimeout millisecond to 
		// allow the worker thread to do some work:
		Thread.Sleep(1);

		// Request that the worker thread stop itself:
		workerObject.RequestStop();

		// Use the Join method to block the current thread  
		// until the object's thread terminates.
		workerThread.Join();
		Console.WriteLine("main thread: Worker thread has terminated.");
	}




}
