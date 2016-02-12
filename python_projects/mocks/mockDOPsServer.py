#!/usr/bin/python
# -*- coding: utf-8 -*-

# /*********************************************************************************************************************
# mock of the DOPs Server - simulates handling of incoming DED packets
#  <u><font color="red">DOPsSever</font></u>.py -- mock of the DOPs Server - simulates handling of incoming DED packets
#    (tab = 4 spaces)
# **********************************************************************************************************************
#    11/2/2016 Johnny Serup
#    Use, distribute, and modify this program freely.
#    Please send me your improved versions.
# **********************************************************************************************************************/

#from websocketserver import websocketserver as ws

import sys
from threading import Thread
from ded import ded
import six

from ded import ded
from websocketserver.simplewebsocketserver import WebSocket, SimpleWebSocketServer
try:
    import Threading
except ImportError:  #TODO use Threading instead of _thread in python3
    import thread as thread

sys.path[0:0] = [""]

try:
    from ssl import SSLError
except ImportError:
    # dummy class of SSLError for ssl none-support environment.
    class SSLError(Exception):
        pass

if six.PY3:
    pass
else:
    pass

VER = sys.version_info[0]


def _check_unicode(val):
    if VER >= 3:
        return isinstance(val, str)
    else:
        return isinstance(val, unicode)


class DOPsServerHandling(WebSocket):

    def handleMessage(self):
        # check if data received is a DED packet
        DED = ded.DEDEncoder()
        DED.PUT_DATA_IN_DECODER(self.data, len(self.data))
        self.sendMessage(self.data)  # echo back the message

    def handleConnected(self):
        pass

    def handleClose(self):
        pass


class MyThread_DOPsServer(Thread):

    def __init__(self, host, port):
        ''' Constructor. '''

        Thread.__init__(self)
        self.port = port
        self.host = host
        self.server = SimpleWebSocketServer(self.host, self.port, DOPsServerHandling)

    def run(self):
        self.server.serveforMe()

    def stop(self):
        self.server.stopServeForMe()


class mockDOPsServer(object):

    myThreadServer = 0

    def __init__(self, host, port):
        self.host = host
        self.port = port
        super(mockDOPsServer, self).__init__()

    def startmockServer(self):
        self.myThreadServer = MyThread_DOPsServer(self.host, self.port)
        self.myThreadServer.setName('Thread DOPs Server')
        self.myThreadServer.start()

    def stopmockServer(self):
        self.myThreadServer.stop()
        self.myThreadServer.join(0)

