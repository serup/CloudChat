#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys


class CUtils:
    def __init__(self):
        # noinspection PyStatementEffect
        self

    @staticmethod
    def isalpha(byte):
        if 128 > ord(byte) > 32:
            alpha = True
        else:
            alpha = False

        return alpha

    def showdatablock(self, chunkdata):
        length = len(chunkdata)
        position_on_line = 0
        offset = 0
        for n in range(0, length):
            if (n + 1) % 16 == 0:
                sys.stdout.write("%02X%s" % (chunkdata[n], " "))
                print " ",
                for c in range(0, 16):
                    if self.isalpha(chr(chunkdata[c + offset])):
                        sys.stdout.write("%s" % chr(chunkdata[c + offset]))
                    else:
                        sys.stdout.write(".")
                offset += 16
                position_on_line = 16
                print
            else:
                if position_on_line == 16:
                    position_on_line = 0
                position_on_line += 1
                sys.stdout.write("%02X%s" % (chunkdata[n], " "))
                if (n + 1) % 16 == 0:
                    print " "

        if position_on_line < 16:
            for l in range(0, 16 - position_on_line):
                sys.stdout.write("   ")
            sys.stdout.write(" ")
            for c in range(0, 16):
                pos = c + length - position_on_line
                if pos < length:
                    if self.isalpha(chr(chunkdata[pos])):
                        sys.stdout.write("%s" % chr(chunkdata[pos]))
                    else:
                        sys.stdout.write(".")
        return
