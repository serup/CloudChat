# -*- coding: utf-8 -*-
#
from ded import ded
from mocks import mockDOPsServer
#from websocketserver import websocketclient as wsclient
import sys
import websocket
import subprocess
import time

sys.path[0:0] = [""]

if sys.version_info[0] == 2 and sys.version_info[1] < 7:
    import unittest2 as unittest
else:
    import unittest


class DOPsServerTest(unittest.TestCase):
    cmdkill = "kill $(ps aux|grep 'DOPsServerTest\|mock'|grep -v 'grep'|awk '{print $2}') 2> /dev/null"
    DOPsServer = 0
    #host = 'ws://127.0.0.1:9876'
    host = '127.0.0.1'
    port = 9876

    def doCleanups(self):
        subprocess.Popen(self.cmdkill, stdout=subprocess.PIPE, shell=True)

    def setUp(self):
        self.DOPsServer = mockDOPsServer.mockDOPsServer(self.host, self.port)
        self.DOPsServer.startmockServer()
        super(DOPsServerTest, self).setUp()

    def tearDown(self):
        self.DOPsServer.stopmockServer()
        self.doCleanups()
        super(DOPsServerTest, self).tearDown()

    def testInitDOPsServer(self):
        _bool = True
        number = 9223372036854775807

        DED = ded.DEDEncoder()
        if DED.PUT_STRUCT_START("event"):
            DED.PUT_METHOD("method", "mediaplayer")
            DED.PUT_ELEMENT("profile", "username",  "johndoe")
            DED.PUT_STDSTRING("stdstring", "hello world")
            DED.PUT_BOOL("bool", _bool)
            DED.PUT_LONG("long", number)
            DED.PUT_USHORT("ushort", 4)
        DED.PUT_STRUCT_END("event")
        DEDobj = DED.GET_ENCODED_DATA()

        # transmitting data to mock DOPs Server
        ws = websocket.WebSocket()
        ws.connect("ws://127.0.0.1:9876")
        ws.send_binary(DEDobj.pCompressedData)
        result = ws.recv()
        print("Received '%s'" % result)
        ws.close()

        DED2 = ded.DEDEncoder()
        DED2.PUT_DATA_IN_DECODER(bytearray(result), len(bytearray(result)))
        # verify that data is inside decoder, and that it has been decompressed correct
        self.assertTrue(DED2.ptotaldata, DEDobj.uncompresseddata)

        # start decoding
        if DED2.GET_STRUCT_START("event"):
            a = DED2.GET_METHOD("method")
            b = DED2.GET_ELEMENT("profile")
            c = DED2.GET_STDSTRING("stdstring")
            d = DED2.GET_BOOL("bool")
            e = DED2.GET_LONG("long")
            f = DED2.GET_USHORT("ushort")
        result = DED2.GET_STRUCT_END("event")
        self.assertEquals(result > 0, result)
        self.assertEquals(a,"mediaplayer")
        self.assertEquals(b.strElementID, "username")
        self.assertEquals(b.ElementData, "johndoe")
        self.assertEquals(c, "hello world")
        self.assertEquals(d, True)
        self.assertEquals(e, number)
        self.assertEquals(f, 4)

    def testCreateProfile(self):
        trans_id = 123
        uniqueId = "a12def123"
        username = "johndoe"
        password = "abc123"

        # TODO: make a DED package for create profile

        DED = ded.DEDEncoder()
        if DED.PUT_STRUCT_START("WSRequest"):
            DED.PUT_METHOD   ("name",  "PythonConnect")
            DED.PUT_USHORT   ("trans_id",  trans_id)
            DED.PUT_STDSTRING("protocolTypeID", "DED1.00.00")
            DED.PUT_STDSTRING("functionName", uniqueId)
            DED.PUT_STDSTRING("username", username)
            DED.PUT_STDSTRING("password", password)
        DED.PUT_STRUCT_END("WSRequest")
        DEDobj = DED.GET_ENCODED_DATA()

        # transmitting data to mock DOPs Server
        ws = websocket.WebSocket()
        ws.connect("ws://127.0.0.1:9876")
        ws.send_binary(DEDobj.pCompressedData)
        receivedData = ws.recv()
        ws.close()

        DED2 = ded.DEDEncoder()
        DED2.PUT_DATA_IN_DECODER(bytearray(receivedData), len(bytearray(receivedData)))
        # verify that data is inside decoder, and that it has been decompressed correct
        self.assertTrue(DED2.ptotaldata, DEDobj.uncompresseddata)

        # start decoding
        if DED2.GET_STRUCT_START("WSResponse"):
                strMethod   = DED2.GET_METHOD ("name" )
                uTrans_id   = DED2.GET_USHORT ("trans_id")
                strProtocolTypeID  = DED2.GET_STDSTRING ("protocolTypeID")
                strFunctionName    = DED2.GET_STDSTRING ("functionName")
                strStatus  = DED2.GET_STDSTRING ("status")
                DED2.GET_STRUCT_END( "WSResponse" )
                bDecoded = True
                print("DED packet decoded - now validate")

                if strMethod != "PythonConnect": bDecoded = False
                self.assertEquals(True, bDecoded)
                if uTrans_id != trans_id: bDecoded = False
                self.assertEquals(True, bDecoded)
                if strProtocolTypeID != "DED1.00.00": bDecoded = False
                self.assertEquals(True, bDecoded)
                if strFunctionName != uniqueId: bDecoded = False
                self.assertEquals(True, bDecoded)
                if strStatus != "ACCEPTED": bDecoded = False
                self.assertEquals(True, bDecoded)

                if bDecoded:
                    print("DED packet validated - OK")
        else:
                bDecoded = False
