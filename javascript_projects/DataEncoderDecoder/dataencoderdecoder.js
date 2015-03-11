// Converted to JavaScript, by johnny.serup@scanva.com
//
/**************************************************************
 DataEncoderDecoder.js -- A Data encoder and decoder program 
 ***************************************************************
 15/11/2013 Johnny Serup
 Use, distribute, and modify this program ONLY,
 if written agreement from SCANVA has been received.
 
 **************************************************************/
//
/*  Example: Creating an event message to be send to a server asking it to use method MusicPlayer
 passing on a transaction id and action Start/Stop
 All data is in the pCompressedData !!! it is this memory that should be send.
 
 // Client code
 // Create DataEncoderDecoder response
 // Encode
 DED_START_ENCODER(encoder_ptr);
 DED_PUT_STRUCT_START( encoder_ptr, "event" );
 DED_PUT_METHOD	( encoder_ptr, "name",  "MusicPlayer" );
 DED_PUT_USHORT	( encoder_ptr, "trans_id",	trans_id);
 DED_PUT_BOOL	( encoder_ptr, "startstop", action );
 DED_PUT_STRUCT_END( encoder_ptr, "event" );
 DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);
 
 // Data to be sent is in pCompressedData
 
 
 // Server code
 // retrieve data ...
 //...
 
 std::string strName,strValue;
 unsigned short iValue;
 bool bValue;
 
 DED_PUT_DATA_IN_DECODER(decoder_ptr,pCompressedData,sizeofCompressedData);
 
 // decode data ...
 if( DED_GET_STRUCT_START( decoder_ptr, "event" ) &&
 DED_GET_METHOD	( decoder_ptr, "name", strValue ) &&
 DED_GET_USHORT	( decoder_ptr, "trans_id", iValue) &&
 DED_GET_BOOL	( decoder_ptr, "startstop", bValue ) &&
 DED_GET_STRUCT_END( decoder_ptr, "event" ))
 {
 TRACE0(_T("FAIL!!!\n"));
 }
 else
 {
 TRACE0(_T("SUCCESS!!!\n"));
 }
 */



var doc = this["document"];

if (!this["output"]) {
    var output = function(string) {
        return doc ? doc.write(string + '<br/>') : console.log(string);
    };
}

function Init() {
    output("\nHello World\n");
    return 1;
}

function pack(bytes) {
    var chars = [];
    for (var i = 0, n = bytes.length; i < n; ) {
        chars.push(((bytes[i++] & 0xff) << 8) | (bytes[i++] & 0xff));
    }
    return String.fromCharCode.apply(null, chars);
}

function unpack(str) {
    var bytes = [];
    for (var i = 0, n = str.length; i < n; i++) {
        var char = str.charCodeAt(i);
        bytes.push(char >>> 8, char & 0xFF);
    }
    return bytes;
}

//////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////
/*  Types of elements:
 
 DED_ELEMENT_TYPE_ZERO	 - end of struct
 DED_ELEMENT_TYPE_BOOL    - bool8
 DED_ELEMENT_TYPE_CHAR    - char
 DED_ELEMENT_TYPE___byte    - __byte
 DED_ELEMENT_TYPE_WCHAR   - wide char
 DED_ELEMENT_TYPE_SHORT   - int16
 DED_ELEMENT_TYPE_USHORT  - uint16
 DED_ELEMENT_TYPE_LONG    - int32 (long)
 DED_ELEMENT_TYPE_ULONG   - uint32 (DWORD)
 DED_ELEMENT_TYPE_FLOAT   - float
 DED_ELEMENT_TYPE_DOUBLE  - double
 DED_ELEMENT_TYPE_STRING  - char pointer
 DED_ELEMENT_TYPE_WSTRING - wide string
 DED_ELEMENT_TYPE_URI     - char pointer
 DED_ELEMENT_TYPE_METHOD  - char pointer
 DED_ELEMENT_TYPE_STRUCT  - structure
 DED_ELEMENT_TYPE_LIST    - list
 DED_ELEMENT_TYPE_ARRAY   - array
 */
var DED_ELEMENT_TYPE_ZERO = 17
var DED_ELEMENT_TYPE_BOOL = 0
var DED_ELEMENT_TYPE_CHAR = 1
var DED_ELEMENT_TYPE_BYTE = 2
//var DED_ELEMENT_TYPE_WCHAR =  3
//var DED_ELEMENT_TYPE_SHORT =  4
var DED_ELEMENT_TYPE_USHORT = 5
var DED_ELEMENT_TYPE_LONG = 6
var DED_ELEMENT_TYPE_ULONG = 7
var DED_ELEMENT_TYPE_FLOAT = 8
//var DED_ELEMENT_TYPE_DOUBLE = 9
var DED_ELEMENT_TYPE_STRING = 10
//var DED_ELEMENT_TYPE_WSTRING = 11
//var DED_ELEMENT_TYPE_URI     = 12
var DED_ELEMENT_TYPE_METHOD = 13
var DED_ELEMENT_TYPE_STRUCT = 14
//var DED_ELEMENT_TYPE_LIST   = 15
//var DED_ELEMENT_TYPE_ARRAY  = 16
var DED_ELEMENT_TYPE_COBJECT = 17
var DED_ELEMENT_TYPE_CLASS = 18
var DED_ELEMENT_TYPE_STDSTRING = 19
var DED_ELEMENT_TYPE_SVGFILE = 20
var DED_ELEMENT_TYPE_VA_LIST = 21
//var DED_ELEMENT_TYPE_FILE    = 22
var DED_ELEMENT_TYPE_STDVECTOR = 23
//
var DED_ELEMENT_TYPE_STRUCT_END = 24
var DED_ELEMENT_TYPE_UNKNOWN = 0xff


//////////////////////////////////////////////
// class CASN1
//////////////////////////////////////////////
function CASN1()
{
// constructor
    var m_pASN1Data;  // will be allocated an contain the data being processed
    var m_iLengthOfData;		// Length of ASN1 data, copied during construction.
    var m_iTotalLengthOfData;	// Total length of ASN1 data.
    var m_pNextASN1;		// Will point at first ASN1 structure, and when FetchNextASN1 is called it will move to (point at) next ASN1 location.
    // The __byte (unsigned char) that it points at will be the length __byte of the ASN1 data.
    var m_pAppendPosition;	// Pointer to end of current ASN1 data
    var CurrentASN1Position, NextASN1Position;

}

CASN1.prototype.CASN1p1 = function(iAppendMaxLength)
{

    try {
        m_pNextASN1 = -1;
        CurrentASN1Position = 0;
        var iLength = iAppendMaxLength;
        m_iLengthOfData = 0;
        m_iTotalLengthOfData = 0;
        if (iLength != 0)
        {
            m_pASN1Data = new Array(iLength);
            if (m_pASN1Data != 0) {
                m_iTotalLengthOfData = iLength;
                m_pNextASN1 = 0; // First ASN1
                m_pAppendPosition = 0;
                for (i = 0; i < iLength; i++)
                    m_pASN1Data[i] = 0;

            }
            else
                return -3;
        }
        else
            return -2;

    }
    catch (error)
    {
        return -1;
    }

    return 1;

}

CASN1.prototype.CASN1p3 = function(LengthOfData, m_pdata, iAppendMaxLength)
{

    try {
        m_pNextASN1 = -1;
        CurrentASN1Position = 0;
        var iLength = iAppendMaxLength + LengthOfData;
        m_iLengthOfData = LengthOfData;
        m_iTotalLengthOfData = iLength; // max room for data
        if (iLength != 0)
        {
            m_pASN1Data = new Array(iLength);
            if (m_pASN1Data != 0)
            {
                for (i = 0; i < iLength; i++)
                    m_pASN1Data[i] = 0;
                m_pNextASN1 = 0; // First ASN1
                for (i = 0; i < LengthOfData; i++)
                    m_pASN1Data[i] = m_pdata[i]; // copy data into new allocated space
                m_pAppendPosition = 0; // make sure next asn appended to this is at the end, this will be calculated based on current content
            }
            else
                return -3;
        }
        else
            return -2;

    }
    catch (error)
    {
        return -1;
    }

    return 1;
}

CASN1.prototype.WhatIsReadSize = function()
{
    if (m_iLengthOfData > 0)
        return m_iLengthOfData;
    else
        return 0;
}

CASN1.prototype.WhatIsWriteSize = function()
{
    var iResult = 0;
    try
    {
        iResult = m_iTotalLengthOfData - m_iLengthOfData;
        if (iResult < 0)
            iResult = 0;
    }
    catch (error)
    {
        iResult = 0;
    }

    return iResult;
}


CASN1.prototype.AppendASN1 = function(LengthOfNewASN1Data, Tag, data)    // Append an ASN1 structure to a data area, fx. the data area of a datapacket.
{
    var bResult = true;

    if (m_iTotalLengthOfData < (m_iLengthOfData + LengthOfNewASN1Data)) {
        bResult = false; // NO ROOM for new ASN1
    }
    else {

        try {
            m_pAppendPosition = m_iLengthOfData;
            //var uint32 = new Uint32Array(2);
            //uint32[0] = LengthOfNewASN1Data;
            //m_pASN1Data[m_pAppendPosition] = LengthOfNewASN1Data;
            
            m_pASN1Data[m_pAppendPosition + 0] = (LengthOfNewASN1Data     & 0x000000ff);
            m_pASN1Data[m_pAppendPosition + 1] = (LengthOfNewASN1Data>>8  & 0x000000ff);
            m_pASN1Data[m_pAppendPosition + 2] = (LengthOfNewASN1Data>>16 & 0x000000ff);
            m_pASN1Data[m_pAppendPosition + 3] = (LengthOfNewASN1Data>>24 & 0x000000ff);
            
            m_pASN1Data[m_pAppendPosition + 4] = Tag & 0x000000FF; // unsigned char 8 bit  -- tag byte

            if (data.length > 0) {
                for (i = 0; i < LengthOfNewASN1Data; i++){
                    if(data[i] === 'string')
                        m_pASN1Data[m_pAppendPosition + 4 + 1 + i] = data[i].charCodeAt();
                    else
                        m_pASN1Data[m_pAppendPosition + 4 + 1 + i] = data[i];
                }
            }
            else
                m_pASN1Data[m_pAppendPosition + 4 + 1] = data;

            m_iLengthOfData = m_iLengthOfData + 4 + 1 + LengthOfNewASN1Data; // Add new ASN1 to length : Length+tag+SizeofData
        }
        catch (error)
        {
            bResult = false;
            m_iLengthOfData = m_iTotalLengthOfData;
        }
    }

    return bResult;
}

/*
 var param = {
 length:0,
 tag:0,
 pdata:0
 };
 */
CASN1.prototype.FetchNextASN1 = function(param) // Returns true if ASN1 was found, and false if not.
{
    var bResult = true;
    try {
        if (m_pNextASN1 >= 0)
        {
            //param.Length = m_pASN1Data[m_pNextASN1];
            //var z = new Uint32Array(m_pASN1Data[m_pNextASN1], 0, 4);
            //param.Length = z;
            param.Length = param.Length | (m_pASN1Data[m_pNextASN1 + 0] & 0x000000ff);
            param.Length = param.Length | (m_pASN1Data[m_pNextASN1 + 1] & 0x000000ff) << 8;
            param.Length = param.Length | (m_pASN1Data[m_pNextASN1 + 2] & 0x000000ff) << 16;
            param.Length = param.Length | (m_pASN1Data[m_pNextASN1 + 3] & 0x000000ff) << 24;
            
            m_pNextASN1 += 4; // sizeof(length) 32 bits 
            param.Tag = m_pASN1Data[m_pNextASN1++] & 0x000000FF; // fetch byte tag
            if (typeof m_pASN1Data[m_pNextASN1] == 'string') {
                param.pdata = "";
                for (i = 0; i < param.Length; i++)
                    param.pdata += m_pASN1Data[m_pNextASN1 + i];
            }
            else
            {
                if (param.Length == 1)
                {
                    param.pdata = m_pASN1Data[m_pNextASN1];
                }
                else
                {
                    if (param.Length == 2)
                        param.pdata = m_pASN1Data[m_pNextASN1];
                    else {
                        param.pdata = new Array(param.Length);
                        for (i = 0; i < param.Length; i++)
                            param.pdata[i] = m_pASN1Data[m_pNextASN1 + i];
                    }
                }
            }
            NextASN1Position = CurrentASN1Position + param.Length + 1 + 4;
            if (NextASN1Position > m_iTotalLengthOfData || NextASN1Position < 0) {
                m_pNextASN1 = 0;
                bResult = false; // ASN1 says it is longer than ASN1 allocated space ??? ASN1 has illegal size.
            }
            else {
                CurrentASN1Position = NextASN1Position;
                if (CurrentASN1Position >= m_iTotalLengthOfData)
                    m_pNextASN1 = 0;
                else
                    m_pNextASN1 += param.Length;
            }
        }
        else
            bResult = false;
    }
    catch (error)
    {
        bResult = false;
    }
    return bResult;
}

/*
 var param = {
 Length:0,
 pdata:0
 };
 */
CASN1.prototype.FetchTotalASN1 = function(param) // Returns true if ASN1 was found, and false if not.
{
    var bResult = true;
    try {
        if (m_pASN1Data != 0)
        {
            param.Length = m_iLengthOfData;
            param.pdata = new Array(param.Length);
            for (i = 0; i < param.Length; i++)
                param.pdata[i] = m_pASN1Data[i];

        }
        else
            bResult = false;
    } catch (error)
    {
        bResult = false;
    }
    return bResult;
}

//////////////////////////////////////////////
// class CDataEncoder
//////////////////////////////////////////////
function CDataEncoder()
{
    var m_pdata;
    var m_iLengthOfData;
    var m_ptotaldata;
    var m_iLengthOfTotalData;
    var m_asn1; // used in decoder
}

//////////////////////////////////////////////
// GENERAL ADD ELEMENTS INTO ASN1 STRUCTURE //
//////////////////////////////////////////////
//TODO: MAKE SURE THAT NUMBERS ARE WRITTEN AS BINARY INTO STRUCTURE
// http://stackoverflow.com/questions/5688042/how-to-convert-a-java-string-to-an-ascii-byte-array
//
CDataEncoder.prototype.AddElement = function(param) {

    var result = -1;

    if (param.ElementType == DED_ELEMENT_TYPE_STRUCT)
    { // First element in structure
        m_pdata = 0;
        m_iLengthOfData = 0;
        m_ptotaldata = 0;
        m_iLengthOfTotalData = 0;
        var asn1 = new CASN1();
        var result = asn1.CASN1p1(param.name.length + 4 + 1);

        var LengthOfAsn1 = param.name.length;
        asn1.AppendASN1(LengthOfAsn1, param.ElementType, param.name);
        var paramasn1 = {
            Length: 0,
            pdata: 0
        };

        asn1.FetchTotalASN1(paramasn1);
        m_iLengthOfTotalData = paramasn1.Length;
        m_pdata = new Array(m_iLengthOfData);
        for (i = 0; i < paramasn1.Length; i++)
            m_pdata[i] = paramasn1.pdata[i];

        m_ptotaldata = m_pdata;
    }
    else
    {
        /* example:
         * 	var param = {
         name:name,
         ElementType:DED_ELEMENT_TYPE_METHOD,
         value:value
         length: value.length // needed if value is different from a string, then length function does not exist
         };
         */
        try {
            var asn1 = new CASN1();
            var result = asn1.CASN1p3(m_iLengthOfTotalData, m_pdata, m_iLengthOfTotalData + param.name.length + param.length + 1);

            // 1. asn  "name"	
            var LengthOfAsn1 = param.name.length;
            asn1.AppendASN1(LengthOfAsn1, param.ElementType, param.name);
            // 2. asn "value"
            LengthOfAsn1 = param.length;
            if (LengthOfAsn1 > 0)
                asn1.AppendASN1(LengthOfAsn1, param.ElementType, param.value);

            var paramasn1 = {
                Length: 0,
                pdata: 0
            };
            asn1.FetchTotalASN1(paramasn1);
            m_iLengthOfTotalData = paramasn1.Length;

            if (m_iLengthOfTotalData > 0)
            {
                m_pdata = new Array(m_iLengthOfTotalData);
                for (i = 0; i < m_iLengthOfTotalData; i++)
                    m_pdata[i] = paramasn1.pdata[i];

                m_ptotaldata = m_pdata;
            }
        } catch (error) {
            output("\nFAIL DURING ADD TO ASN1 DATA AREA\n");
            result = -1
        }

    }
    if (m_iLengthOfTotalData > 0)
        result = m_iLengthOfTotalData;
    return result;
}

CDataEncoder.prototype.EncodeStructStart = function(name) {

    var result = -1;
    var param = {
        name: name,
        ElementType: DED_ELEMENT_TYPE_STRUCT
    };
    result = this.AddElement(param);

    return result;
}
CDataEncoder.prototype.EncodeStructEnd = function(name) {
    var result = -1;
    var value = -1;
    var length = 0;
    var param = {
        name: name,
        ElementType: DED_ELEMENT_TYPE_STRUCT_END,
        value: value,
        length: length
    };
    result = this.AddElement(param);

    return result;
}
CDataEncoder.prototype.EncodeMethod = function(name, value, length) {

    var result = -1;
    var param = {
        name: name,
        ElementType: DED_ELEMENT_TYPE_METHOD,
        value: value,
        length: length
    };
    result = this.AddElement(param);

    return result;
}



CDataEncoder.prototype.EncodeUShort = function(name, value, length) {

    var result = -1;
    var param = {
        name: name,
        ElementType: DED_ELEMENT_TYPE_USHORT,
        value: value,
        length: length
    };
    result = this.AddElement(param);

    return result;
}

CDataEncoder.prototype.EncodeLong = function(name, value, length) {

    var result = -1;
    var param = {
        name: name,
        ElementType: DED_ELEMENT_TYPE_LONG,
        value: value,
        length: length
    };
    result = this.AddElement(param);

    return result;
}

CDataEncoder.prototype.EncodeBool = function(name, value, length) {

    var result = -1;
    var param = {
        name: name,
        ElementType: DED_ELEMENT_TYPE_BOOL,
        value: value,
        length: length
    };
    result = this.AddElement(param);

    return result;
}

CDataEncoder.prototype.EncodeStdString = function(name, value, length) {

    var result = -1;
    var param = {
        name: name,
        ElementType: DED_ELEMENT_TYPE_STDSTRING,
        value: value,
        length: length
    };
    result = this.AddElement(param);

    return result;
}

CDataEncoder.prototype.Encodestdvector = function(name, value, length) {

    var result = -1;
    var param = {
        name: name,
        ElementType: DED_ELEMENT_TYPE_STDVECTOR,
        value: value,
        length: length
    };
    result = this.AddElement(param);

    return result;
}

function _Elements() {
    var strElementID;
    var ElementData;
}

CDataEncoder.prototype.EncodeElement = function(entityname, elementname, elementvalue) {

    var result = -1;
    var element = new _Elements();
    element.strElementID = elementname;
    element.ElementData = elementvalue;

    //TODO: add to_lower on both below strings
    var strentity_chunk_id = entityname + "_chunk_id";
    var strentity_chunk_data = entityname + "_chunk_data";

    result = this.EncodeStdString(strentity_chunk_id, element.strElementID, element.strElementID.length); // key of particular item
    if(result != -1) 
        result = this.Encodestdvector(strentity_chunk_data, element.ElementData, elementvalue.length); //
    return result;
}


CDataEncoder.prototype.DataEncoder_GetData = function(DEDobject) {

    var result = -1;
    if (m_ptotaldata.length > 0) {
        DEDobject.iLengthOfTotalData = m_iLengthOfTotalData;
        result = m_ptotaldata;
    }
    return result;
}
////////////////////////////////////////////////////////////////
// FETCH ELEMENTS FROM ASN1 DATAENCODER 
////////////////////////////////////////////////////////////////
CDataEncoder.prototype.GetElement = function(DEDobject) {
    /* example:
     var DEDobject = {
     name:name,
     elementtype:DED_ELEMENT_TYPE_METHOD,
     value:-1
     };
     */
    var result = -1;

    if (DEDobject.elementtype == DED_ELEMENT_TYPE_STRUCT)
    {
        m_asn1 = new CASN1();
        result = m_asn1.CASN1p3(this.m_iLengthOfTotalData, this.m_pdata, this.m_iLengthOfTotalData + 1);
    }


    var value = DEDobject.value;
    var ElementType = DEDobject.elementtype;
    var param = {
        Length: 0,
        Tag: 0,
        pdata: 0
    };
    if (m_asn1.FetchNextASN1(param))
    {
        if (param.Tag == ElementType)
        {
            var strCmp = "";
            if (typeof param.pdata !== 'string')
                strCmp = String.fromCharCode.apply(null, param.pdata);
            else
                strCmp = param.pdata;
            if (DEDobject.name == strCmp)
            {
                if (param.Tag == DED_ELEMENT_TYPE_STRUCT || param.Tag == DED_ELEMENT_TYPE_STRUCT_END)
                {
                    // start and end elements does NOT have value, thus no need to go further
                    result = 1;
                }
                else {
                    param.Length = 0;
                    param.Tag = 0;
                    param.pdata = 0;
                    if (m_asn1.FetchNextASN1(param))
                    {
                        if (param.Tag == ElementType)
                        {
                            if (ElementType == DED_ELEMENT_TYPE_METHOD || ElementType == DED_ELEMENT_TYPE_STRING || ElementType == DED_ELEMENT_TYPE_STDSTRING)
                            {
                                var str = "";
                                //if (typeof param.pdata !== 'string')
                                if (typeof param.pdata === 'object')
                                    str = String.fromCharCode.apply(null, param.pdata);
                                else
                                    str = param.pdata;
                                DEDobject.value = str;
                            }
                            else
                            {
                                DEDobject.value = param.pdata;
                            }
                            result = 1;
                        }
                    }
                }
            }
        }
    }
    return result;
}


/////////////////////////////////////////////
// DEFINES                                 //
/////////////////////////////////////////////
function DED_START_ENCODER()
{
    encoder_ptr = new CDataEncoder();

    return encoder_ptr;
}

function DED_PUT_STRUCT_START(encoder_ptr, name)
{
    var result = -1;

    if (encoder_ptr != 0)
        result = encoder_ptr.EncodeStructStart(name);

    return result;
}

function DED_PUT_STRUCT_END(encoder_ptr, name)
{
    var result = -1;

    if (encoder_ptr != 0)
        result = encoder_ptr.EncodeStructEnd(name);

    return result;
}

function DED_PUT_METHOD(encoder_ptr, name, value)
{
    var result = -1;
    if (encoder_ptr != 0)
        result = encoder_ptr.EncodeMethod(name, value, value.length);

    return result;
}

function DED_PUT_USHORT(encoder_ptr, name, value)
{
    var result = -1;
    if (encoder_ptr != 0)
        result = encoder_ptr.EncodeUShort(name, value, 1);

    return result;
}

function DED_PUT_LONG(encoder_ptr, name, value)
{
    var result = -1;
    if (encoder_ptr != 0)
        result = encoder_ptr.EncodeLong(name, value, 1);

    return result;
}


function DED_PUT_BOOL(encoder_ptr, name, value)
{
    var result = -1;
    var boolvalue = 0; // 0 == false, 1 == true 
    if (encoder_ptr != 0) {
        if (value == true)
            boolvalue = 1;
        result = encoder_ptr.EncodeBool(name, boolvalue, 1);
    }
    return result;
}

function DED_PUT_STDSTRING(encoder_ptr, name, value)
{
    var result = -1;
    if (encoder_ptr != 0) {
        if(value === "") value = "##empty##";
        result = encoder_ptr.EncodeStdString(name, value, value.length);
    }
    return result;
}

function DED_PUT_ELEMENT(encoder_ptr, entityname, elementname, elementvalue)
{
    var result = -1;
    if (encoder_ptr != 0) {
        result = encoder_ptr.EncodeElement(entityname, elementname, elementvalue);
    }
    return result;
}

function DED_GET_ENCODED_DATA(DEDobject)
{
    /* var DEDobject = {
     *	encoder_ptr: encoder_ptr,
     *	uncompresseddata: ,
     *	iLengthOfTotalData: ,
     *	pCompressedData: ,
     *	sizeofCompressedData: 
     *};
     */
    var result = -1;
    if (DEDobject.encoder_ptr != 0)
        DEDobject.uncompresseddata = DEDobject.encoder_ptr.DataEncoder_GetData(DEDobject);

    // Do compression - okumura style
    // First make sure byte are in same format !!!!
    var uncmpdata = new Uint8Array(DEDobject.uncompresseddata.length);
    for(i=0;i<DEDobject.uncompresseddata.length;i++){
            var str = DEDobject.uncompresseddata[i];
            if(typeof str == 'string'){
                    uncmpdata[i] = str.charCodeAt(); 
            }
            else
                    uncmpdata[i] = DEDobject.uncompresseddata[i]; 
    }
    // now make room for case where compression yields lager size - when trying to compress an image for example.
    var tmpCompressedData = new Array(uncmpdata.length * 2)
    // now compress
    var compressedSize = compress_lzss(tmpCompressedData, tmpCompressedData.length*2, uncmpdata, uncmpdata.length);
    if (compressedSize > 0) {
        DEDobject.pCompressedData = new Array(compressedSize);
        DEDobject.sizeofCompressedData = compressedSize;
        for (i = 0; i < compressedSize; i++)
            DEDobject.pCompressedData[i] = tmpCompressedData[i];
        result = 1;
    }
    else
    {
       // somehow compression went wrong !!!! ignore and just use uncompressed data - perhaps data was already compressed !?
        DEDobject.pCompressedData = new Array(uncmpdata.length);
        DEDobject.sizeofCompressedData = uncmpdata.length;
        for (i = 0; i < DEDobject.uncompresseddata.length; i++)
            DEDobject.pCompressedData[i] = uncmpdata[i];
        result = 1;
    }
    
//    var tmpCompressedData = new Array(DEDobject.uncompresseddata.length * 2)
//    var compressedSize = compress_lzss(tmpCompressedData, tmpCompressedData.length, DEDobject.uncompresseddata, DEDobject.uncompresseddata.length);
//    if (compressedSize > 0) {
//        DEDobject.pCompressedData = new Array(compressedSize);
//        DEDobject.sizeofCompressedData = compressedSize;
//        for (i = 0; i < compressedSize; i++)
//            DEDobject.pCompressedData[i] = tmpCompressedData[i];
//        result = 1;
//    }
//    else
//    {
//       // somehow compression went wrong !!!! ignore and just use uncompressed data - perhaps data was already compressed !?
//        DEDobject.pCompressedData = new Array(DEDobject.uncompresseddata.length);
//        DEDobject.sizeofCompressedData = DEDobject.uncompresseddata.length;
//        for (i = 0; i < DEDobject.uncompresseddata.length; i++)
//            DEDobject.pCompressedData[i] = DEDobject.uncompresseddata[i];
//        result = 1;
//    }
//    delete tmpCompressedData;

///////////////////////////test
/////+test
//    // Do compression - okumura style
//    var tmpCompressedData = new Array(DEDobject.uncompresseddata.length * 2)
//    var compressedSize = compress_lzss(tmpCompressedData, tmpCompressedData.length, DEDobject.uncompresseddata, DEDobject.uncompresseddata.length);
//    if (compressedSize > 0) {
//        DEDobject.pCompressedData = new Array(compressedSize);
//        DEDobject.sizeofCompressedData = compressedSize;
//        for (i = 0; i < compressedSize; i++)
//            DEDobject.pCompressedData[i] = tmpCompressedData[i];
//        result = 1;
//    }
////-test
//
//    // Do compression jsLiquid style
//    //loads the Data module on demand:
//    jsl.require("Data")
//    var Data = jsl.Data;
//    var Blob = Data.Blob;
//    
//    //encodes a JavaScript string into a sequence
//    //of UTF-8 bytes:
//    //var s = "testtesttesttest";
//    //var blob = Data.toUtf8(s);
//    
//    var uncmpdata = new Uint8Array(DEDobject.uncompresseddata.length);
//    for(i=0;i<DEDobject.uncompresseddata.length;i++){
//            var str = DEDobject.uncompresseddata[i];
//            if(typeof str == 'string'){
//                    uncmpdata[i] = str.charCodeAt(); 
//            }
//            else
//                    uncmpdata[i] = DEDobject.uncompresseddata[i]; 
//    }
//    
//    var blob = new Blob(uncmpdata);
//
//    ///compresses the UTF-8 text using the LzSS
//    //algorithm:
//    //var lzss = Data.toLzSS(blob)
//    var cmprsdBlob = Data.toLzSS(blob);
//    DEDobject.pCompressedData = cmprsdBlob.toArray();
//   
//    
/////+test
//    // Do compression - okumura style
//    var tmpCompressedData2 = new Array(DEDobject.uncompresseddata.length * 2)
//    var compressedSize2 = compress_lzss(tmpCompressedData2, tmpCompressedData2.length, uncmpdata, uncmpdata.length);
//    if (compressedSize2 > 0) {
//        var pCmprsd = new Array(compressedSize2);
//        for (i = 0; i < compressedSize2; i++)
//            pCmprsd[i] = tmpCompressedData2[i];
//        result = 1;
//    }
////-test
    
    return result;
}

function DED_PUT_DATA_IN_DECODER(pCompressedData, sizeofCompressedData)
{
    var decoder_ptr = new CDataEncoder();

    var decmprsd = decompress_lzss(pCompressedData, sizeofCompressedData);
    if (decmprsd.length > 0) {
        decoder_ptr.m_ptotaldata = decmprsd;
        m_ptotaldata = decoder_ptr.m_ptotaldata;
        decoder_ptr.m_pdata = m_ptotaldata;
        decoder_ptr.m_iLengthOfTotalData = decmprsd.length;
        m_iLengthOfTotalData = decoder_ptr.m_iLengthOfTotalData;
    }
    return decoder_ptr;
}

function DED_GET_STRUCT_START(decoder_ptr, name)
{
    var result = -1;
    var DEDobject = {
        name: name,
        elementtype: DED_ELEMENT_TYPE_STRUCT,
        value: -1
    };
    result = decoder_ptr.GetElement(DEDobject);

    return result;
}

function DED_GET_METHOD(decoder_ptr, name)
{
    var result = -1;
    var DEDobject = {
        name: name,
        elementtype: DED_ELEMENT_TYPE_METHOD,
        value: -1
    };
    result = decoder_ptr.GetElement(DEDobject);
    if (result == 1)
        result = DEDobject.value;
    else
        result = -1;
    return result;
}

function DED_GET_USHORT(decoder_ptr, name)
{
    var result = -1;
    var DEDobject = {
        name: name,
        elementtype: DED_ELEMENT_TYPE_USHORT,
        value: -1
    };
    result = decoder_ptr.GetElement(DEDobject);
    if (result == 1)
        result = DEDobject.value;
    else
        result = -1;
    return result;
}

function DED_GET_LONG(decoder_ptr, name)
{
    var result = -1;
    var DEDobject = {
        name: name,
        elementtype: DED_ELEMENT_TYPE_LONG,
        value: -1
    };
    result = decoder_ptr.GetElement(DEDobject);
    if (result == 1)
        result = DEDobject.value;
    else
        result = -1;
    return result;
}

function DED_GET_BOOL(decoder_ptr, name)
{
    var result = -1;
    var DEDobject = {
        name: name,
        elementtype: DED_ELEMENT_TYPE_BOOL,
        value: -1
    };
    result = decoder_ptr.GetElement(DEDobject);
    if (result == 1)
    {
        if (DEDobject.value == 1)
            result = true;
        else
            result = false;
    }
    else
        result = -1;
    return result;
}

function emptycheck(str)
{
    var strreturn="";
    if(str==="##empty##")
        strreturn = "";
    else
        strreturn = str;
    return strreturn;
}

function DED_GET_STDSTRING(decoder_ptr, name)
{
    var result = -1;
    var DEDobject = {
        name: name,
        elementtype: DED_ELEMENT_TYPE_STDSTRING,
        value: -1
    };
    result = decoder_ptr.GetElement(DEDobject);
    if (result == 1) {
        result = DEDobject.value;
        result = emptycheck(result);
    }
    else
        result = -1;
    return result;
}

//TODO: 20140724 consider designing _GET_ so that if element is NOT found, then internal pointer is NOT moved as it is NOW!!!
function DED_GET_ELEMENT(decoder_ptr, entityname, elementvalue)
{
    var result = -1;
  
    //TODO: add to_lower on both below strings
    var strentity_chunk_id = entityname + "_chunk_id";
    var strentity_chunk_data = entityname + "_chunk_data";

    var DEDobject1 = {
        name: strentity_chunk_id,
        elementtype: DED_ELEMENT_TYPE_STDSTRING,
        value: -1
    };
    var DEDobject2 = {
        name: strentity_chunk_data,
        elementtype: DED_ELEMENT_TYPE_STDVECTOR,
        value: -1
    };
    
    result = decoder_ptr.GetElement(DEDobject1);
    if (result == 1)
        result = DEDobject1.value;
    else
        result = -1;
    if (result != -1){
        result = decoder_ptr.GetElement(DEDobject2);
        if (result == 1)
            result = DEDobject2.value;
        else
            result = -1;
    }
    elementvalue.strElementID = DEDobject1.value;
    elementvalue.ElementData = DEDobject2.value;
    if(result != -1) result = 1;
    return result;
}

// ...
//
//

function DED_GET_STRUCT_END(decoder_ptr, name)
{
    var result = -1;
    var DEDobject = {
        name: name,
        elementtype: DED_ELEMENT_TYPE_STRUCT_END,
        value: -1
    };
    result = decoder_ptr.GetElement(DEDobject);

    return result;
}


