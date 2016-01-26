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
	public class Client
	{
		private static object consoleLock = new object();
		private const int sendChunkSize = 256;
//		private const int receiveChunkSize = 64; // TODO: perhaps implement ringbuffer to handle incomming data
		private const int receiveChunkSize = 256;
		private const bool verbose = true;
		private static readonly TimeSpan delay = TimeSpan.FromMilliseconds(1000);
		private static ClientWebSocket webSocket = null;

/*		static void Main(string[] args)
		{
			Thread.Sleep(1000);
			Connect("ws://localhost/wsDemo").Wait();
			Console.WriteLine("Press any key to exit...");
			Console.ReadKey();
		}
*/
		public static async Task Connect(string uri)
		{
			//ClientWebSocket webSocket = null;

			try
			{
				webSocket = new ClientWebSocket();
				webSocket.ConnectAsync(new Uri(uri), CancellationToken.None).Wait();
				await Task.WhenAll(Receive(webSocket), SendRandom(webSocket));
			}
			catch (Exception ex)
			{
				Console.WriteLine("Exception: {0}", ex);
			}
			finally
			{
				if (webSocket != null) {
					if (webSocket.State == WebSocketState.Open || webSocket.State == WebSocketState.CloseReceived)
						webSocket.Dispose ();
				}
				Console.WriteLine("Connection with mockDOPsServer ended for WebSocketClient");

				lock (consoleLock)
				{
					Console.ForegroundColor = ConsoleColor.Red;
					Console.WriteLine("WebSocket closed.");
					Console.ResetColor();
				}
			}
		}

		public static async Task Connect(string uri, Func<ClientWebSocket, Task> ReceiveDataFunction)
		{
			//ClientWebSocket webSocket = null;

			try
			{
				webSocket = new ClientWebSocket();
				webSocket.ConnectAsync(new Uri(uri), CancellationToken.None).Wait();
//				await Task.WhenAll(Receive(webSocket), SendRandom(webSocket));
				await Task.WhenAll(ReceiveDataFunction(webSocket));
			}
			catch (Exception ex)
			{
				Console.WriteLine("Exception: {0}", ex);
			}
			finally
			{
				if (webSocket != null) {
					if (webSocket.State == WebSocketState.Open || webSocket.State == WebSocketState.CloseReceived)
						webSocket.Dispose ();
				}
				Console.WriteLine("Connection with mockDOPsServer ended for WebSocketClient");

				lock (consoleLock)
				{
					Console.ForegroundColor = ConsoleColor.Red;
					Console.WriteLine("WebSocket closed.");
					Console.ResetColor();
				}
			}
		}

		public static async Task SendBLOB(byte[] buffer)
		{
			Console.WriteLine("WebSocketClient SendBLOB starting!");

			if (webSocket.State == WebSocketState.Open)
			{
				webSocket.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Binary, false, CancellationToken.None).Wait();
				LogStatus(false, buffer, buffer.Length);

				await Task.Delay(delay);
			}
			Console.WriteLine("WebSocket SendBLOB ending!");
		}

		public static async Task SendRandomBLOB()
		{
			var random = new Random();
			byte[] buffer = new byte[sendChunkSize];
			Console.WriteLine("WebSocketClient SendBLOB setup");

			while (webSocket.State == WebSocketState.Open)
			{
				random.NextBytes(buffer);

				await webSocket.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Binary, false, CancellationToken.None);
				LogStatus(false, buffer, buffer.Length);

				await Task.Delay(delay);
			}
			Console.WriteLine("WebSocket SendBLOB ending!");
		}

		private static async Task SendRandom(ClientWebSocket webSocket)
		{
			var random = new Random();
			byte[] buffer = new byte[sendChunkSize];
			Console.WriteLine("WebSocketClient Send setup");

			while (webSocket.State == WebSocketState.Open)
			{
				random.NextBytes(buffer);

				await webSocket.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Binary, false, CancellationToken.None);
				LogStatus(false, buffer, buffer.Length);

				await Task.Delay(delay);
			}
			Console.WriteLine("WebSocket Send ending!");
		}

		private static async Task Receive(ClientWebSocket webSocket)
		{
			byte[] buffer = new byte[receiveChunkSize];
			Console.WriteLine("WebSocketClient Receive setup");
			while (webSocket.State == WebSocketState.Open)
			{                
				var result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);
				if (result.MessageType == WebSocketMessageType.Close)
				{
					await webSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, string.Empty, CancellationToken.None);
				}
				else
				{
					if (buffer.Length < result.Count)
						Console.WriteLine ("WARNING - incomming data is larger than internal buffer !!!");
					else
						LogStatus(true, buffer, result.Count);
				}
			}
			Console.WriteLine("WebSocket Receive ending!");
		}

		private static void LogStatus(bool receiving, byte[] buffer, int length)
		{
			lock (consoleLock)
			{
				Console.ForegroundColor = receiving ? ConsoleColor.Green : ConsoleColor.Gray;
				Console.WriteLine("{0} {1} bytes... ", receiving ? "Received" : "Sent", length);

				if (verbose)
					Console.WriteLine(BitConverter.ToString(buffer, 0, length));

				Console.ResetColor();
			}
		}
	}


}