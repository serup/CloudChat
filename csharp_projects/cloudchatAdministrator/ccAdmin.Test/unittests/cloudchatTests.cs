﻿using System;
using NUnit.Framework;
using csharpServices;
using DED;
using System.Threading;
using Moq;

namespace cloudchat
{

	[TestFixture]
	public class cloudchatTests : Assert {

		cloudChatPacketHandler chatHandler;

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
				DED.PUT_STDSTRING("src", "4086d4ab369e14ca1b6be7364d88cf85"); //  uniqueID
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
		public void GetReady() { chatHandler = new cloudChatPacketHandler();}

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


	[TestFixture]
	public class AARTests : Assert {


		class WaitWorker {
			private volatile bool _shouldStop;
			public EventWaitHandle WaitHandleExternal;
			public int IntervalInMilliseconds;
			public void DoWork ()
			{
				while (!_shouldStop)
				{
					Console.WriteLine("worker thread: working...");
					Thread.Sleep(IntervalInMilliseconds);
					WaitHandleExternal.Set();
				}
			}

			public void RequestStop()
			{
				_shouldStop = true;
			}

			public void WaitForMilliseconds(int Interval)
			{
				EventWaitHandle _waitHandle = new AutoResetEvent (false); // is signaled value change to true

				// start a thread which will after a small time set an event
				WaitWorker workerObject = new WaitWorker ();
				workerObject.IntervalInMilliseconds = Interval;
				workerObject.WaitHandleExternal = _waitHandle;
				Thread workerThread = new Thread(workerObject.DoWork);

				// Start the worker thread.
				workerThread.Start();

				Console.WriteLine ("Waiting...");
				_waitHandle.WaitOne();                // Wait for notification
				Console.WriteLine ("Notified");

				// Stop the worker thread.
				workerObject.RequestStop();
			}
		}

		cloudChatPacketHandler chatHandler;

		private byte[] createForwardInfoRequest(string src, string srcAlias) {

			DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
			DED.PUT_STRUCT_START ("CloudManagerRequest");
			DED.PUT_METHOD ("Method", "JSCForwardInfo");
			DED.PUT_USHORT ("TransID", 23);
			DED.PUT_STDSTRING("protocolTypeID", "DED1.00.00");
			DED.PUT_STDSTRING("dest", "ccAdmin");  // This Admins uniqueID, since this is a test it really does not matter
			DED.PUT_STDSTRING("src", src); // CloudChatManager uniqueID
			DED.PUT_STDSTRING("srcAlias", srcAlias);
			DED.PUT_STRUCT_END("CloudManagerRequest");
			byte[] byteArray = DED.GET_ENCODED_BYTEARRAY_DATA ();
			Assert.IsNotNull (byteArray);
			return byteArray;
		}

		private byte[] createChatInfo(string src, string srcAlias) {

			DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
			DED.PUT_STRUCT_START ("ClientChatRequest");
			DED.PUT_METHOD ("Method", "JSCChatInfo");
			DED.PUT_USHORT ("TransID", 23);
			DED.PUT_STDSTRING("protocolTypeID", "DED1.00.00");
			DED.PUT_STDSTRING("dest", "ccAdmin");  // This Admins uniqueID, since this is a test it really does not matter
			DED.PUT_STDSTRING("src", src); // uniqueID
			DED.PUT_STDSTRING("srcAlias", srcAlias);
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
		public void GetReady() { 
			chatHandler = new cloudChatPacketHandler();
		}

		// this method is run after each Test* method is called. You can put
		// clean-up code, etc. here.  Whatever needs to be done after each test.
		// Just leave the method empty if you don't need to use it.
		// The name of the method does not matter; the attribute does.
		[TearDown]
		public void Clean() {}


		[Test]
		public void addManagerToListTest() {
			AARHandler aar = new AARHandler();
			byte[] data = createForwardInfoRequest("4086d4ab369e14ca1b6be7364d88cf85","SERUP");
			dedAnalyzed dana = chatHandler.parseDEDpacket(data);
			aar.handleRouting(dana);
			Assert.True(aar.getManagerListCount() == 1);
		}

		[Test]
		public void addMultipleManagerToListTest() {
			AARHandler aar = new AARHandler();
			byte[] data = createForwardInfoRequest("4086d4ab369e14ca1b6be7364d88cf85","SERUP");
			dedAnalyzed dana = chatHandler.parseDEDpacket(data);
			aar.handleRouting(dana);
			data = createForwardInfoRequest("4086d4ab369e14ca1b6be7364d88cf44","SERUP2");
			dana = chatHandler.parseDEDpacket(data);
			aar.handleRouting(dana);
			data = createForwardInfoRequest("4086d4ab369e14ca1b6be7364d88cf45","SERUP3");
			dana = chatHandler.parseDEDpacket(data);
			aar.handleRouting(dana);
			data = createForwardInfoRequest("4086d4ab369e14ca1b6be7364d88cf46","SERUP4");
			dana = chatHandler.parseDEDpacket(data);
			aar.handleRouting(dana);
			Assert.True(aar.getManagerListCount() == 4);
		}


		[Test]
		public void removeOfflineManagerTest() {

			// First add some managers
			AARHandler aar = new AARHandler();
			const int Interval = 30000;
			aar.setMaxIdleTimeInList (Interval);  
			byte[] data = createForwardInfoRequest("4086d4ab369e14ca1b6be7364d88cf85","SERUP");
			dedAnalyzed dana = chatHandler.parseDEDpacket(data);
			aar.handleRouting(dana);
			data = createForwardInfoRequest("4086d4ab369e14ca1b6be7364d88cf99","SERUP2");
			dana = chatHandler.parseDEDpacket(data);
			aar.handleRouting(dana);
			Assert.True(aar.getManagerListCount() == 2);

			//WaitWorker ww = new WaitWorker ();
			//ww.WaitForMilliseconds (Interval/2); // TODO: find a way to do relative sleep, meaning no actual time is spend (push / pop actual time, warping effect)
			aar.InitiateWarpTime(Interval/2); // This will add milliseconds to relative time, thus warping it

			data = createForwardInfoRequest("4086d4ab369e14ca1b6be7364d88cf77","SERUP3");
			dana = chatHandler.parseDEDpacket(data);
			aar.handleRouting(dana);
			Assert.False(aar.getManagerListCount() == 1); // There where still previous added manager in list which did not exceed the time span allowed to be in list, hence the failure

			// Wait for max timespan
			aar.InitiateWarpTime(aar.getMaxIdleMillisecondTimeInList()+(Interval/2)); // This will add milliseconds to relative time, thus warping it

			data = createForwardInfoRequest("4086d4ab369e14ca1b6be7364d88cf22","SERUP4");
			dana = chatHandler.parseDEDpacket(data);
			aar.handleRouting(dana);
			Assert.True(aar.getManagerListCount() == 1);
		}

		[Test]
		public void forwardChatInfoToOnlineManagersTest() {

			// First add some managers
			AARHandler aar = new AARHandler();
			const int Interval = 30000;
			aar.setMaxIdleTimeInList (Interval);  
			byte[] data = createForwardInfoRequest("4086d4ab369e14ca1b6be7364d88cf85","SERUP");
			dedAnalyzed dana = chatHandler.parseDEDpacket(data);
			aar.handleRouting(dana);
			data = createForwardInfoRequest("4086d4ab369e14ca1b6be7364d88cf99","SERUP2");
			dana = chatHandler.parseDEDpacket(data);
			aar.handleRouting(dana);
			Assert.True(aar.getManagerListCount() == 2);

			// Then create chatInfo
			data = createChatInfo("4086d4ab369e14ca1b6be7364d88cf11","chatClient");
			dana = chatHandler.parseDEDpacket(data);

			// Then handle chatInfo - updateOnlineManagersWithIncomingChatInfo
			aar.handleRouting(dana);

			// Verify that dana dest was NOT changed to manager src, since NO dops API was initiated
			Assert.False(dana.getElement("dest").value == "4086d4ab369e14ca1b6be7364d88cf99");
		}

		[Test]
		public void avoidMultipleEntriesInListTest()
		{
			// Same ded should NOT create multiple entries in list
			AARHandler aar = new AARHandler();
			byte[] data = createForwardInfoRequest("4086d4ab369e14ca1b6be7364d88cf22","SERUP4");
			dedAnalyzed dana = chatHandler.parseDEDpacket(data);
			aar.handleRouting(dana);
			data = createForwardInfoRequest("4086d4ab369e14ca1b6be7364d88cf22","SERUP4");
			dana = chatHandler.parseDEDpacket(data);
			aar.handleRouting(dana);
			Assert.True(aar.getManagerListCount() == 1);
		}
	}
}

