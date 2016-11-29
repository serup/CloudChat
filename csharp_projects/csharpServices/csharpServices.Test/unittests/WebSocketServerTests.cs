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
using Fleck;


namespace WebSocketTests
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
		public void SendReceiveBLOBMockDOPsServer()
		{
			// instantiate the mock server
			var mockDOPsServer = new MockDOPsServer (8046,"/websockets/MockServerEndpoint");

			// start mock DOPs Server
			mockDOPsServer.Start();

			// connect to DOPs Server
			Client.wshandles _handles = Client.WSConnect ("ws://127.0.0.1:8046/websockets/MockServerEndpoint");

			// create random bytes
			var random = new Random();
			byte[] byteArray = new byte[64];
			random.NextBytes(byteArray);

			while(_handles.webSocket.State != WebSocketState.Open);

			// Send the blob containing the random bytes to Server
			Client.SendBLOB (byteArray,_handles.webSocket); 

			// Fetch the return data from the Server - in this case it should be an echo
			byte[] ReceiveBuffer = Client.FetchReceived (_handles);
//			byte[] ReceiveBuffer = Client.FetchReceived();

			// verify 
			Assert.IsNotNull (ReceiveBuffer);
			Assert.IsTrue(byteArray.SequenceEqual(ReceiveBuffer));

			// wait for release
			mockDOPsServer.WaitForStop (1); 
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
