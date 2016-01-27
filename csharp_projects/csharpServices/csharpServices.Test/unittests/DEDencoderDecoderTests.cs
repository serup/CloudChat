using System;
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


		[Test]
		public void SendReceiveDEDMockDOPsServer()
		{
			byte[] ReceiveBuffer = null;
			var mockDOPsServer = new MockDOPsServer (8046,"/websockets/MockServerEndpoint");

			// start mock DOPs Server
			mockDOPsServer.Start();

			// connect to DOPs Server
			Client.Connect ("ws://localhost:8046/websockets/MockServerEndpoint");

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
			Client.SendBLOB (byteArray).Wait(); // NB! Not really necessary to add Wait() in this case since FetchReceived() will wait until data is ready

			// Fetch the return data from the Server
			ReceiveBuffer = Client.FetchReceived ();

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

			// wait for release
			mockDOPsServer.WaitForStop (0); 
		}
	}
}
