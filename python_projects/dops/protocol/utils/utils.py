#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from array import array
import zlib, struct
import StringIO
import struct



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

    @staticmethod
    def escape_html_tags(chunkdata):
        chunkdata = chunkdata.replace(">","&gt;")
        chunkdata = chunkdata.replace("<","&lt;")
        chunkdata = chunkdata.replace("\"","&quot;")
        return chunkdata

    def showdatablock(self, chunkdata):
        length = len(chunkdata)
        position_on_line = 0
        offset = 0
        for n in range(0, length):
            if (n + 1) % 16 == 0:
                sys.stdout.write("%02X%s" % (chunkdata[n], " "))
                print " ",
                for c in range(0, 16):
                    if self.isalpha(str(chr(chunkdata[c + offset]))):
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

    def hexdump_to_string_escaped_html(self, chunkdata):
        length = len(chunkdata)
        position_on_line = 0
        offset = 0
        output = ""
        for n in range(0, length):
            if (n + 1) % 16 == 0:
                output += "{:02X}".format(chunkdata[n]) + " "
                for c in range(0, 16):
                    if self.isalpha(chr(chunkdata[c + offset])):
                        tmp = chr(chunkdata[c + offset])
                        tmp = self.escape_html_tags(tmp)
                        output += tmp
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
                        tmp = chr(chunkdata[pos])
                        tmp = self.escape_html_tags(tmp)
                        output += tmp
                    else:
                        output += "."
        return output

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
                        tmp = chr(chunkdata[c + offset])
                        tmp = self.escape_html_tags(tmp)
                        output += tmp
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
                        tmp = chr(chunkdata[pos])
                        tmp = self.escape_html_tags(tmp)
                        output += tmp
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

    def hexdump_file_to_html_string(self, filepathname):
        output = "<html><pre>"
        flipflop = False
        with open(filepathname, 'rb') as infile:
            while True:
                chunk = infile.read(16)
                if len(chunk) == 0:
                    break

                if flipflop:
                    output += "<font size=\"2\" face=\"courier new\" color=\"green\">"
                    flipflop = False
                else:
                    output += "<font size=\"2\" face=\"courier new\" color=\"orange\">"
                    flipflop = True

                s = array('B', chunk)
                output += self.hexdump_to_string_escaped_html(s)

        # remove end of file marker
        # return output[:-1]
        output += "</pre></html>"
        return output

    def write_png(self, buf, width, height):
        """ buf: must be bytes or a bytearray in Python3.x,
            a regular string in Python2.x.
        """

        # reverse the vertical line order and add null bytes at the start
        width_byte_4 = width * 4
        raw_data = b''.join(b'\x00' + buf[span:span + width_byte_4]
                        for span in range((height - 1) * width_byte_4, -1, - width_byte_4))

        def png_pack(png_tag, data):
            chunk_head = png_tag + data
            return (struct.pack("!I", len(data)) +
                chunk_head +
                struct.pack("!I", 0xFFFFFFFF & zlib.crc32(chunk_head)))

        return b''.join([
        b'\x89PNG\r\n\x1a\n',
        png_pack(b'IHDR', struct.pack("!2I5B", width, height, 8, 6, 0, 0, 0)),
        png_pack(b'IDAT', zlib.compress(raw_data, 9)),
        png_pack(b'IEND', b'')])

# below is based on get_image_info.py from github Gist from Corey Goldberg, 2013
# start of snippet
    """validate and analyze dimensions of image files.  Supports: PNG, JPG, GIF."""
    """example usage:
            with open('foo.png', 'rb') as f:
                data = f.read()
            w, h = CUtils.get_image_dimensions(data)
            print w, h
    """

    def is_gif(self, data):
        return (data[:6] in ('GIF87a', 'GIF89a'))

    def is_png(self, data):
        return ((data[:8] == '\211PNG\r\n\032\n') and (data[12:16] == 'IHDR'))

    def is_old_png(self, data):
        return ((data[:8] == '\211PNG\r\n\032\n') and (data[12:16] != 'IHDR'))

    def is_jpeg(self, data):
        return (data[:2] == '\377\330')

    def get_image_dimensions(self, data):
        if len(data) < 16:
            raise Exception('not valid image data')

        elif self.is_gif(data):
            w, h = struct.unpack('<HH', data[6:10])
            width = int(w)
            height = int(h)

        elif self.is_png(data):
            w, h = struct.unpack('>LL', data[16:24])
            width = int(w)
            height = int(h)

        elif self.is_old_png(data):
            w, h = struct.unpack('>LL', data[8:16])
            width = int(w)
            height = int(h)

        elif self.is_jpeg(data):
            jpeg = StringIO.StringIO(data)
            jpeg.read(2)
            b = jpeg.read(1)
            try:
                while (b and ord(b) != 0xDA):
                    while (ord(b) != 0xFF): b = jpeg.read(1)
                    while (ord(b) == 0xFF): b = jpeg.read(1)
                    if (ord(b) >= 0xC0 and ord(b) <= 0xC3):
                        jpeg.read(3)
                        h, w = struct.unpack('>HH', jpeg.read(4))
                        break
                    else:
                        jpeg.read(int(struct.unpack('>H', jpeg.read(2))[0])-2)
                    b = jpeg.read(1)
                width = int(w)
                height = int(h)
            except struct.error:
                pass
            except ValueError:
                pass

        else:
            raise Exception('not valid image data')

        return width, height
# end of snippet

    @staticmethod
    def is_binay_file(filepathname):
        textchars = bytearray([7,8,9,10,12,13,27]) + bytearray(range(0x20, 0x7f)) + bytearray(range(0x80, 0x100))
        is_binary_string = lambda bytes: bool(bytes.translate(None, textchars))

        if is_binary_string(open(filepathname, 'rb').read(1024)):
            return True
        else:
            return False



