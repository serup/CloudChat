import sys
import unittest

sys.path[0:0] = [""]


class DEDTest(unittest.TestCase):

    def test_hdfs_ls(self):
        hdfs = 0;
        self.assertTrue(False, hdfs != 0)  # TODO: hadoop fs -ls /
