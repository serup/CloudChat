# -*- coding: utf-8 -*-
#
from dops.protocol.utils import utils
import sys
import unittest
from array import array

sys.path[0:0] = [""]


class showDataBlockTest(unittest.TestCase):

    def testShowDataBlock(self):
        util = utils.CUtils()
        s = array('B', 'RAabB\0\0\0cdeeeeeeeeeeefffffffffgggggggggggä½ å¥½å•Šä¸–ç•ŒgggggggggggggggggggggssdfasdfaseaadfdssfsbbbbBBBBB')

        #util.showdatablock(s)

        str = util.hexdump_to_string(s)
        print str

        self.assertEqual(True, len(s) > 0)

