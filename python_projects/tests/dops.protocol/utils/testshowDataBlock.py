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
        s = array('B', 'RAabB\0\0\0cdeeeeeeeeeeefffffffffgggggggggggä½ å¥½å•Šä¸–ç•Œgggggggggggggggg<gggggssdfasdfaseaadfdssfsbbbbBBBB<B')

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

    def testWritePNG(self):
        util = utils.CUtils()

        buf = b'RAabB\0\0\0cdeeeeeeeeeeefffffffffgggggggggggä½ å¥½å•Šä¸–ç•Œgggggggggggggggg<gggggssdfasdfaseaadfdssfsbbbbBBBB<B<8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b>'

        #buf = b'<8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b><8b>'

        data = util.write_png(buf, 64, 64)
        with open("my_image.png", 'wb') as fd:
            fd.write(data)

        print " my_image.png filesize %d " % os.stat("my_image.png").st_size

    def testWriteBinFileAsPNG(self):
        util = utils.CUtils()

        with open('datafile.binary', 'rb') as infile:
             buf = infile.read(os.stat("datafile.binary").st_size)

        data = util.write_png(buf, 128, 128)
        with open('my_image.png', 'wb') as fd:
             fd.write(data)

    def testtastefile(self):
        util = utils.CUtils()
        self.assertEqual(True, util.is_binay_file('datafile.binary'))
