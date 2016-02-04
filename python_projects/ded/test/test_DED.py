# -*- coding: utf-8 -*-
#
from ded import ded
import unittest
import sys
sys.path[0:0] = [""]


class DEDTest(unittest.TestCase):

    def testDED_START_ENCODER(self):
        DED = ded.DEDEncoder()
        encoder = DED.DED_START_ENCODER()
        self.assertTrue(True, encoder != 0)

    def testDED_PUT_STRUCT_START(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START(DED, "event")
        self.assertTrue(result == 1, result)

    def testDED_PUT_STRUCT_END(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START(DED, "event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_STRUCT_END(DED, "event")
        self.assertTrue(result > 0, result)

    def testDED_PUT_METHOD(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START(DED, "event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_METHOD(DED, "method", "mediaplayer")
        self.assertTrue(result > 0, result)

    def testDED_PUT_USHORT(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START(DED, "event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_USHORT(DED, "ushort", 1)
        self.assertTrue(result > 0, result)

    def testDED_PUT_LONG(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START(DED, "event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_LONG(DED, "long", 1)
        self.assertTrue(result > 0, result)

    def testDED_PUT_BOOL(self):
        DED = ded.DEDEncoder()
        result = DED.PUT_STRUCT_START(DED, "event")
        self.assertTrue(result == 1, result)
        result = DED.PUT_BOOL(DED, "bool", 1)
        self.assertTrue(result > 0, result)

