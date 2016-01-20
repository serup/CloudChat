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

		// An nice way to test for exceptions the class under test should 
		// throw is:
		/*
	[Test]
	[ExpectedException(typeof(ArgumentNullException))]
	public void OnValid() {
		ConcreteCollection myCollection;
		myCollection = new ConcreteCollection();
		....
		AssertEquals ("#UniqueID", expected, actual);
		....
		Fail ("Message");
	}
	*/

	}
}
