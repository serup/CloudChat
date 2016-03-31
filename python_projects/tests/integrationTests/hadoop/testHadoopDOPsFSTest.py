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

        # should now be a file on node one.cluster --
        # vagrant ssh one
        # hadoop fs -ls /tmp
        # should show something like this
        # [vagrant@one ~]$ hadoop fs -ls /tmp
        # Found 8 items
        # drwxr-xr-x   - vagrant   hdfs           0 2016-03-11 11:42 /tmp/hello-world
        # -rwx------   3 ambari-qa hdfs        1472 2016-03-11 10:17 /tmp/ida8c06600_date171116
        # -rwx------   3 ambari-qa hdfs        1472 2016-03-11 09:52 /tmp/ida8c06600_date501116
        # -rwx------   3 ambari-qa hdfs        1472 2016-02-29 11:58 /tmp/ida8c06600_date582916
        # -rwxr-xr-x   3 serup     serup       3690 2016-03-01 07:52 /tmp/old
        # -rwxr-xr-x   3 serup     hdfs        3690 2016-03-01 08:38 /tmp/old2
        # -rwxr-xr-x   3 serup     hdfs        3690 2016-03-01 10:43 /tmp/old3
        # -rwxr-xr-x   1 vagrant   hdfs          13 2016-03-11 11:57 /tmp/test.txt
        # [vagrant@one ~]$

        print "Upload file: " + f.name

        webhdfs.copyFromLocal(f.name, "/tmp/hello-world/test8.txt")
        f.close()
        webhdfs.copyToLocal("/tmp/hello-world/test8.txt", "/tmp/testresult.txt")  #  WILL reside on your local machine in /tmp/folder - NOT on hadoop node

        for i in webhdfs.listdir("/tmp/hello-world/"):
            print str(i)


    def test_hdfs_webhdfs_ls(self):

        webhdfs = WebHDFS("one.cluster", 50070, "vagrant")
        for i in webhdfs.listdir('/'):
            print str(i)

