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

# This class provides the functionality of a switch / case statement. You only need to look at
# this if you want to know how this works. It only needs to be defined
# once, no need to muck around with its internals.
# http://code.activestate.com/recipes/410692/?_ga=1.176088226.1080543275.1455291568
class switch(object):
    def __init__(self, value):
        self.value = value
        self.fall = False

    def __iter__(self):
        """Return the match method once, then stop"""
        yield self.match
        raise StopIteration

    def match(self, *args):
        """Indicate whether or not to enter a case suite"""
        if self.fall or not args:
            return True
        elif self.value in args: # changed for v1.5, see below
            self.fall = True
            return True
        else:
            return False


class DOPsServerHandling(WebSocket):

    def handleMessage(self):
        DED = ded.DEDEncoder()
        DED.PUT_DATA_IN_DECODER(self.data, len(self.data))
        if DED.GET_STRUCT_START("WSRequest") :
            strMethod = DED.GET_METHOD("name")
            for case in switch(strMethod):
                if case('PythonConnect'):
                    uTrans_id          = DED.GET_USHORT("trans_id")
                    strProtocolTypeID  = DED.GET_STDSTRING("protocolTypeID")
                    strFunctionName    = DED.GET_STDSTRING("functionName")
                    strUsername        = DED.GET_STDSTRING("username")
                    strPassword        = DED.GET_STDSTRING("password")
                    # handle PythonConnect WSRequest
                    if DED.GET_STRUCT_END("WSRequest"):
                        bDecoded = True
                        # expected values
                        trans_id = 69
                        uniqueId = "985998707DF048B2A796B44C89345494"
                        username = "johndoe@email.com"
                        password = "12345"

                        if uTrans_id != trans_id: bDecoded = False
                        if strFunctionName != uniqueId: bDecoded = False
                        if strProtocolTypeID != "DED1.00.00": bDecoded = False
                        if strUsername != username: bDecoded = False
                        if strPassword != password: bDecoded = False

                        if bDecoded:
                           strStatus="ACCEPTED"
                        else:
                           strStatus="NOT ACCEPTED USER"

                        # 3. create response packet
                        DED2 = ded.DEDEncoder()
                        DED2.PUT_STRUCT_START("WSResponse")
                        DED2.PUT_METHOD("name", strMethod)
                        DED2.PUT_USHORT("trans_id", uTrans_id)
                        DED2.PUT_STDSTRING("protocolTypeID", "DED1.00.00")
                        DED2.PUT_STDSTRING("functionName", strFunctionName)
                        DED2.PUT_STDSTRING("status", strStatus)
                        DED2.PUT_STRUCT_END("WSResponse")
                        DEDobj = DED2.GET_ENCODED_DATA()

                        # 4. send response
                        self.sendMessage(DEDobj.pCompressedData)  # echo back the message
                    break
                if case('JavaConnect'):
                    print "WSRequest - Error this is NOT a valid response"
                    break
                if case():  # default, could also just omit condition or 'if True'
                    print "Warning - Unknown WSRequest"
                    self.sendMessage(self.data)  # echo back the incoming message
                    # No need to break here, it'll stop anyway
        else:
            if DED.GET_STRUCT_START("DFDRequest"):
                print "DFDRequest - received - now parse"
                strMethod = DED.GET_METHOD("Method")
                for case in switch(strMethod):
                    if case('CreateProfile'):
                        uTrans_id = DED.GET_USHORT("TransID")
                        strProtocolTypeID = DED.GET_STDSTRING("protocolTypeID")
                        for destination_case in switch(DED.GET_STDSTRING("dest")):
                            if destination_case('DFD_1.1'):
                                strClientSrc = DED.GET_STDSTRING("src")
                                if (DED.GET_STDSTRING("STARTrequest").__eq__("EmployeeRequest") and
                                    DED.GET_STDSTRING("STARTrecord").__eq__("record")):
                                    print "- EmployeeRequest - received - now parse"
                                    if (DED.GET_STDSTRING("profileID").__sizeof__() > 0 and
                                        DED.GET_STDSTRING("profileName").__sizeof__() > 0 and
                                        DED.GET_STDSTRING("protocolTypeID").__sizeof__() > 0 and
                                        DED.GET_STDSTRING("sizeofProfileData").__sizeof__() > 0 and
                                        DED.GET_STDSTRING("profile_chunk_id").__sizeof__() > 0 and
                                        DED.GET_STDSTRING("AccountStatus").__sizeof__() > 0 and
                                        DED.GET_STDSTRING("ExpireDate").__sizeof__() > 0 and
                                        DED.GET_STDSTRING("ProfileStatus").__sizeof__() > 0):
                                        print "-- Employee record received - now validate TOAST "
                                        if DED.GET_STDSTRING("STARTtoast").__sizeof__() > 0:
                                            # TOAST area found, now iterate thru all elements
                                            print "--- TOAST area found, now iterate thru all elements"
                                            bFoundElement = True
                                            bDecoded = False
                                            while bFoundElement:
                                                elementvalue = DED.GET_ELEMENT("profile")
                                                if elementvalue != -1:
                                                    bFoundElement = True
                                                    print "--- TOAST element : " + elementvalue.strElementID
                                                    if ((DED.GET_STDSTRING("elements-ignore").isEmpty()) and
                                                         DED.GET_STDSTRING("ENDrecord").__eq__("record") and
                                                         DED.GET_STDSTRING("ENDrequest").__eq__("EmployeeRequest") and
                                                         DED.GET_STDSTRING("DFDRequest").isEmpty()):
                                                         print "-- END Employee record"
                                                         print "- END EmployeeRequest"
                                                         print "END DFDRequest"
                                                         print "DFDRequest parsed correct"
                                                         bDecoded = True
                                                else:
                                                    bFoundElement = False

                                                # 2. determine what to respond
                                                if bDecoded:
                                                    strStatus = "Profile Saved in database";
                                                else:
                                                    strStatus = "Error in creating profile";

                                                # 3. create response packet
                                                DED2 = ded.DEDEncoder()
                                                DED2.PUT_STRUCT_START("DFDResponse")
                                                DED2.PUT_METHOD("Method", "CreateProfile")
                                                DED2.PUT_USHORT("TransID", uTrans_id)
                                                DED2.PUT_STDSTRING("protocolTypeID", "DED1.00.00")
                                                DED2.PUT_STDSTRING("dest", strClientSrc)
                                                DED2.PUT_STDSTRING("src", "DFD_1.1")
                                                DED2.PUT_STDSTRING("status", strStatus)
                                                DED2.PUT_STRUCT_END("DFDResponse")
                                                DEDobj = DED2.GET_ENCODED_DATA()

                                                # 4. send response
                                                self.sendMessage(DEDobj.pCompressedData)  # echo back the message
                                        else:
                                            print "No TOAST area found in request, meaning NO elements added to profile info"
                                break
                            if destination_case():  # default, could also just omit condition or 'if True'
                                print "Warning - unknown DFDRequest - accepting basic parsing - header of packet was correct "
                                # No need to break here, it'll stop anyway
                        break
                    if case():  # default, could also just omit condition or 'if True'
                        print "Warning - Unknown DFDRequest - this version of mockDOPsServer does NOT understand"
                        self.sendMessage(self.data)  # echo back the incoming message
                        # No need to break here, it'll stop anyway

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

