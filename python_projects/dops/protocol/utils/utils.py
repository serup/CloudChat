#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from array import array


class CUtils:

    def __init__(self):
        self

    def showdatablock(self, chunkdata, length):
        usecolor = False
        flipflop = False
        for n in range(0, length):
            if (n + 1) % 16 == 0:
                if usecolor:
                    sys.stdout.write("%02X%s" % (chunkdata[n]," ") )
                else:
                    sys.stdout.write("%02X%s" % (chunkdata[n]," ") )
                print
            else:
                if usecolor:
                    sys.stdout.write("%02X%s" % (chunkdata[n]," ") )
                else:
                    sys.stdout.write("%02X%s" % (chunkdata[n]," ") )

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
