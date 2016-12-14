# -*- coding: utf-8 -*-
#
from dops.protocol.utils import utils
import sys
import unittest
from array import array
import os

sys.path[0:0] = [""]


class showDataBlockTest(unittest.TestCase):

    def testShowDataBlock(self):
        util = utils.CUtils()
        s = array('B', 'RAabB\0\0\0cdeeeeeeeeeeefffffffffgggggggggggä½ å¥½å•Šä¸–ç•ŒgggggggggggggggggggggssdfasdfaseaadfdssfsbbbbBBBBB')

        #util.showdatablock(s)
        str = util.hexdump_to_string(s)
        print str
        print
        str = util.hexdump_to_html_string(s)
        print str
        print

        cwd = os.getcwd()
        print cwd
        print
        filepathname = cwd+"/datafile.binary"
        strfile = util.hexdump_file_to_string(filepathname)

        # NB! this will have 0A at the end for end of file marker
        print strfile
        print
        strfilehtml = util.hexdump_file_to_html_string(filepathname)
        print strfilehtml

        self.assertEqual(True, len(s) > 0)

