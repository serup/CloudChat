# -*- coding: utf-8 -*-
#
import sys
from array import array

import six

from dops.protocol.compression import lzss

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


class LZSSTest(unittest.TestCase):

    def testCompression(self):
        s = array('B', 'ab\0cdeeeeeeeeeeefffffffffggggggggg')
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
