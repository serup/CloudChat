using System;
using System.Net;
using System.Security.Cryptography.X509Certificates;
using Moq;
using NUnit.Framework;
using System.Net.Sockets;
using System.Security.Authentication;
using System.Linq;
using System.Timers;
using System.Threading.Tasks;
using System.Windows.Threading;
using System.Threading;
using System.Net.WebSockets;
using System.Text;
using WebSocketClient;

namespace Fleck.Tests
{
    [TestFixture]
    public class WebSocketServerTests
    {
		private WebSocketServer _server;
        private MockRepository _repository;

        private IPAddress _ipV4Address;
        private IPAddress _ipV6Address;

        private Socket _ipV4Socket;
        private Socket _ipV6Socket;

        [SetUp]
        public void Setup()
        {
            _repository = new MockRepository(MockBehavior.Default);
            _server = new WebSocketServer("ws://0.0.0.0:8000");

            _ipV4Address = IPAddress.Parse("127.0.0.1");
            _ipV6Address = IPAddress.Parse("::1");

            _ipV4Socket = new Socket(_ipV4Address.AddressFamily, SocketType.Stream, ProtocolType.IP);
            _ipV6Socket = new Socket(_ipV6Address.AddressFamily, SocketType.Stream, ProtocolType.IP);


        }

		private static void OnTimedEvent(Object source, ElapsedEventArgs e)
		{
			Console.WriteLine("The Elapsed event was raised at {0:HH:mm:ss.fff}",
				e.SignalTime);
		}

		private static System.Timers.Timer aTimer;

		private static void SetTimer()
		{
			// Create a timer with a two second interval.
			aTimer = new System.Timers.Timer(2000);
			// Hook up the Elapsed event for the timer. 
			aTimer.Elapsed += OnTimedEvent;
			aTimer.AutoReset = true;
			aTimer.Enabled = true;
		}

		public static void DelayAction(int millisecond, Action action)
		{
			var timer = new DispatcherTimer();
			timer.Tick += delegate
			{
				action.Invoke();
				timer.Stop();
			};

			timer.Interval = TimeSpan.FromMilliseconds(millisecond);
			timer.Start();
		}

		[Test]
		[Ignore]
		public void ConnectionToMockDOPsServer()
		{
			var mockDOPsServer = new MockDOPsServer ();
			mockDOPsServer.Start(); // start mock DOPs Server

			// connect to mock DOPs server
			ClientWebSocket clientSocket = new ClientWebSocket ();
			clientSocket.ConnectAsync (new Uri ("ws://localhost:8046/websockets/MockServerEndpoint"), CancellationToken.None);

			bool bStateConnection=false;
			if(clientSocket.State == WebSocketState.Connecting)
				bStateConnection = true;

			// wait for release
			mockDOPsServer.WaitForStop (0); 

			// verify communication with server
			Assert.IsTrue(bStateConnection); // did we have a connection?
		}

		[Test]
		[Ignore]
		public async void SendBLOBToMockDOPsServer()
		{
			var mockDOPsServer = new MockDOPsServer ();
			mockDOPsServer.Start(); // start mock DOPs Server

 			// connect to mock DOPs server
			ClientWebSocket clientSocket = new ClientWebSocket ();
			clientSocket.ConnectAsync (new Uri ("ws://localhost:8046/websockets/MockServerEndpoint"), CancellationToken.None);

			bool bStateConnection=false;
			if(clientSocket.State == WebSocketState.Connecting)
				bStateConnection = true;

			while (clientSocket.State == WebSocketState.Connecting); // wait until socket is in open state

			bool bStateOpen = false;
			if (clientSocket.State == WebSocketState.Open)
				bStateOpen = true;

			// Send blob to server
			var ob = new ArraySegment<byte>(Encoding.UTF8.GetBytes("hello")); 
			try {
				await clientSocket.SendAsync(ob, WebSocketMessageType.Binary, false, CancellationToken.None);
			}catch(Exception e) {
				Console.WriteLine ("ERROR: Exception %s", e.ToString());
			}

			// wait for release
			mockDOPsServer.WaitForStop (0); 

			// verify communication with server
			Assert.IsTrue(bStateConnection); 	// did we have a connection?
			Assert.IsTrue(bStateOpen); 			// did we have an open socket?
		}

		[Test]
		public async void SendReceiveBLOBMockDOPsServer()
		{
			var mockDOPsServer = new MockDOPsServer ();
			mockDOPsServer.Start(); // start mock DOPs Server

			// connect to mock DOPs server
			Client.Connect ("ws://localhost:8046/websockets/MockServerEndpoint");

			Client.SendBLOB ();

			Thread.Sleep (1000);

			// wait for release
			//mockDOPsServer.WaitForStop (0); 

			// verify communication with server
			//Assert.IsTrue(bStateConnection); 	// did we have a connection?
			//Assert.IsTrue(bStateOpen); 			// did we have an open socket?
		}

        [Test]
        public void ShouldStart()
        {
            var socketMock = _repository.Create<ISocket>();

            _server.ListenerSocket = socketMock.Object;
            _server.Start(connection => { });

            socketMock.Verify(s => s.Bind(It.Is<IPEndPoint>(i => i.Port == 8000)));
            socketMock.Verify(s => s.Accept(It.IsAny<Action<ISocket>>(), It.IsAny<Action<Exception>>()));
        }

        [Test]
        public void ShouldFailToParseIPAddressOfLocation()
        {
            Assert.Throws(typeof(FormatException), () => {
                new WebSocketServer("ws://localhost:8000");
            });
        }

        [Test]
        public void ShouldBeSecureWithWssAndCertificate()
        {
            var server = new WebSocketServer("wss://0.0.0.0:8000");
            server.Certificate = new X509Certificate2();
            Assert.IsTrue(server.IsSecure);
        }

        [Test]
        public void ShouldDefaultToNoneWithWssAndCertificate()
        {
            var server = new WebSocketServer("wss://0.0.0.0:8000");
            server.Certificate = new X509Certificate2();
            Assert.AreEqual(server.EnabledSslProtocols, SslProtocols.None);
        }

        [Test]
        public void ShouldNotBeSecureWithWssAndNoCertificate()
        {
            var server = new WebSocketServer("wss://0.0.0.0:8000");
            Assert.IsFalse(server.IsSecure);
        }

        [Test]
        public void ShouldNotBeSecureWithoutWssAndCertificate()
        {
            var server = new WebSocketServer("ws://0.0.0.0:8000");
            server.Certificate = new X509Certificate2();
            Assert.IsFalse(server.IsSecure);
        }

        [Test]
        public void ShouldSupportDualStackListenWhenServerV4All()
        {
            _server = new WebSocketServer("ws://0.0.0.0:8000");
            _server.Start(connection => { });
            _ipV4Socket.Connect(_ipV4Address, 8000);
            _ipV6Socket.Connect(_ipV6Address, 8000);
        }

        #if __MonoCS__
          // None
        #else

            [Test]
            public void ShouldSupportDualStackListenWhenServerV6All()
            {
                _server = new WebSocketServer("ws://[::]:8000");
                _server.Start(connection => { });
                _ipV4Socket.Connect(_ipV4Address, 8000);
                _ipV6Socket.Connect(_ipV6Address, 8000);
            }

        #endif

        [TearDown]
        public void TearDown()
        {
            _ipV4Socket.Dispose();
            _ipV6Socket.Dispose();
            _server.Dispose();
        }
    }
}
