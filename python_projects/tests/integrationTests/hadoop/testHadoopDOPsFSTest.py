import unittest
import subprocess
# import hadoop  # only used if app is running on a hadoop node
# import pydoop.hdfs
from webhdfs.webhdfs import WebHDFS
import os, tempfile

import pyhdfs
from pyhdfs import HdfsClient
from hdfs import *

from pyhdf.HDF import *
from pyhdf.VS import *

host = "one.cluster"
port = 8020


def read_local_dir(local_path):
    for fn in os.listdir(local_path):
        path = os.path.join(local_path, fn)
        if os.path.isfile(path):
            yield path, open(path).read()


#  hdfs://one.cluster:8020/
# def copyToHdfs(local_path, hdfs_path):
#    hdfsobj = hdfs.hdfs(host, port, user='vagrant', groups=['vagrant'])
#    hdfsobj.copy(local_path, hdfsobj, hdfs_path)


class HadoopTest(unittest.TestCase):

    def test_connect_hdfs(self):
        #fs = hadoop.HadoopDFS("vagrant", "vagrant", "one.cluster", 8020)
        #ds = fs.listDirectory("/tmp")
        #for i in ds:
        #     print i.mName
        #fs.disconnect()
        client = Client('http://one.cluster:8020')
        status = client.status('/tmp')
        print status



    def test_list_status(self):
        # client = HdfsClient(hosts='namenode1.example.com:50070,namenode2.example.com:50070')
        # client = HdfsClient(hosts='one.cluster:50070')
        client = HdfsClient('one.cluster:8020')
        print(client.listdir('/tmp'))
#        print(client.list_status('//one.cluster:50070'))

    def test_copyToHdfs(self):
        open_file_for_reading_vdata = HDF("your_input_file.hdf", HC.READ).vstart()
        vdata = open_file_for_reading_vdata.vdatainfo()
        print vdata

    def test_createfile_in_hdfs(self):
        f = pyhdfs.open()
        fs = pyhdfs.connect(host, port)
        # pyhdfs.get(fs, "/path/to/remote-src-file", "/path/to/local-dst-file")

        f = pyhdfs.open(fs, "/user/wormtongue/quotes.txt", "w")
        pyhdfs.write(fs, f, "Why do you lay these troubles on an already troubled mind?")
        pyhdfs.close(fs, f)

        pyhdfs.disconnect(fs)


    def test_hdfs_ls(self):
        # ls = subprocess.Popen(["hadoop", "fs", "-ls", "/tmp"], stdout=subprocess.PIPE)
        ls = subprocess.Popen(["hadoop", "rpc://one.cluster:8020", "-l3"], stdout=subprocess.PIPE)

        for line in ls.stdout:
            print line
        hdfs = 0
        self.assertTrue(False, hdfs != 0)  # TODO: hadoop fs -ls /

    def test_hdfs_webhdfs_upload_file(self):

        webhdfs = WebHDFS("one.cluster", 50070, "vagrant")
        webhdfs.mkdir("/tmp/hello-world/")

        # create a temporary file
        f = tempfile.NamedTemporaryFile()
        f.write(b'Hello world!\n')
        f.flush()

        print "Upload file: " + f.name

        webhdfs.copyFromLocal(f.name, "/hello-world/test3.txt")
        webhdfs.copyToLocal("/hello-world/test3.txt", "/tmp/test3.txt")

        for i in webhdfs.listdir("/hello-world/"):
            print str(i)

        f.close()

    def test_hdfs_webhdfs_ls(self):

        webhdfs = WebHDFS("one.cluster", 50070, "vagrant")
        for i in webhdfs.listdir("/tmp"):
            print str(i)

