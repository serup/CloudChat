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
	public class Client : ClientWebSocket
	{
		private const int BufferSize = 4096;
		private const int BufferAmplifier = 20;
		private static int maxAmountOfBytesToRead = BufferSize;
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
					var temporaryBuffer = new byte[BufferSize];
					var buffer = new byte[BufferSize*BufferAmplifier];
					var offset = 0;
					WebSocketReceiveResult response;
					do {
						do 
						{
							response = await ReceiveAsync(new ArraySegment<byte>(temporaryBuffer), CancellationToken.None);
							temporaryBuffer.CopyTo(buffer, offset);
							offset += response.Count;
							temporaryBuffer = new byte[BufferSize];
						} while (!response.EndOfMessage);
						if (response.MessageType != WebSocketMessageType.Close )
						{
							if (bufferForRead.Length < response.Count) {
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
								receivedBuffer = new byte[response.Count];
								Buffer.BlockCopy (buffer, 0, receivedBuffer, 0, response.Count);
								buffer = new byte[BufferSize*BufferAmplifier];
								offset = 0;
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