import sys
import unittest
import websocket
from dops.protocol.ded import ded
sys.path[0:0] = [""]


class DOPsIntegrationTest(unittest.TestCase):

    def testPythonConnectToDOPsServer(self):
        trans_id = 69
        # TODO: hardcoded user in server
#        uniqueId = "985998707DF048B2A796B44C89345494"
#        username = "johndoe@email.com"
#        password = "12345"
        uniqueId = "985998707DF048B2A796B44C89345494"
        username = "johndoe@email.com" #  TODO: find a way to safely handle retrieval of username,password - should NOT be stored in source code
        password = "12345"

        print "Testing if DOPs Server can handle a pythonConnect DED datapacket"
        print "---------------------------------------------------------------------"

        DED = ded.DEDEncoder()
        if DED.PUT_STRUCT_START("WSRequest"):
            DED.PUT_METHOD("Method",  "PythonConnect")
            DED.PUT_USHORT("TransID",  trans_id)
            DED.PUT_STDSTRING("protocolTypeID", "DED1.00.00")
            DED.PUT_STDSTRING("functionName", uniqueId)
            DED.PUT_STDSTRING("username", username)
            DED.PUT_STDSTRING("password", password)
        DED.PUT_STRUCT_END("WSRequest")
        DEDobj = DED.GET_ENCODED_DATA()

        # transmitting data to mock DOPs Server
        ws = websocket.WebSocket()
        ws.connect("ws://backend.scanva.com:7777")
        # ws.connect("ws://localhost:7778")  # NB! make sure backend VM is NOT running or start local server with port 7778 - then you can debug server :-)
        self.assertEquals(True, ws.connected)
        ws.send_binary(DEDobj.pCompressedData)
        receivedData = ws.recv()
        ws.close()

        DED2 = ded.DEDEncoder()
        DED2.PUT_DATA_IN_DECODER(receivedData)
        # verify that data is inside decoder, and that it has been decompressed correct

        # start decoding
        if DED2.GET_STRUCT_START("WSResponse"):
            strMethod = DED2.GET_METHOD("Method")
            uTrans_id = DED2.GET_USHORT("TransID")
            strProtocolTypeID = DED2.GET_STDSTRING("protocolTypeID")
            strFunctionName = DED2.GET_STDSTRING("functionName")
            strStatus = DED2.GET_STDSTRING ("status")
            DED2.GET_STRUCT_END("WSResponse")
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