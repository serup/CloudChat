# -*- coding: utf-8 -*-
#
from ded import ded
from mocks import mockDOPsServer
#from websocketserver import websocketclient as wsclient
import sys
import websocket

sys.path[0:0] = [""]

if sys.version_info[0] == 2 and sys.version_info[1] < 7:
    import unittest2 as unittest
else:
    import unittest


class DOPsServerTest(unittest.TestCase):
    DOPsServer = 0
    #host = 'ws://127.0.0.1:9876'
    host = '127.0.0.1'
    port = 9876

    def setUp(self):
        self.DOPsServer = mockDOPsServer.mockDOPsServer(self.host, self.port)
        super(DOPsServerTest, self).setUp()

    def tearDown(self):
        self.DOPsServer.stopmockServer()
        super(DOPsServerTest, self).tearDown()

    def testInitDOPsServer(self):
        _bool = True
        number = 9223372036854775807

        DED = ded.DEDEncoder()
        if DED.PUT_STRUCT_START(DED, "event"):
            DED.PUT_METHOD(DED, "method", "mediaplayer")
            DED.PUT_ELEMENT(DED, "profile", "username",  "johndoe")
            DED.PUT_STDSTRING(DED, "stdstring", "hello world")
            DED.PUT_BOOL(DED, "bool", _bool)
            DED.PUT_LONG(DED, "long", number)
            DED.PUT_USHORT(DED, "ushort", 4)
        DED.PUT_STRUCT_END(DED, "event")
        DEDobj = DED.GET_ENCODED_DATA()

        # transmitting data to mock DOPs Server
        #ws = websocket.WebSocket()
        #ws.connect("ws://127.0.0.1:9876")
        #ws.send("Hello, Server")
        #ws.recv()
        #ws.close()

        ws = websocket.WebSocket()
        ws.connect("ws://127.0.0.1:9876")
        ws.send("Hello world")
        result = ws.recv()
        print("Received '%s'" % result)
        ws.send_binary(DEDobj.pCompressedData)
        result = ws.recv()
        print("Received '%s'" % result)
        ws.close()

        self.assertTrue(True == False, False)  #  NOT READY YET


        DED2 = ded.DEDEncoder()
        DED2.PUT_DATA_IN_DECODER(DEDobj.pCompressedData, len(DEDobj.pCompressedData))
        # verify that data is inside decoder, and that it has been decompressed correct
        self.assertTrue(DED2.ptotaldata, DEDobj.uncompresseddata)

        # start decoding
        if DED2.GET_STRUCT_START("event"):
            DED2.GET_METHOD("method")
            DED2.GET_ELEMENT("profile")
            DED2.GET_STDSTRING("stdstring")
            DED2.GET_BOOL("bool")
            DED2.GET_LONG("long")
            DED2.GET_USHORT("ushort")
        result = DED2.GET_STRUCT_END("event")
        self.assertEquals(result > 0, result)
