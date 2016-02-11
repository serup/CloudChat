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

from simplewebsocketserver import WebSocket, SimpleWebSocketServer, SimpleSSLWebSocketServer
from optparse import OptionParser

#import websocket
from random import randint
from threading import Thread
import sys
import time
import six
try:
    import Threading
except ImportError:  #TODO use Threading instead of _thread in python3
    import thread as thread
import time
import signal, ssl

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


class MyThread(Thread):
    def __init__(self, val):
        ''' Constructor. '''

        Thread.__init__(self)
        self.val = val

    def run(self):
        for i in range(1, self.val):
            print('Value %d in thread %s' % (i, self.getName()))

            # Sleep for random time between 1 ~ 3 second
            secondsToSleep = randint(1, 5)
            print('%s sleeping fo %d seconds...' % (self.getName(), secondsToSleep))
            time.sleep(secondsToSleep)



class SimpleEcho(WebSocket):

    def handleMessage(self):
        self.sendMessage(self.data)

    def handleConnected(self):
        pass

    def handleClose(self):
        self.server.close()  #  hmmm - maybe not a solution
        pass

class DOPsServerHandling(WebSocket):

    def handleMessage(self):
        self.sendMessage(self.data)

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
        #websocket.enableTrace(True)
        # host = "ws://127.0.0.1:9876/"
        #host = self.host
        #ws = websocket.WebSocketApp(host,
        #                            on_message=on_message,
        #                            on_error=on_error,
        #                            on_close=on_close)
        #ws.on_open = on_open
        #ws.run_forever()

        # self.server = SimpleWebSocketServer(self.host, self.port, SimpleEcho)
        # self.server = SimpleWebSocketServer(self.host, self.port, DOPsServerHandling)
        self.server.serveforMe()

    def stop(self):
        self.server.stopServeForMe()
        # self.server.close()
        # self.server.bContinueToServe = False
        
class mockDOPsServer(object):

    def __init__(self, host, port):

        # Declare objects of MyThread class
        # myThreadOb1 = MyThread(4)
        # myThreadOb1.setName('Thread 1')
        # Start running the threads!
        # myThreadOb1.start()
        # Wait for the thread to finish...
        # myThreadOb1.join()

        self.myThreadServer = MyThread_DOPsServer(host, port)
        self.myThreadServer.setName('Thread DOPs Server')
        self.myThreadServer.start()

        super(mockDOPsServer, self).__init__()

    def stopmockServer(self):
        self.myThreadServer.stop()
