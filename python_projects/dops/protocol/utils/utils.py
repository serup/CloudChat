#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from array import array


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

    def hexdump_to_string(self, chunkdata):
        length = len(chunkdata)
        position_on_line = 0
        offset = 0
        output = ""
        for n in range(0, length):
            if (n + 1) % 16 == 0:
                output += "{:02X}".format(chunkdata[n]) + " "
                for c in range(0, 16):
                    if self.isalpha(chr(chunkdata[c + offset])):
                        output += chr(chunkdata[c + offset])
                    else:
                        output += "."
                offset += 16
                position_on_line = 16
                output += "\n"
            else:
                if position_on_line == 16:
                    position_on_line = 0
                position_on_line += 1
                output += "{:02X}".format(chunkdata[n]) + " "
                if (n + 1) % 16 == 0:
                    output += " "

        if position_on_line < 16:
            for l in range(0, 16 - position_on_line):
                output += "   "
            for c in range(0, 16):
                pos = c + length - position_on_line
                if pos < length:
                    if self.isalpha(chr(chunkdata[pos])):
                        output += chr(chunkdata[pos])
                    else:
                        output += "."
        return output

    def hexdump_to_html_string(self, chunkdata):
        length = len(chunkdata)
        position_on_line = 0
        offset = 0
        flipflop=False;
        output = "<html><pre><font size=\"2\" face=\"courier new\" color=\"green\">"
        for n in range(0, length):
            if (n + 1) % 16 == 0:
                output += "{:02X}".format(chunkdata[n]) + " "
                for c in range(0, 16):
                    if self.isalpha(chr(chunkdata[c + offset])):
                        output += chr(chunkdata[c + offset])
                    else:
                        output += "."
                offset += 16
                position_on_line = 16
                if flipflop:
                    output += "\n<font size=\"2\" face=\"courier new\" color=\"green\">"
                    flipflop = False
                else:
                    output += "\n<font size=\"2\" face=\"courier new\" color=\"orange\">"
                    flipflop = True
            else:
                if position_on_line == 16:
                    position_on_line = 0
                position_on_line += 1
                output += "{:02X}".format(chunkdata[n]) + " "
                if (n + 1) % 16 == 0:
                    output += " "

        if position_on_line < 16:
            for l in range(0, 16 - position_on_line):
                output += "   "
            for c in range(0, 16):
                pos = c + length - position_on_line
                if pos < length:
                    if self.isalpha(chr(chunkdata[pos])):
                        output += chr(chunkdata[pos])
                    else:
                        output += "."
        output += "</pre></html>"
        return output

    def hexdump_file_to_string(self, filepathname):
        output = ""
        with open(filepathname, 'rb') as infile:
            while True:
                chunk = infile.read(16)
                if len(chunk) == 0:
                    break

                s = array('B', chunk)
                output += self.hexdump_to_string(s)

        # remove end of file marker
        # return output[:-1]
        return output



