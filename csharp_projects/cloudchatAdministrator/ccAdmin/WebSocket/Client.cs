using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.WebSockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace WebSocketClient
{
//	public class Client
//	{
//		public static bool bClientError = false;
//		private static object consoleLock = new object();
//		private const int sendChunkSize = 256;
////		private const int receiveChunkSize = 64; // TODO: perhaps implement ringbuffer to handle incomming data
//		private const int receiveChunkSize = 256;
//		private const bool verbose = true;
//		private static readonly TimeSpan delay = TimeSpan.FromMilliseconds(100);
//		public ClientWebSocket webSocket = null;
//		private static byte[] receivedBuffer = null;
//		EventWaitHandle _waitHandle = null; // is signaled when data has been read
//		CancellationToken token;
//		byte[] bufferForRead = new byte[1024];
//		ArraySegment<byte> arraySegment;
//
////		public static async Task _Connect(string uri)
////		{
////			//ClientWebSocket webSocket = null;
////
////			try
////			{
////				webSocket = new ClientWebSocket();
////				webSocket.ConnectAsync(new Uri(uri), CancellationToken.None).Wait();
////				await Task.WhenAll(Receive(webSocket), SendRandom(webSocket));
////			}
////			catch (Exception ex)
////			{
////				Console.WriteLine("Exception: {0}", ex);
////			}
////			finally
////			{
////				if (webSocket != null) {
////					if (webSocket.State == WebSocketState.Open || webSocket.State == WebSocketState.CloseReceived)
////						webSocket.Dispose ();
////				}
////				Console.WriteLine("Connection with mockDOPsServer ended for WebSocketClient");
////
////				lock (consoleLock)
////				{
////					Console.ForegroundColor = ConsoleColor.Red;
////					Console.WriteLine("WebSocket closed.");
////					Console.ResetColor();
////				}
////			}
////		}
////
//
//		public async Task Connect(string uri)
//		{
//			try
//			{
//				ArraySegment<byte> arraySegment = new ArraySegment<byte>(bufferForRead);
//				// Define the cancellation token.
//      			//CancellationTokenSource source = new CancellationTokenSource();
//				//token = source.Token;
//				token = CancellationToken.None;
//				_waitHandle = new AutoResetEvent (false); // is signaled when data has been read
//				webSocket = new ClientWebSocket();
//				webSocket.ConnectAsync(new Uri(uri), token).Wait();
//				//await Task.WhenAll(ReceiveBLOB(bufferForRead, arraySegment, webSocket,_waitHandle));  
//				new Task(() => {
//					SetupReceiveLoop();  
//				}).Start();
//			}
//			catch (Exception ex)
//			{
//				Console.WriteLine("Exception: {0}", ex);
//			}
//			//finally
//			//{
//				//if (webSocket != null) {
//				//	if(webSocket.State == WebSocketState.Open || webSocket.State == WebSocketState.CloseReceived) {
//				//		webSocket.Dispose();
//				//		webSocket.Abort();
//				//	}
//				//}
//
//				//lock (consoleLock)
//				//{
//				//	Console.ForegroundColor = ConsoleColor.Red;
//				//	Console.WriteLine("Connection with mockDOPsServer ended for WebSocketClient");
//				//	Console.WriteLine("WebSocket closed.");
//				//	Console.ResetColor();
//				//}
//			//}
//		}
//
//		public async Task SetupReceiveLoop()
//		{
//			try {
//				await Task.WhenAll(ReceiveBLOB(bufferForRead, arraySegment, webSocket,_waitHandle));  
//			}
//			catch(Exception e) {
//				Console.WriteLine(e.Message.ToString());
//			}
//		}
//
////		public static async Task Connect(string uri, ClientWebSocket _webSocket)
////		{
////			try
////			{
////				webSocket = new ClientWebSocket();
////				webSocket.ConnectAsync(new Uri(uri), CancellationToken.None).Wait();
////				await Task.WhenAll(ReceiveBLOB(webSocket));  
////			}
////			catch (Exception ex)
////			{
////				Console.WriteLine("Exception: {0}", ex);
////			}
////			finally
////			{
////				if (webSocket != null) {
////					if (webSocket.State == WebSocketState.Open || webSocket.State == WebSocketState.CloseReceived)
////						webSocket.Dispose ();
////				}
////				Console.WriteLine("Connection with mockDOPsServer ended for WebSocketClient");
////
////				lock (consoleLock)
////				{
////					Console.ForegroundColor = ConsoleColor.Red;
////					Console.WriteLine("WebSocket closed.");
////					Console.ResetColor();
////				}
////			}
////		}
//
////		public static async Task Connect(string uri, Func<ClientWebSocket, Task> ReceiveDataFunction)
////		{
////			//ClientWebSocket webSocket = null;
////
////			try
////			{
////				webSocket = new ClientWebSocket();
////				webSocket.ConnectAsync(new Uri(uri), CancellationToken.None).Wait();
//////				await Task.WhenAll(Receive(webSocket), SendRandom(webSocket));
////				await Task.WhenAll(ReceiveDataFunction(webSocket));
////			}
////			catch (Exception ex)
////			{
////				Console.WriteLine("Exception: {0}", ex);
////			}
////			finally
////			{
////				if (webSocket != null) {
////					if (webSocket.State == WebSocketState.Open || webSocket.State == WebSocketState.CloseReceived)
////						webSocket.Dispose ();
////				}
////				Console.WriteLine("Connection with mockDOPsServer ended for WebSocketClient");
////
////				lock (consoleLock)
////				{
////					Console.ForegroundColor = ConsoleColor.Red;
////					Console.WriteLine("WebSocket closed.");
////					Console.ResetColor();
////				}
////			}
////		}
//
////		public static async Task ConnectTo(string uri)
////		{
////			ClientWebSocket __webSocket = null;
////			EventWaitHandle WaitHandle = new AutoResetEvent (false); // is signaled when data has been read
////
////			try
////			{
////				__webSocket = new ClientWebSocket();
////				__webSocket.ConnectAsync(new Uri(uri), CancellationToken.None).Wait();
////				webSocket = __webSocket;
////				await Task.WhenAll(ReceiveBLOB(__webSocket, WaitHandle));
////			}
////			catch (Exception ex)
////			{
////				Console.WriteLine("Exception: {0}", ex);
////			}
////			finally
////			{
////				if (webSocket != null) {
////					if (webSocket.State == WebSocketState.Open || webSocket.State == WebSocketState.CloseReceived)
////						webSocket.Dispose ();
////				}
////				Console.WriteLine("Connection with mockDOPsServer ended for WebSocketClient");
////
////				lock (consoleLock)
////				{
////					Console.ForegroundColor = ConsoleColor.Red;
////					Console.WriteLine("WebSocket closed.");
////					Console.ResetColor();
////				}
////			}
////		}
//
//		public class wshandles 
//		{
//			public Client client;
//		 	public ClientWebSocket webSocket;
//			public EventWaitHandle waitHandle;
//			public CancellationToken token;
//		}
//
//		public static wshandles WSConnect (string uri)
//		{
//			Client client = new Client();
//			client.Connect(uri);
//			//Thread.Sleep(1000); // Wait for establishment
//			wshandles _handles = new wshandles();
//			_handles.client = client;
//			_handles.webSocket = client.webSocket;
//			_handles.waitHandle = client._waitHandle;
//			_handles.token = client.token;
//			if (client.webSocket != null) {
//				if (client.webSocket.State == WebSocketState.Connecting || client.webSocket.State == WebSocketState.CloseReceived)
//					client.webSocket.Dispose (); // Somehow no backend to connect to - disconnect to try again later
//
//			}
//			return _handles;
//		}
//
//		public static void WSDisconnect(wshandles _handles)
//		{
//			_handles.waitHandle = null;
//			_handles.webSocket.Dispose();
//		}
//
////		public static async Task SendBLOB(byte[] buffer)
////		{
////			Console.WriteLine("WebSocketClient SendBLOB starting!");
////
////			if (webSocket.State == WebSocketState.Open)
////			{
////				webSocket.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Binary, false, CancellationToken.None).Wait();
////				LogStatus(false, buffer, buffer.Length);
////
////				await Task.Delay(delay);
////			}
////			Console.WriteLine("WebSocket SendBLOB ending!");
////		}
//
//		public static async Task SendBLOB(byte[] buffer, ClientWebSocket webSocket)
//		{
//			Console.WriteLine("WebSocketClient SendBLOB starting!");
//
//			if (webSocket.State == WebSocketState.Open)
//			{
////				webSocket.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Binary, false, CancellationToken.None).Wait();
//				webSocket.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Binary, true, CancellationToken.None).Wait();
//				LogStatus(false, buffer, buffer.Length);
//
//				await Task.Delay(delay);
//			}
//			Console.WriteLine("WebSocket SendBLOB ending!");
//		}
//
////		public static async Task SendRandomBLOB()
////		{
////			var random = new Random();
////			byte[] buffer = new byte[sendChunkSize];
////			Console.WriteLine("WebSocketClient SendBLOB setup");
////
////			while (webSocket.State == WebSocketState.Open)
////			{
////				random.NextBytes(buffer);
////
////				await webSocket.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Binary, false, CancellationToken.None);
////				LogStatus(false, buffer, buffer.Length);
////
////				await Task.Delay(delay);
////			}
////			Console.WriteLine("WebSocket SendBLOB ending!");
////		}
//
////		private static async Task SendRandom(ClientWebSocket webSocket)
////		{
////			var random = new Random();
////			byte[] buffer = new byte[sendChunkSize];
////			Console.WriteLine("WebSocketClient Send setup");
////
////			while (webSocket.State == WebSocketState.Open)
////			{
////				random.NextBytes(buffer);
////
////				await webSocket.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Binary, false, CancellationToken.None);
////				LogStatus(false, buffer, buffer.Length);
////
////				await Task.Delay(delay);
////			}
////			Console.WriteLine("WebSocket Send ending!");
////		}
//
////		private static async Task Receive(ClientWebSocket webSocket)
////		{
////			byte[] buffer = new byte[receiveChunkSize];
////			Console.WriteLine("WebSocketClient Receive setup");
////			while (webSocket.State == WebSocketState.Open)
////			{                
////				var result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);
////				if (result.MessageType == WebSocketMessageType.Close)
////				{
////					await webSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, string.Empty, CancellationToken.None);
////				}
////				else
////				{
////					if (buffer.Length < result.Count)
////						Console.WriteLine ("WARNING - incomming data is larger than internal buffer !!!");
////					else
////						LogStatus(true, buffer, result.Count);
////				}
////			}
////			Console.WriteLine("WebSocket Receive ending!");
////		}
//
////		public static byte[] FetchReceived()
////		{
////			Console.WriteLine ("Waiting...");
////			_waitHandle.WaitOne();                // Wait for notification
////			Console.WriteLine ("Notified");
////			return receivedBuffer;
////		}
//
//		public static byte[] FetchReceived (wshandles _handles)
//		{
//			try{
//				if(bClientError) return null;
//				Console.WriteLine("Waiting...");
//				if(_handles.webSocket.State == WebSocketState.Open && _handles.webSocket != null) {
//					_handles.waitHandle.WaitOne();                // Wait for notification
//					Console.WriteLine("Notified");
//				} else
//					Console.WriteLine("Waiting - aborted - socket is closed");
//			}
//			catch (Exception e) {
//				Console.WriteLine(e.ToString());
//			}
//			return receivedBuffer;
//		}
//
////		private static async Task ReceiveBLOB(ClientWebSocket webSocket)
////		{
////			byte[] buffer = new byte[1024];
////			Console.WriteLine("WebSocketClient Receive setup");
////			receivedBuffer = null;
////
////			while (webSocket.State == WebSocketState.Open)
////			{                
////				var result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);
////				if (result.MessageType == WebSocketMessageType.Close)
////				{
////					await webSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, string.Empty, CancellationToken.None);
////				}
////				else
////				{
////					if (buffer.Length < result.Count) {
////						Console.WriteLine ("WARNING - incomming data is larger than internal buffer - trying to receive and merge rest !!!");
////						receivedBuffer = null;
////						/*  DOES NOT WORK - MISSING BYTES IN BETWEEN - FIND A BETTER WAY
////						int restBytes = result.Count - buffer.Length;
////						byte[] restBuffer = new byte[restBytes];
////						receivedBuffer = new byte[result.Count]; // make room for ALL data
////						Buffer.BlockCopy (buffer, 0, receivedBuffer, 0, buffer.Length); // copy first part
////						// try to receive waiting bytes
////						result = await webSocket.ReceiveAsync(new ArraySegment<byte>(restBuffer), CancellationToken.None);
////						Buffer.BlockCopy (restBuffer, 0, receivedBuffer, buffer.Length-1, restBuffer.Length); // copy rest part
////						*/
////						_waitHandle.Set (); // signal that data has been received - this will wake up WaitForData() - which will then return it to user
////					}
////					else {
////						receivedBuffer = new byte[result.Count];
////						Buffer.BlockCopy (buffer, 0, receivedBuffer, 0, result.Count);
////						_waitHandle.Set (); // signal that data has been received - this will wake up WaitForData() - which will then return it to user
////					}
////				}
////			}
////			Console.WriteLine("WebSocket Receive ending!");
////		}
////
//
//		private static async Task ReceiveBLOB(byte[] buffer, ArraySegment<byte> arraySegment, ClientWebSocket webSocket, EventWaitHandle WaitHandle)
//		{
//			
//			Console.WriteLine("WebSocketClient Receive setup");
//			receivedBuffer = null;
//			try {
//				while (webSocket.State == WebSocketState.Open)
//				{            
//					try {
//						//var result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), token);
//
//					var result = await webSocket.ReceiveAsync(arraySegment, CancellationToken.None);
//					if (result.MessageType == WebSocketMessageType.Close )
//					{
//						if(webSocket.State == WebSocketState.Open)
//							await webSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, string.Empty, CancellationToken.None);
//					}
//					else
//					{
//						if (buffer.Length < result.Count) {
//							Console.WriteLine ("WARNING - incomming data is larger than internal buffer - trying to receive and merge rest !!!");
//							receivedBuffer = null;
//							/*  DOES NOT WORK - MISSING BYTES IN BETWEEN - FIND A BETTER WAY
//							int restBytes = result.Count - buffer.Length;
//							byte[] restBuffer = new byte[restBytes];
//							receivedBuffer = new byte[result.Count]; // make room for ALL data
//							Buffer.BlockCopy (buffer, 0, receivedBuffer, 0, buffer.Length); // copy first part
//							// try to receive waiting bytes
//							result = await webSocket.ReceiveAsync(new ArraySegment<byte>(restBuffer), CancellationToken.None);
//							Buffer.BlockCopy (restBuffer, 0, receivedBuffer, buffer.Length-1, restBuffer.Length); // copy rest part
//							*/
//							WaitHandle.Set (); // signal that data has been received - this will wake up WaitForData() - which will then return it to user
//						}
//						else {
//							receivedBuffer = new byte[result.Count];
//							Buffer.BlockCopy (buffer, 0, receivedBuffer, 0, result.Count);
//							WaitHandle.Set (); // signal that data has been received - this will wake up WaitForData() - which will then return it to user
//						}
//					}
//					}
//					catch(Exception e)
//					{
//						Console.WriteLine(e.Message.ToString());
////						Console.WriteLine("ERROR: reading on socket failed - will try to ignore it");
////						webSocket.Abort();
//					}
//				}
//			}
//			catch (Exception e)
//			{
//				Console.WriteLine(e.ToString());
//				Console.WriteLine(e.Message.ToString());
//				Console.WriteLine("Exception: NO longer possible to receive data from DOPs");
//				bClientError = true;
//			}
//			Console.WriteLine("WebSocket Receive ending!");
//			WaitHandle.Set (); // Signal to end a possible wait
//		}
//
//		private static void LogStatus(bool receiving, byte[] buffer, int length)
//		{
//			lock (consoleLock)
//			{
//				Console.ForegroundColor = receiving ? ConsoleColor.Green : ConsoleColor.Gray;
//				Console.WriteLine("{0} {1} bytes... ", receiving ? "Received" : "Sent", length);
//
//				if (verbose)
//					Console.WriteLine(BitConverter.ToString(buffer, 0, length));
//
//				Console.ResetColor();
//			}
//		}
//	}
//
	/// <summary>
	/// REFACTOR section
	/// </summary>
	// Above is a mess - try to refactor, by making a new class
	public class newClient : ClientWebSocket
	{
		private static int maxAmountOfBytesToRead = 1024;
		private byte[] bufferForRead = new byte[maxAmountOfBytesToRead];
		private ArraySegment<byte> linkToBufferForRead;
		private byte[] receivedBuffer = null;
		EventWaitHandle signalDataHasArrived  = new AutoResetEvent (false); // is signaled when data is ready in receivedBuffer;
		bool signalToStopReceiving = false; // continue reading until notified to stop

		public bool Connect(string uri) {
			bool bResult = false;
			try {
				linkToBufferForRead = new ArraySegment<byte>(bufferForRead);
				ConnectAsync(new Uri(uri), CancellationToken.None).Wait();
				if(State == WebSocketState.Open) {
					setupReceiveTask();
					bResult = true;
				}
			}
			catch(Exception e) {
				Console.WriteLine(e.Message.ToString());
			}
			return bResult;
		}

		private bool setupReceiveTask()
		{
			bool bResult = true;
			try {
				new Task(async () => {
					Console.WriteLine ("Receiving Task has started");
					do {
						var taskReturn = await ReceiveAsync(linkToBufferForRead, CancellationToken.None);
						if (taskReturn.MessageType != WebSocketMessageType.Close )
						{
							if (bufferForRead.Length < taskReturn.Count) {
								Console.WriteLine ("WARNING - incomming data is larger than internal buffer - trying to receive and merge rest !!!");
								receivedBuffer = null;
								/*  DOES NOT WORK - MISSING BYTES IN BETWEEN - FIND A BETTER WAY
								int restBytes = result.Count - buffer.Length;
								byte[] restBuffer = new byte[restBytes];
								receivedBuffer = new byte[result.Count]; // make room for ALL data
								Buffer.BlockCopy (buffer, 0, receivedBuffer, 0, buffer.Length); // copy first part
								// try to receive waiting bytes
								result = await webSocket.ReceiveAsync(new ArraySegment<byte>(restBuffer), CancellationToken.None);
								Buffer.BlockCopy (restBuffer, 0, receivedBuffer, buffer.Length-1, restBuffer.Length); // copy rest part
								*/
								signalDataHasArrived.Set (); // signal that data has been received - this will wake up WaitForData() - which will then return it to user
							}
							else {
								receivedBuffer = new byte[taskReturn.Count];
								Buffer.BlockCopy (bufferForRead, 0, receivedBuffer, 0, taskReturn.Count);
								signalDataHasArrived.Set (); // signal that data has been received - this will wake up WaitForData() - which will then return it to user
							}
						}
					}while(signalToStopReceiving==false && State == WebSocketState.Open);
					Console.WriteLine ("Receiving Task has stopped");
				}).Start();
			}
			catch(Exception e) {
				//Console.WriteLine(e.Message.ToString());
				Console.WriteLine("WARNING: Receiving Task was forcefylly stopped");
				bResult = false;
			}
			return bResult;
		}

		public async Task SendBLOB(byte[] buffer)
		{
			Console.WriteLine("WebSocketClient SendBLOB starting!");
			if (State == WebSocketState.Open)
			{
				await SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Binary, true, CancellationToken.None);
			}
			Console.WriteLine("WebSocket SendBLOB ending!");
		}

		public byte[] FetchReceived(bool bContinueInternalReceiveTaskAfterFetch = false)
		{
			Console.WriteLine("Waiting for data to arrive...");
			if(State == WebSocketState.Open) {
				if(!bContinueInternalReceiveTaskAfterFetch) 
					stopReceivingTask();
				signalDataHasArrived.WaitOne();  // Wait for notification, and if signalToStopReceiving is true, then receiving task will exit
				Console.WriteLine("Notified - data has arrived");
				return receivedBuffer;
			} else {
				Console.WriteLine("ERROR: WebSocket is NOT open, could NOT wait for notification of data receival");
				return null;
			}
		}

		public void stopReceivingTask()
		{
			signalToStopReceiving = true;
		}
	}

}