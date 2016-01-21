using System;
using NUnit.Framework;
using DED;

// all test namespaces start with "MonoTests."  Append the Namespace that
// contains the class you are testing, e.g. MonoTests.System.Collections


namespace DEDTests.Tests
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


		// An nice way to test for exceptions the class under test should 
		// throw is:

		/*  
//	[Test]
//	[ExpectedException(typeof(ArgumentNullException))]
//	public void OnValid() {
//		ConcreteCollection myCollection;
//		myCollection = new ConcreteCollection();
//		....
//		AssertEquals ("#UniqueID", expected, actual);
//		....
//		Fail ("Message");
//	}
	*/

	}
}
