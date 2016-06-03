using System;
using NUnit.Framework;
using csharpServices;
using DED;

namespace cloudchat
{
	[TestFixture]
	public class cloudchatTests : Assert {

		cloudChatHandler chatHandler;

		private byte[] createForwardInfoRequest() {

			DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
			DED.PUT_STRUCT_START ("CloudManagerRequest");
				DED.PUT_METHOD ("Method", "JSCForwardInfo");
				DED.PUT_USHORT ("TransID", 23);
				DED.PUT_STDSTRING("protocolTypeID", "DED1.00.00");
				DED.PUT_STDSTRING("dest", "ccAdmin");  // This Admins uniqueID, since this is a test it really does not matter
				DED.PUT_STDSTRING("src", "4086d4ab369e14ca1b6be7364d88cf85"); // CloudChatManager uniqueID
				DED.PUT_STDSTRING("srcAlias", "SERUP");
			DED.PUT_STRUCT_END("CloudManagerRequest");
			byte[] byteArray = DED.GET_ENCODED_BYTEARRAY_DATA ();
			Assert.IsNotNull (byteArray);
			return byteArray;
		}

		private byte[] createChatInfo() {
			
			DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
			DED.PUT_STRUCT_START ("ClientChatRequest");
				DED.PUT_METHOD ("Method", "JSCChatInfo");
				DED.PUT_USHORT ("TransID", 23);
				DED.PUT_STDSTRING("protocolTypeID", "DED1.00.00");
				DED.PUT_STDSTRING("dest", "ccAdmin");  // This Admins uniqueID, since this is a test it really does not matter
				DED.PUT_STDSTRING("src", "4086d4ab369e14ca1b6be7364d88cf85"); // CloudChatManager uniqueID
				DED.PUT_STDSTRING("srcAlias", "SERUP");
				DED.PUT_STDSTRING("srcHomepageAlias", "Montenegro");
				DED.PUT_STDSTRING("lastEntryTime", "20160101");
				DED.PUT_STRUCT_END("ClientChatRequest");
			byte[] byteArray = DED.GET_ENCODED_BYTEARRAY_DATA ();
			Assert.IsNotNull (byteArray);
			return byteArray;
		}

		// this method is run before each [Test] method is called. You can put
		// variable initialization, etc. here that is common to each test.
		// Just leave the method empty if you don't need to use it.
		// The name of the method does not matter; the attribute does.
		[SetUp]
		public void GetReady() { chatHandler = new cloudChatHandler();}

		// this method is run after each Test* method is called. You can put
		// clean-up code, etc. here.  Whatever needs to be done after each test.
		// Just leave the method empty if you don't need to use it.
		// The name of the method does not matter; the attribute does.
		[TearDown]
		public void Clean() {}

		[Test]
		public void incommingCloudManagerUnknownDEDTest() {

			byte[] data = null;
			dedAnalyzed dana = chatHandler.parseDEDpacket(data);
			Assert.True(dana.type == "<unknown>");
		}

		[Test]
		public void incommingCloudManagerForwardInfoRequestDEDTest() {

			byte[] data = createForwardInfoRequest();
			dedAnalyzed dana = chatHandler.parseDEDpacket(data);
			Assert.True(dana.type == "ForwardInfoRequest");
		}

		[Test]
		public void incommingCloudChatInfoDEDTest() {

			byte[] data = createChatInfo();
			dedAnalyzed dana = chatHandler.parseDEDpacket(data);
			Assert.True(dana.type == "ChatInfo");
		}

	}

}

