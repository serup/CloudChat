# -*- coding: utf-8 -*-
#
import sys
import unittest

from dops.protocol.compression import lzss
from dops.protocol.ded import ded

sys.path[0:0] = [""]


class DEDTest(unittest.TestCase):

    def testDED_START_ENCODER(self):
        DED = ded.DEDEncoder()
        encoder = DED.DED_START_ENCODER()
        self.assertTrue(True, encoder != 0)

    def testDED_PUT_STRUCT_START(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)

    def testDED_PUT_STRUCT_END(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_STRUCT_END("event")
        self.assertTrue(result > 0, result)

    def testDED_PUT_METHOD(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_METHOD("method", "mediaplayer")
        self.assertTrue(result > 0, result)

    def testDED_PUT_USHORT(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_USHORT("ushort", 1)
        self.assertTrue(result > 0, result)

    def testDED_PUT_LONG(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_LONG("long", 1)
        self.assertTrue(result > 0, result)

    def testDED_PUT_BOOL(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_BOOL("bool", 1)
        self.assertTrue(result > 0, result)

    def testDED_PUT_STDSTRING(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_STDSTRING("stdstring", "hello world")
        self.assertTrue(result > 0, result)

    def testDED_PUT_ELEMENT(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_ELEMENT("profile", "username",  "johndoe")
        self.assertTrue(result > 0, result)

    # DEDobject = {
    #	encoder_ptr: encoder_ptr,
    #	uncompresseddata: ,
    #	iLengthOfTotalData: ,
    #	pCompressedData: ,
    #	sizeofCompressedData:
    # }
    def testDataEncoder_GetData(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_ELEMENT("profile", "username",  "johndoe")
        self.assertTrue(result > 0, result)
        DEDobj = DED.DEDobject
        uncompresseddata = DED.DataEncoder_GetData(DEDobj)

        self.assertTrue(True, uncompresseddata != 0)

    def testDED_GET_ENCODED_DATA(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_ELEMENT("profile", "username",  "johndoe")
        self.assertTrue(result > 0, result)
        DEDobj = DED.GET_ENCODED_DATA()
        self.assertTrue(DEDobj.uncompresseddata > 0, DEDobj.uncompresseddata)
        self.assertTrue(DEDobj.pCompressedData > 0, DEDobj.pCompressedData)

        # verify that the compressed data is same as uncompressed when decompressed
        tmpdecode = bytearray(lzss.decode(DEDobj.pCompressedData, 0, len(DEDobj.pCompressedData)))
        self.assertTrue(DEDobj.uncompresseddata == tmpdecode, tmpdecode)

    def testPUT_DATA_IN_DECODER(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_ELEMENT("profile", "username",  "johndoe")
        self.assertTrue(result > 0, result)
        DEDobj = DED.GET_ENCODED_DATA()

        # simulate transmitting data ....
        # simulate receiving data ....

        DED2 = ded.DEDEncoder()
        DED2.PUT_DATA_IN_DECODER(DEDobj.pCompressedData)
        # verify that data is inside decoder, and that it has been decompressed correct
        self.assertTrue(True, DED2.ptotaldata == DEDobj.uncompresseddata)

    def testGET_STRUCT_START(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_ELEMENT("profile", "username",  "johndoe")
        self.assertTrue(result > 0, result)
        DEDobj = DED.GET_ENCODED_DATA()

        # simulate transmitting data ....
        # simulate receiving data ....

        DED2 = ded.DEDEncoder()
        DED2.PUT_DATA_IN_DECODER(DEDobj.pCompressedData)
        # verify that data is inside decoder, and that it has been decompressed correct
        self.assertTrue(DED2.ptotaldata, DEDobj.uncompresseddata)

        # start decoding
        result = DED2.GET_STRUCT_START("event")
        self.assertTrue(result > 0, result)

    def testGET_METHOD(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_METHOD("method", "mediaplayer")
        self.assertTrue(result > 0, result)
        DEDobj = DED.GET_ENCODED_DATA()

        # simulate transmitting data ....
        # simulate receiving data ....

        DED2 = ded.DEDEncoder()
        DED2.PUT_DATA_IN_DECODER(DEDobj.pCompressedData)
        # verify that data is inside decoder, and that it has been decompressed correct
        self.assertTrue(True, DED2.ptotaldata == DEDobj.uncompresseddata)

        # start decoding
        result = DED2.GET_STRUCT_START("event")
        self.assertTrue(True, result > 0)

        result = DED2.GET_METHOD("method")
        self.assertEquals("mediaplayer", result)

    def testGET_USHORT(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_USHORT("ushort", 4)
        self.assertTrue(result > 0, result)
        DEDobj = DED.GET_ENCODED_DATA()

        # simulate transmitting data ....
        # simulate receiving data ....

        DED2 = ded.DEDEncoder()
        DED2.PUT_DATA_IN_DECODER(DEDobj.pCompressedData)
        # verify that data is inside decoder, and that it has been decompressed correct
        self.assertTrue(True, DED2.ptotaldata == DEDobj.uncompresseddata)

        # start decoding
        result = DED2.GET_STRUCT_START("event")
        self.assertTrue(True, result > 0)

        DED2.endianness = 'be';   # necessary for this test, since default is little endian

        result = DED2.GET_USHORT("ushort")
        self.assertEquals(4, result)

    def testGET_LONG(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        number = 9223372036854775807
        result = DED.PUT_LONG("long", number)
        self.assertTrue(result > 0, result)
        DEDobj = DED.GET_ENCODED_DATA()

        # simulate transmitting data ....
        # simulate receiving data ....

        DED2 = ded.DEDEncoder()
        DED2.PUT_DATA_IN_DECODER(DEDobj.pCompressedData)
        # verify that data is inside decoder, and that it has been decompressed correct
        self.assertTrue(True, DED2.ptotaldata == DEDobj.uncompresseddata)

        # start decoding
        DED2.endianness = 'be';   # necessary for this test, since default is little endian
        result = DED2.GET_STRUCT_START("event")
        self.assertTrue(True, result > 0)

        result = DED2.GET_LONG("long")
        self.assertEquals(number, result)

    def testGET_BOOL(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        number = True
        result = DED.PUT_BOOL("bool", number)
        self.assertTrue(result > 0, result)
        DEDobj = DED.GET_ENCODED_DATA()

        # simulate transmitting data ....
        # simulate receiving data ....

        DED2 = ded.DEDEncoder()
        DED2.PUT_DATA_IN_DECODER(DEDobj.pCompressedData)
        # verify that data is inside decoder, and that it has been decompressed correct
        self.assertTrue(True, DED2.ptotaldata == DEDobj.uncompresseddata)

        # start decoding
        result = DED2.GET_STRUCT_START("event")
        self.assertTrue(True, result > 0)

        result = DED2.GET_BOOL("bool")
        self.assertEquals(number, result)

    def testGET_STDSTRING(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_STDSTRING("stdstring", "hello world")
        self.assertTrue(result > 0, result)
        DEDobj = DED.GET_ENCODED_DATA()

        # simulate transmitting data ....
        # simulate receiving data ....

        DED2 = ded.DEDEncoder()
        DED2.PUT_DATA_IN_DECODER(DEDobj.pCompressedData)
        # verify that data is inside decoder, and that it has been decompressed correct
        self.assertTrue(True, DED2.ptotaldata == DEDobj.uncompresseddata)

        # start decoding
        result = DED2.GET_STRUCT_START("event")
        self.assertTrue(True, result > 0)

        result = DED2.GET_STDSTRING("stdstring")
        self.assertEquals("hello world", result)

    def testGET_ELEMENT(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START("event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_ELEMENT("profile", "username",  "johndoe")
        self.assertTrue(result > 0, result)

        DEDobj = DED.GET_ENCODED_DATA()

        # simulate transmitting data ....
        # simulate receiving data ....

        DED2 = ded.DEDEncoder()
        DED2.PUT_DATA_IN_DECODER(DEDobj.pCompressedData)
        # verify that data is inside decoder, and that it has been decompressed correct
        self.assertTrue(True, DED2.ptotaldata == DEDobj.uncompresseddata)

        # start decoding
        result = DED2.GET_STRUCT_START("event")
        self.assertTrue(True, result > 0)

        result = DED2.GET_ELEMENT("profile")
        self.assertEquals("username", result.strElementID)
        self.assertEquals("johndoe", result.ElementData)

    def testGET_STRUCT_END(self):
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

        # simulate transmitting data ....
        # simulate receiving data ....

        DED2 = ded.DEDEncoder()
        DED2.PUT_DATA_IN_DECODER(DEDobj.pCompressedData)
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
