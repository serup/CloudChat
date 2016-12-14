#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from array import array


class CUtils:

    def __init__(self):
        self

    def isalpha(self, byte):
        if ord(byte) < 128 and ord(byte) > 32:
            bResult = True
        else:
            bResult = False

        return bResult

    def showdatablock(self, chunkdata, length):
        usecolor = False
        flipflop = False
        offset=0
        for n in range(0, length):
            if (n + 1) % 16 == 0:
                if usecolor:
                    sys.stdout.write("%02X%s" % (chunkdata[n]," ") )
                else:
                    sys.stdout.write("%02X%s" % (chunkdata[n]," ") )
                print " ",
                for c in range(0, 16):
                    if self.isalpha(chr(chunkdata[c+offset])):
                        sys.stdout.write("%s" % chr(chunkdata[c+offset]))
                    else:
                        sys.stdout.write(".")
                offset=offset+16
                print
            else:
                if usecolor:
                    sys.stdout.write("%02X%s" % (chunkdata[n]," ") )
                else:
                    sys.stdout.write("%02X%s" % (chunkdata[n]," ") )
                if (n + 1) % 16 == 0:
                    print " "

            if usecolor:
                if ( n + 1 ) % 16 == 0:
                    if flipflop == True:
                        flipflop=False
                    else:
                        flipflop=True
                    if flipflop:
                        print("%s" % "\e[0m")
                    else:
                        print("%02X%s" % chunkdata[n] )
        return
