# -*- coding: utf-8 -*-
#
import sys
from array import array

import six

from dops.protocol.compression import lzss
from dops.protocol.utils import utils

sys.path[0:0] = [""]

try:
    from ssl import SSLError
except ImportError:
    # dummy class of SSLError for ssl none-support environment.
    class SSLError(Exception):
        pass

if sys.version_info[0] == 2 and sys.version_info[1] < 7:
    import unittest2 as unittest
else:
    import unittest

if six.PY3:
    pass
else:
    pass


class showDataBlockTest(unittest.TestCase):

    def testInit(self):
        s = array('B', 'ab\0\0\0cdeeeeeeeeeeefffffffffggggggggg')
        print 'Before compress  : ' + s.tostring()
        sEncoded = lzss.encode(s, 0, len(s))
        print 'After compress   : ' + sEncoded.tostring()
        sDecoded = lzss.decode(sEncoded, 0, len(sEncoded))
        print 'After decompress : ' + sDecoded.tostring()

        #
        # NB! pt. unicode encoding does NOT work
        #        s = lzss.encode(array('B', codecs.encode('ä½ å¥½å•Šä¸–ç•Œ','gbk')), 2, 8)
        #        s = lzss.encode(array('B', codecs.encode('Hej med dig Unicode','gbk')), 2, 8)
        #        print codecs.decode(lzss.decode(s, 0, len(s)).tostring(), 'gbk')

        self.assertEqual(True, s == sDecoded)

    def testShowDataBlock(self):
        util = utils.CUtils()
        s = array('B', 'AabB\0cdeeeeeeeeeeefffffffffggggggggg')

        util.showdatablock(s, len(s))
        self.assertEqual(True, len(s) > 0)

