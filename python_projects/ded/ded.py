#!/usr/bin/python
# -*- coding: utf-8 -*-

#/***************************************************************************
#    <u><font color="red">DED</font></u>.py -- Data Encoding/Decoding Program
#    (tab = 4 spaces)
#****************************************************************************
#    2/2/2016 Johnny Serup
#    Use, distribute, and modify this program freely.
#    Please send me your improved versions.
#**************************************************************/
import copy

from array import array

# Element types
DED_ELEMENT_TYPE_BOOL = 0
DED_ELEMENT_TYPE_CHAR = 1
DED_ELEMENT_TYPE_BYTE = 2
#DED_ELEMENT_TYPE_WCHAR =  3
#DED_ELEMENT_TYPE_SHORT =  4
DED_ELEMENT_TYPE_USHORT = 5
DED_ELEMENT_TYPE_LONG = 6
DED_ELEMENT_TYPE_ULONG = 7
DED_ELEMENT_TYPE_FLOAT = 8
#DED_ELEMENT_TYPE_DOUBLE = 9
DED_ELEMENT_TYPE_STRING = 10
#DED_ELEMENT_TYPE_WSTRING = 11
#DED_ELEMENT_TYPE_URI     = 12
DED_ELEMENT_TYPE_METHOD = 13
DED_ELEMENT_TYPE_STRUCT = 14
#DED_ELEMENT_TYPE_LIST   = 15
#DED_ELEMENT_TYPE_ARRAY  = 16
DED_ELEMENT_TYPE_ZERO = 17
DED_ELEMENT_TYPE_CLASS = 18
DED_ELEMENT_TYPE_STDSTRING = 19
DED_ELEMENT_TYPE_SVGFILE = 20
DED_ELEMENT_TYPE_VA_LIST = 21
#DED_ELEMENT_TYPE_FILE    = 22
DED_ELEMENT_TYPE_STDVECTOR = 23
DED_ELEMENT_TYPE_STRUCT_END = 24
#...
DED_ELEMENT_TYPE_UNKNOWN = 0xff

#
# ASN1 - limited version
# sequence of abstract syntax notation
# tag-byte,length-byte,data,tag-byte,length-byte,data,....
#
class CASN1:
    ASN1Data = 0		# will be allocated an contain the data being processed
    iLengthOfData = 0  	        # Length of ASN1 data, copied during construction.
    iTotalLengthOfData = 0      # Total length of ASN1 data.
    pNextASN1 = 0			    # Will point at first ASN1 structure, and when FetchNextASN1 is called it will move to (point at) next ASN1 location.
    pAppendPosition = 0	        # Pointer to end of current ASN1 data
    CurrentASN1Position = 0
    NextASN1Position = 0

    def __init__(self):
        ASN1Data = bytearray()		# will be allocated an contain the data being processed
        iLengthOfData = 0  	        # Length of ASN1 data, copied during construction.
        iTotalLengthOfData = 0      # Total length of ASN1 data.
        pNextASN1 = 0			    # Will point at first ASN1 structure, and when FetchNextASN1 is called it will move to (point at) next ASN1 location.
        pAppendPosition = 0	        # Pointer to end of current ASN1 data
        CurrentASN1Position = 0
        NextASN1Position = 0


    def CASN1p1(self, iAppendMaxLength):
        result = -1
        if iAppendMaxLength > 0:
            pNextASN1 = -1
            CurrentASN1Position = 0
            iLengthOfData = 0
            ASN1Data = bytearray()
            for i in range(iAppendMaxLength): ASN1Data[i] = 0
            if ASN1Data.length == iAppendMaxLength:
                iTotalLengthOfData = iAppendMaxLength
                pNextASN1 = 0 # First ASN1
                pAppendPosition = 0
                for i in range(iAppendMaxLength):
                    ASN1Data[i] = 0
                result = 1
        return result

    def CASN1p3(self, LengthOfData,data, iAppendMaxLength):
        result = -1
        pNextASN1 = -1
        CurrentASN1Position = 0
        iLength = iAppendMaxLength + LengthOfData
        if data == 0:
            return -1
        iLengthOfData = LengthOfData
        iTotalLengthOfData = iLength # max room for data
        if iLength != 0:
            ASN1Data = bytearray()
            for i in range(iLength): ASN1Data.append(0)
            if ASN1Data.length == iLength:
                for i in range(iLength):
                    ASN1Data[i] = 0
                pNextASN1 = 0  # First ASN1
                #for n in range(LengthOfData):
                #   ASN1Data[n] = data[n] # copy data into new allocated space
                ASN1Data = copy.deepcopy(data)
                pAppendPosition = 0
                result = 1
            else:
                result = -2
        return result

    def WhatIsReadSize(self):
        return self.iLengthOfData

    def WhatIsWriteSize(self):
        result=0
        result = self.iTotalLengthOfData - self.iLengthOfData
        if result<0:
            result=0
        return result

    def AppendASN1(self, LengthOfNewASN1Data, Tag, data):
        bresult=True
        if self.iTotalLengthOfData < (self.iLengthOfData + LengthOfNewASN1Data):
            bresult = False
        else:
            pAppendPosition = self.iLengthOfData
            self.ASN1Data[pAppendPosition + 0] = (LengthOfNewASN1Data       & 0x000000ff)
            self.ASN1Data[pAppendPosition + 1] = (LengthOfNewASN1Data >> 8  & 0x000000ff)
            self.ASN1Data[pAppendPosition + 2] = (LengthOfNewASN1Data >> 16 & 0x000000ff)
            self.ASN1Data[pAppendPosition + 3] = (LengthOfNewASN1Data >> 24 & 0x000000ff)
            self.ASN1Data[pAppendPosition + 4] = (Tag & 0x000000FF)  # unsigned char 8 bit  -- tag byte

            if data.length > 0:
                for i in range(LengthOfNewASN1Data):
                    self.ASN1Data[pAppendPosition + 4 + 1 + i] = data[i]
                self.iLengthOfData = self.iLengthOfData + 4 + 1 + LengthOfNewASN1Data # Add new ASN1 to length : Length+tag+SizeofData
            else:
                bresult = False
        return bresult

    def FetchNextASN1(self, param): # Returns true if ASN1 was found, and false if not.
        bresult = True
        if self.pNextASN1 >= 0:
                param.Length = param.Length | (self.ASN1Data[self.pNextASN1 + 0] & 0x000000ff)
                param.Length = param.Length | (self.ASN1Data[self.pNextASN1 + 1] & 0x000000ff) << 8
                param.Length = param.Length | (self.ASN1Data[self.pNextASN1 + 2] & 0x000000ff) << 16
                param.Length = param.Length | (self.ASN1Data[self.pNextASN1 + 3] & 0x000000ff) << 24

                self.pNextASN1 += 4 # sizeof(length) 32 bits
                param.Tag = self.ASN1Data[self.pNextASN1+1] & 0x000000FF # fetch byte tag

                if param.Length == 1:
                    param.data[0] = self.ASN1Data[self.pNextASN1]
                else:
                    if param.Length == 2:
                        param.data[0] = self.ASN1Data[self.pNextASN1]
                        param.data[1] = self.ASN1Data[self.pNextASN1 + 1]
                    else:
                        param.data = bytearray()
                        for n in range(param.Length): param.data.append(0)
                        for i in range(param.Length):
                            param.data[i] = self.ASN1Data[self.pNextASN1 + i]

                NextASN1Position = self.CurrentASN1Position + param.Length + 1 + 4
                if NextASN1Position > self.iTotalLengthOfData | NextASN1Position < 0:
                    pNextASN1 = 0
                    bResult = False; # ASN1 says it is longer than ASN1 allocated space ??? ASN1 has illegal size.
                else:
                    CurrentASN1Position = NextASN1Position
                    if CurrentASN1Position >= self.iTotalLengthOfData:
                        self.pNextASN1 = 0
                    else:
                        self.pNextASN1 += param.Length
        else:
                bResult = False

        return bResult


    def FetchTotalASN1(self, data):
        bresult = True
        if self.ASN1Data.length > 0:
            param.Length = self.iLengthOfData # Array can be larger than amount of valid data inside
        param.data = bytearray()
        for n in range(param.Length): param.data.append(0)
        for i in range(param.Length):
            param.data[i] = self.ASN1Data[i]
        else:
            bresult = False

        return bresult

class param:
    name = ""
    ElementType = 0
    value = 0
    length = 0

class data:
    Length = 0
    data = 0

class asn:
    Length = 0
    Tag = 0
    data = 0

class DEDEncoder(object):
    encoder = 0
    pdata = 0
    iLengthOfData = 0
    ptotaldata = 0
    iLengthOfTotalData = 0

    def __init__(self):
        self.encoder = 0

    def addelement(self, element):
        if element.ElementType == DED_ELEMENT_TYPE_STRUCT:
            # First element in structure
            asn1 = CASN1()
            result = asn1.CASN1p1(element.name.length() + 4 + 1)
            LengthOfAsn1 = len(element.name)
            asn1.AppendASN1(LengthOfAsn1, element.ElementType, element.name.getBytes())
            paramasn1 = data()
            asn1.FetchTotalASN1(paramasn1)
            iLengthOfTotalData = paramasn1.Length
            iLengthOfData = iLengthOfTotalData  # First element in structure
            pdata = bytearray()
            # for i in range(iLengthOfData): pdata.append(0)
            for i in range(paramasn1.Length):
                pdata.append(paramasn1.data[i])
            # pdata = copy.deepcopy(paramasn1.data)
            self.ptotaldata = pdata
        else:
            asn1 = CASN1()
            result = asn1.CASN1p3(self.iLengthOfTotalData, self.pdata, self.iLengthOfTotalData + len(element.name) +
                                  element.length + 1)
            # 1. asn  "name"
            LengthOfAsn1 = len(element.name)
            asn1.AppendASN1(LengthOfAsn1, element.ElementType, element.name.getBytes())
            # 2. asn "value"
            LengthOfAsn1 = element.length
            if LengthOfAsn1 > 0:
                asn1.AppendASN1(LengthOfAsn1, element.ElementType, element.value)
                paramasn1 = data()
                asn1.FetchTotalASN1(paramasn1)
                self.iLengthOfTotalData = paramasn1.Length
                if self.iLengthOfTotalData > 0:
                    pdata = bytearray()
                    for i in range(self.iLengthOfTotalData):
                        pdata.append(paramasn1.data[i])
                    self.ptotaldata = pdata
            if self.iLengthOfTotalData > 0:
                result = self.iLengthOfTotalData
        return result

    def encodestructstart(self, name):
        element = param()
        element.name = name
        element.ElementType = DED_ELEMENT_TYPE_STRUCT
        element.value = 0
        element.length = 0
        result = self.addelement(element)
        return result

    def DED_START_ENCODER(self):
        self.encoder = DEDEncoder(self)
        return self.encoder

    def PUT_STRUCT_START(self, name):
        return self.result