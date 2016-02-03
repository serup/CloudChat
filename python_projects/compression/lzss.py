#!/usr/bin/env python
# -*- coding: utf-8 -*-

#/**************************************************************
#    <u><font color="red">LZSS</font></u>.C -- A Data Compression Program
#    (tab = 4 spaces)
#***************************************************************
#    4/6/1989 Haruhiko Okumura
#    Use, distribute, and modify this program freely.
#    Please send me your improved versions.
#        PC-VAN        SCIENCE
#        NIFTY-Serve    PAF01022
#        CompuServe    74050,1022
#**************************************************************/

from array import array
try:
    import c_lzss
except:
    c_lzss = False


llen = len

N = 2048
F = 33
THRESHOLD = 1

NIL = N

mask1 = 0xe0
mask2 = 0x1f

text_buf = array('B', '\0' * (N + F - 1))

lson = [0] * (N + 1)
rson = [0] * (N + 257)
dad = [0] * (N + 1)

textsize = 0
codesize = 0
printcount = 0

match_position = 0
match_length = 0

def initTree():
    global N, F, THRESHOLD, NIL, maskl, mask2, text_buf, lson, rson, dad, textsize, codesize, printcount, match_length, match_position
    for i in xrange(N + 1, N + 257):
        rson[i] = NIL
    for i in xrange(0, N):
        dad[i] = NIL

def insertNode(r):
    global N, F, THRESHOLD, NIL, maskl, mask2, text_buf, lson, rson, dad, textsize, codesize, printcount, match_length, match_position
    cmp = 1
    p = N + 1 + text_buf[r]
    rson[r] = lson[r] = NIL
    match_length = 0
    while True:
        if cmp >= 0:
            if rson[p] != NIL:
                p = rson[p]
            else:
                rson[p] = r
                dad[r] = p
                return
        else:
            if lson[p] != NIL:
                p = lson[p]
            else:
                lson[p] = r
                dad[r] = p
                return
        i = 1
        while i < F:
            cmp = text_buf[r + i] - text_buf[p + i]
            if cmp != 0:
                break
            i += 1

        if i > match_length:
            match_position = p
            match_length = i
            if match_length >= F:
                break
    dad[r] = dad[p]
    lson[r] = lson[p]
    rson[r] = rson[p]
    dad[lson[p]] = r
    dad[rson[p]] = r
    if rson[dad[p]] == p:
        rson[dad[p]] = r
    else:
        lson[dad[p]] = r
    dad[p] = NIL

def deleteNode(p):
    global N, F, THRESHOLD, NIL, maskl, mask2, text_buf, lson, rson, dad, textsize, codesize, printcount, match_length, match_position
    q = 0
    if dad[p] == NIL:
        return
    if rson[p] == NIL:
        q = lson[p]
    elif lson[p] == NIL:
        q = rson[p]
    else:
        q = lson[p]
        if rson[q] != NIL:
            while True:
                q = rson[q]
                if rson[q] == NIL:
                    break
            rson[dad[q]] = lson[q]
            dad[lson[q]] = dad[q]
            lson[q] = lson[p]
            dad[lson[p]] = q
        rson[q] = rson[p]
        dad[rson[p]] = q
    dad[q] = dad[p]
    if rson[dad[p]] == p:
        rson[dad[p]] = q
    else:
        lson[dad[p]] = q
    dad[p] = NIL

def encode(inputBuf, offset, length):
    #    print length
    if c_lzss:
        inputBufStr = inputBuf.tostring()
        #        print llen(c_lzss.encode(inputBufStr, offset, length))
        #        array('B', c_lzss.encode(inputBufStr, offset, length)).tofile(open('d:/temp/c.lzss','wb'))
        #        raise self
        return array('B', c_lzss.encode(inputBufStr, offset, length))


    global N, F, THRESHOLD, NIL, maskl, mask2, text_buf, lson, rson, dad, textsize, codesize, printcount, match_length, match_position
    #codesize=0
    #textsize=0
    i, c, len, r, s, last_match_length, code_buf_ptr, mask = 0, 0, 0, 0, 0, 0, 0, 0
    code_buf = array('B', '\0' * (F - 1))
    initTree()
    code_buf[0] = 0
    code_buf_ptr = mask = 1
    s = 0
    r = N - F
    outputBuf = array('B')
    p = offset
    for i in xrange(s, r):
        text_buf[i] = 0
    i += 1
    len = 0
    while True:
        if len >= F:
            break
        if p >= length + offset:
            break
        c = inputBuf[p]
        p += 1

        text_buf[r + len] = c

        len += 1
    textsize = len
    if textsize == 0:
        return
    for i in xrange(1, F + 1):
        insertNode(r - i)
    i += 1
    insertNode(r)
    while True:
        if match_length > len:
            match_length = len
        if match_length <= THRESHOLD:
            match_length = 1
            code_buf[0] |= mask
            code_buf[code_buf_ptr] = text_buf[r]
            code_buf_ptr += 1
        else:
            code_buf[code_buf_ptr] = match_position & 0xff
            code_buf_ptr += 1
            code_buf[code_buf_ptr] = (((match_position >> 3) & mask1)
                                      | (match_length - (THRESHOLD + 1))) & 0xff
            code_buf_ptr += 1
        mask <<= 1
        mask &= 0xff
        if mask == 0:
            for i in xrange(0, code_buf_ptr):
                outputBuf.append(code_buf[i])
            i += 1
            codesize += code_buf_ptr
            code_buf[0] = 0
            code_buf_ptr = mask = 1
        last_match_length = match_length
        i = 0
        while True:
            if i >= last_match_length:
                break
            if p >= length + offset:
                break
            c = inputBuf[p]
            p += 1

            deleteNode(s)
            text_buf[s] = c
            if s < F - 1:
                text_buf[s + N] = c
            s = (s + 1) & (N - 1)
            r = (r + 1) & (N - 1)
            insertNode(r)

            i += 1
        textsize += i
        if textsize > printcount:
            #print textsize
            printcount += 1024
        while i < last_match_length:
            i += 1
            deleteNode(s)
            s = (s + 1) & (N - 1)
            r = (r + 1) & (N - 1)
            len -= 1
            if len > 0:
                insertNode(r)
        i += 1
        if len <= 0:
            break
    if code_buf_ptr > 1:
        for i in xrange(0, code_buf_ptr):
            outputBuf.append(code_buf[i])
        i += 1
        codesize += code_buf_ptr
        #print 'In: ' + str(textsize) + ' bytes\n'
        #print 'Out: ' + str(codesize) + ' bytes\n'
    #    print llen(outputBuf)
    #    print llen(outputBuf)
    #    outputBuf.tofile(open('d:/temp/a.lzss','wb'))
    #    raise self
    return outputBuf

def decode(inputBuf, offset, length):
    #    print length
    if c_lzss:
        inputBufStr = inputBuf.tostring()
        #        print llen(c_lzss.decode(inputBufStr, offset, length))
        #        array('B', c_lzss.decode(inputBufStr, offset, length)).tofile(open('d:/temp/c.lzss','wb'))
        #        raise self
        #        array('B', c_lzss.encode(inputBufStr, offset, length)).tofile(open('d:/temp/c.lzss','wb'))
        #        raise self
        return array('B', c_lzss.decode(inputBufStr, offset, length))
    global N, F, THRESHOLD, NIL, maskl, mask2, text_buf, lson, rson, dad, textsize, codesize, printcount, match_length, match_position
    i = 0
    j = 0
    k = 0
    c = 0
    p = offset
    flags = 0

    text_buf = array('B', '\0' * (N + F))
    outputBuf = array('B')
    r = N - F

    while(1):
        flags = flags >> 1
        if flags & 256 == 0:
            if p >= length + offset:
                break
            c = inputBuf[p]
            p += 1
            flags = c | 0xff00
        if flags & 1:
            if p >= length + offset:
                break
            c = inputBuf[p]
            p += 1
            outputBuf.append(c)
            text_buf[r] = c
            r = r + 1
            r &= N - 1
        else:
            if p >= length + offset:
                break
            i = inputBuf[p]
            p += 1
            if p >= length + offset:
                break
            j = inputBuf[p]
            p += 1
            i |= ((j & mask1) << 3)
            j = (j & mask2) + THRESHOLD
            for k in xrange(0, j + 1):
                c = text_buf[(i + k) & (N - 1)]
                outputBuf.append(c)
                text_buf[r] = c
                r += 1
                r &= N - 1
                #    print llen(outputBuf)
                #    outputBuf.tofile(open('d:/temp/a.lzss','wb'))
                #    raise self
    return outputBuf


#import codecs
#s=encode(array('B','ab\0cde'),1,4)
#print decode(s,0,len(s)).tostring()
#s=encode(array('B',codecs.encode(u'ä½ å¥½å•Šä¸–ç•Œ','gbk')),2,8)
#print codecs.decode(decode(s,0,len(s)).tostring(),'gbk')


