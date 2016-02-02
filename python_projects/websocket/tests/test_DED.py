# -*- coding: utf-8 -*-
#

import six
import sys
sys.path[0:0] = [""]

import os
import os.path
import base64
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

import uuid

if six.PY3:
    from base64 import decodebytes as base64decode
else:
    from base64 import decodestring as base64decode


class DEDTest(unittest.TestCase):

    def testINIT(self):
        self.assertEqual(True, False)