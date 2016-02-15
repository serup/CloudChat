# -*- coding: utf-8 -*-
#
import uuid

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

    # 'ws://127.0.0.1:9876'
    host = '127.0.0.1'
    port = 9876
    # start the mock server
    DOPsServer = mockDOPsServer.mockDOPsServer(host, port)
    DOPsServer.startmockServer()

    def doCleanups(self):
        cmdkill = "kill $(ps aux|grep 'DOPsServerTest\|mock'|grep -v 'grep'|awk '{print $2}') 2> /dev/null"
        subprocess.Popen(cmdkill, stdout=subprocess.PIPE, shell=True)

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
        DED2.PUT_DATA_IN_DECODER(result)
        # verify that data is inside decoder, and that it has been decompressed correct
        self.assertTrue(DED2.ptotaldata, DEDobj.uncompresseddata)

        # start decoding
        if DED2.GET_STRUCT_START("event"):
            self.assertEquals(DED2.GET_METHOD("method"), "mediaplayer")
            b = DED2.GET_ELEMENT("profile")
            self.assertEquals(b.strElementID, "username")
            self.assertEquals(b.ElementData, "johndoe")
            self.assertEquals(DED2.GET_STDSTRING("stdstring"), "hello world")
            self.assertEquals(DED2.GET_BOOL("bool"), True)
            self.assertEquals(DED2.GET_LONG("long"), number)
            self.assertEquals(DED2.GET_USHORT("ushort"), 4)
        result = DED2.GET_STRUCT_END("event")
        self.assertEquals(result > 0, result)

    def testPythonConnectToDOPsServer(self):
        trans_id = 69
        # hardcoded user in mock server
        uniqueId = "985998707DF048B2A796B44C89345494"
        username = "johndoe@email.com"
        password = "12345"

        DED = ded.DEDEncoder()
        if DED.PUT_STRUCT_START("WSRequest"):
            DED.PUT_METHOD("name",  "PythonConnect")
            DED.PUT_USHORT("trans_id",  trans_id)
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
        DED2.PUT_DATA_IN_DECODER(receivedData)
        # verify that data is inside decoder, and that it has been decompressed correct
        # self.assertTrue(DED2.ptotaldata, DEDobj.uncompresseddata)  # only if echo

        # start decoding
        if DED2.GET_STRUCT_START("WSResponse"):
            strMethod = DED2.GET_METHOD("name")
            uTrans_id = DED2.GET_USHORT("trans_id")
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

    def testCreateProfileInMockServer(self):
        bCreatedProfile = False
        trans_id = 66
        EntityFileName = str(uuid.uuid4()).replace("-", "")
        EntityTOASTFileName = str(uuid.uuid4()).replace("-", "")

        DED = ded.DEDEncoder()
        if DED.PUT_STRUCT_START("DFDRequest"):
            # + fixed area start
            DED.PUT_METHOD("Method",  "1_1_1_CreateProfile")
            DED.PUT_USHORT("TransID", trans_id)
            DED.PUT_STDSTRING("protocolTypeID", "DED1.00.00")
            DED.PUT_STDSTRING("dest", "DFD_1.1")
            DED.PUT_STDSTRING("src", "pythonclient")  # should be this client
            # - fixed area end
            # + Profile record area start
            DED.PUT_STDSTRING("STARTrequest", "EmployeeRequest")
            DED.PUT_STDSTRING("STARTrecord", "record")
            DED.PUT_STDSTRING("profileID", EntityFileName)
            DED.PUT_STDSTRING("profileName", "TestProfile")
            DED.PUT_STDSTRING("protocolTypeID", "DED1.00.00")
            DED.PUT_STDSTRING("sizeofProfileData", "0")
            DED.PUT_STDSTRING("profile_chunk_id", EntityTOASTFileName)
            DED.PUT_STDSTRING("AccountStatus", "1")
            DED.PUT_STDSTRING("ExpireDate", "20160101")
            DED.PUT_STDSTRING("ProfileStatus", "2")
            DED.PUT_STDSTRING("STARTtoast", "elements")
            #   + start toast elements
            DED.PUT_ELEMENT("profile", "lifecyclestate", "1")  # will add profile_chunk_id and profile_chunk_data
            DED.PUT_ELEMENT("profile", "username", "serup")
            DED.PUT_ELEMENT("profile", "password", "sussiskoller")
            DED.PUT_ELEMENT("profile", "devicelist", " ")
            DED.PUT_ELEMENT("profile", "firstname", "Johnny")
            DED.PUT_ELEMENT("profile", "lastname", "Serup")
            DED.PUT_ELEMENT("profile", "streetname", "...")
            DED.PUT_ELEMENT("profile", "streetno", "...")
            DED.PUT_ELEMENT("profile", "postalcode", "...")
            DED.PUT_ELEMENT("profile", "city", "...")
            DED.PUT_ELEMENT("profile", "state", "...")
            DED.PUT_ELEMENT("profile", "country", "...")
            DED.PUT_ELEMENT("profile", "email", "...")
            DED.PUT_ELEMENT("profile", "mobilephone", "...")
            DED.PUT_ELEMENT("profile", "title", "...")
            DED.PUT_ELEMENT("profile", "about", "...")
            DED.PUT_ELEMENT("profile", "foto", "data:image/jpeg;base64,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" )
            #   - end toast elements
            DED.PUT_STDSTRING("ENDtoast", "elements")
            #  - Profile record area end
            DED.PUT_STDSTRING("ENDrecord", "record")
            DED.PUT_STDSTRING("ENDrequest", "EmployeeRequest")
            # - Profile request area end
            DED.PUT_STRUCT_END("DFDRequest")
            DEDobj = DED.GET_ENCODED_DATA()

            # transmitting data to mock DOPs Server
            ws = websocket.WebSocket()
            ws.connect("ws://" + self.host + ":" + str(self.port))
            ws.send_binary(DEDobj.pCompressedData)
            receivedData = ws.recv()
            # print("Received '%s'" % receivedData)
            ws.close()

            DED2 = ded.DEDEncoder()
            DED2.PUT_DATA_IN_DECODER(receivedData)

            if (DED2.GET_STRUCT_START("DFDResponse") == 1 and
                DED2.GET_METHOD("Method").__eq__("CreateProfile") and
                DED2.GET_USHORT("TransID") != -1 and
                DED2.GET_STDSTRING("protocolTypeID").__eq__("DED1.00.00") and
                DED2.GET_STDSTRING("dest").__sizeof__() > 0 and
                DED2.GET_STDSTRING("src").__sizeof__() > 0):

                strStatus = DED2.GET_STDSTRING("status")
                DED2.GET_STRUCT_END("DFDResponse")

                if strStatus == "Profile Saved in database":
                    bCreatedProfile = True
                    print "SUCCESS profile created in mock DOPS servers database"

        self.assertTrue(True, bCreatedProfile)
