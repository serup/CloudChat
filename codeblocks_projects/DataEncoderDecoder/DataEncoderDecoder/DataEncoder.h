/**************************************************************
 DataEncoder.h -- Encodes / Decodes data
***************************************************************
    3/5/2013 Johnny Serup
    Use, distribute, and modify this program freely.
    Please send me your improved versions.
        johnny.serup@scanva.com
        use "Att.: Improvements to DataEncoder"

**************************************************************/
// http://www.drdobbs.com/cpp/c11-uniqueptr/240002708

#include <vector>
#include <string>
//#pragma comment( user, "Compiled on " __DATE__ " at " __TIME__ )
//#pragma message( "Compiling " __FILE__ )
//#pragma message( "Last modified on " __TIMESTAMP__ )
//#pragma once
#include <sys/stat.h>
#include "ASN1.h"
//#include "compression-lib/compression.h"
/*
#include "svg_utils\handlesvg.h"
#include "Rpc.h"					// Needed when using GUIDgen
#pragma comment(lib, "rpcrt4.lib")	// Needed when using GUIDgen
#include "objbase.h"				// Needed when using GUIDgen
#pragma comment(lib, "ole32.lib")	// Needed when using GUIDgen
*/

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

typedef unsigned short WORD;
typedef unsigned long DWORD;
#define uint8  unsigned char
#define uint16 unsigned short
#define int32  long
#define ulong  DWORD

// TODO:Use this bundle object class to complex transfers
// add elements to it here !!!
//class CBundleObject : public CObject
class CBundleObject
{
public:
	CBundleObject(void)
	{
	};
	~CBundleObject(void)
	{
	};

	int iObjID;

};

#ifndef Elements
struct Elements
{
    std::string strElementID;
    std::vector<unsigned char> ElementData;
};
#endif

class CDataEncoder
{
public:
	CDataEncoder(void);
	CDataEncoder(unsigned char* data_ptr, int iLengthOfdata);
	~CDataEncoder(void);

	void EncodeStructStart(std::string name);
	void AddElement(std::string name, int ElementType);
	void EncodeMethod(std::string method, std::string value);
	void Encodestdstring(std::string method, std::string value);
	void AddElement(std::string name, int ElementType, std::string value);
	void Encodestdvector(std::string method, std::vector<unsigned char> value);
	void AddElement(std::string name, int ElementType, std::vector<unsigned char> value);
	void EncodeUShort(std::string name, uint16 value);
	void AddElement(std::string name, int ElementType, uint16 value);
	void EncodeBool(std::string name, bool value);
	void AddElement(std::string name, int ElementType, bool value);
	void EncodeChar(std::string name, char value);
	void AddElement(std::string name, int ElementType, char value);
	void EncodeByte(std::string name, __byte value);
	void AddElement(std::string name, int ElementType, __byte value);
	void EncodeLong(std::string name, long value);
	void AddElement(std::string name, int ElementType, long value);
	void EncodeULong(std::string name, ulong value);
	void AddElement(std::string name, int ElementType, ulong value);
	void EncodeFloat(std::string name, float value);
	void AddElement(std::string name, int ElementType, float value);
	void EncodeCBundleObject(std::string name, CBundleObject* value);
	void AddElement(std::string name, int ElementType, CBundleObject* value);
	void EncodeClassInstance(std::string name, void* value, int sizeofclass);
	void AddElement(std::string name, int ElementType, void* value, int sizeofclass);
//	void EncodeSVGfile(std::string name, std::string filename);
	void EncodeVA_LIST(std::string name, va_list value, int size);
	void AddElement(std::string name, int ElementType, va_list value, int size);
	void AddElement(std::string name, int ElementType, struct stat value);
    void EncodeElement(std::string entityname, std::string elementname, std::vector<unsigned char> elementvalue);


	void Add(std::string name, int ElementType, const char* value,int iLength); // General add

//..
	void EncodeStructEnd(std::string name);
	__byte* DataEncoder_GetData(int& iLengthOfTotalData);

	bool DecodeStructStart(std::string name);
	bool GetElement(std::string name, int ElementType);
	bool DecodeMethod(std::string name, std::string& value);
	bool GetElement(std::string name, int ElementType, std::string& value);
	bool DecodeUShort(std::string name,uint16& value);
	bool GetElement(std::string name, int ElementType, uint16& value);
	bool DecodeBool(std::string name, bool& value);
	bool GetElement(std::string name, int ElementType, bool& value);
	bool DecodeChar(std::string name, char& value);
	bool GetElement(std::string name, int ElementType, char& value);
	bool Decode__byte(std::string name, __byte& value);
	bool GetElement(std::string name, int ElementType, __byte& value);
	bool DecodeLong(std::string name, long& value);
	bool GetElement(std::string name, int ElementType, long& value);
	bool DecodeULong(std::string name, ulong& value);
	bool GetElement(std::string name, int ElementType, ulong& value);
	bool DecodeFloat(std::string name, float& value);
	bool GetElement(std::string name, int ElementType, float& value);
	bool DecodeCBundleObject(std::string name, CBundleObject& value);
	bool Decodestdstring(std::string name, std::string& value);
	bool GetElement(std::string name, int ElementType, __byte** value);
	bool GetElement(std::string name, int ElementType, __byte** value,DWORD& length );
	bool Decodestdvector(std::string name, std::vector<unsigned char>& value);
	bool GetElement(std::string name, int ElementType, std::vector<unsigned char>& value);
//	bool DecodeClassInstance(std::string name, CObject& value);
//	bool DecodeSVGfile(std::string name, __byte** value);
    bool DecodeEntityElement(std::string entityname, Elements& __element);
    bool DecodeToast(std::string entityname, std::vector<Elements> &_vecElements);
	bool DecodeVA_LIST(std::string name, va_list& value);

	bool Get(std::string name, int ElementType,__byte* pdata,WORD& length); // General get
//..
	bool DecodeStructEnd(std::string name);


private:
	 std::unique_ptr<__byte> pasn1data;
	 std::unique_ptr<CASN1> pasn1FetchData;
//	 CPtrList svgObjectsForDeletion;
//	 CPtrList va_list_ObjectsForDeletion;

	 __byte* m_pdata;
	 int  m_iLengthOfData;
	 __byte* m_ptotaldata;
	 int  m_iLengthOfTotalData;


};

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
#define DED_ELEMENT_TYPE_ZERO    17
#define DED_ELEMENT_TYPE_BOOL    0
#define DED_ELEMENT_TYPE_CHAR    1
#define DED_ELEMENT_TYPE_BYTE    2
//#define DED_ELEMENT_TYPE_WCHAR   3
//#define DED_ELEMENT_TYPE_SHORT   4
#define DED_ELEMENT_TYPE_USHORT  5
#define DED_ELEMENT_TYPE_LONG    6
#define DED_ELEMENT_TYPE_ULONG   7
#define DED_ELEMENT_TYPE_FLOAT   8
//#define DED_ELEMENT_TYPE_DOUBLE  9
#define DED_ELEMENT_TYPE_STRING  10
//#define DED_ELEMENT_TYPE_WSTRING 11
//#define DED_ELEMENT_TYPE_URI     12
#define DED_ELEMENT_TYPE_METHOD  13
#define DED_ELEMENT_TYPE_STRUCT  14
//#define DED_ELEMENT_TYPE_LIST    15
//#define DED_ELEMENT_TYPE_ARRAY   16
#define DED_ELEMENT_TYPE_COBJECT  17
#define DED_ELEMENT_TYPE_CLASS 18
#define DED_ELEMENT_TYPE_STDSTRING 19
#define DED_ELEMENT_TYPE_SVGFILE 20
#define DED_ELEMENT_TYPE_VA_LIST 21
//#define DED_ELEMENT_TYPE_FILE 22

#define DED_ELEMENT_TYPE_STDVECTOR 23
#define DED_ELEMENT_TYPE_STRUCT_END  24
#define DED_ELEMENT_TYPE_UNKNOWN 0xff

// Encode
#define DED_PUT_STRUCT_START(DataEncoderptr, name) DataEncoderptr->EncodeStructStart((std::string)name)

#define DED_PUT_METHOD(DataEncoderptr, name, value) DataEncoderptr->EncodeMethod((std::string)name,(std::string)value)
#define DED_PUT_USHORT(DataEncoderptr, name, value) DataEncoderptr->EncodeUShort((std::string)name,value)
#define DED_PUT_BOOL(DataEncoderptr, name, value) DataEncoderptr->EncodeBool((std::string)name,value)
#define DED_PUT_CHAR(DataEncoderptr, name, value) DataEncoderptr->EncodeChar((std::string)name,value)
#define DED_PUT_BYTE(DataEncoderptr, name, value) DataEncoderptr->EncodeByte((std::string)name,value)
#define DED_PUT_LONG(DataEncoderptr, name, value) DataEncoderptr->EncodeLong((std::string)name,value)
#define DED_PUT_ULONG(DataEncoderptr, name, value) DataEncoderptr->EncodeULong((std::string)name,value)
#define DED_PUT_FLOAT(DataEncoderptr, name, value) DataEncoderptr->EncodeFloat((std::string)name,value)
#define DED_PUT_COBJECT(DataEncoderptr, name, value) DataEncoderptr->EncodeCBundleObject((std::string)name,value)
#define DED_PUT_STDSTRING(DataEncoderptr, name, value) DataEncoderptr->Encodestdstring((std::string)name,(std::string)value)
#define DED_PUT_STDVECTOR(DataEncoderptr, name, value) DataEncoderptr->Encodestdvector((std::string)name,(std::vector<unsigned char>)value)
#define DED_PUT_CLASS(DataEncoderptr, name, value, size) DataEncoderptr->EncodeClassInstance((std::string)name,value, size)
#define DED_PUT_SVGFILE(DataEncoderptr, name, filename) DataEncoderptr->EncodeSVGfile((std::string)name,(std::string)filename)
#define DED_PUT_VA_LIST(DataEncoderptr, name, value, size) DataEncoderptr->EncodeVA_LIST((std::string)name,value, size)
#define _DED_PUT_ELEMENT(DataEncoderptr, entityname, elementname, elementvalue) DataEncoderptr->EncodeElement((std::string)entityname,(std::string)elementname,(std::vector<unsigned char>) elementvalue)
//...

#define ADD_ELEMENT(_vecElements,name,value) {std::vector<std::string> strVecValues = {(std::string)value};\
        Elements element;\
        element.strElementID = (std::string)name;\
        std::copy(strVecValues[0].begin(), strVecValues[0].end(), std::back_inserter(element.ElementData));\
        _vecElements.push_back(element);}

#define DED_PUT_ELEMENT(_encoder_ptr,_entity,_name,_value) \
        { std::vector<Elements> __vecElements; \
        ADD_ELEMENT(__vecElements, _name,_value); {\
        Elements _e;\
        _e.strElementID = (std::string)_name;\
        std::vector<std::string> _strVecValues = {(std::string)_value};\
        std::copy(_strVecValues[0].begin(), _strVecValues[0].end(), std::back_inserter(_e.ElementData));\
        _DED_PUT_ELEMENT( _encoder_ptr, _entity, _e.strElementID, _e.ElementData ); }}\

#define DED_PUT_VECTOR_ELEMENT(_encoder_ptr,_entity,_name,_value) \
        {Elements _e;\
        _e.strElementID = (std::string)_name;\
        _e.ElementData = _value;\
        _DED_PUT_ELEMENT( _encoder_ptr, _entity, _e.strElementID, _e.ElementData ); }\

#define DED_PUT_TOAST_ELEMENTS( _encoder_ptr, _realmname, _record_values ) \
        {\
            DED_PUT_STDSTRING	( _encoder_ptr, "STARTtoast", (std::string)"elements" );\
            BOOST_FOREACH(Elements _f, _record_values)\
            {\
                std::string strtmp = _realmname;\
                std::string _realmname_lower = boost::to_lower_copy(strtmp);\
                DED_PUT_VECTOR_ELEMENT( _encoder_ptr, _realmname_lower, (std::string)_f.strElementID, _f.ElementData )\
            }\
            DED_PUT_STDSTRING	( encoder_ptr, "ENDtoast", (std::string)"elements" );\
        }\


//NB! Automatic Pointer [deprecated]
//Note: This class template is deprecated as of C++11. unique_ptr is a new facility with a similar functionality, but with improved security (no fake copy assignments), added features (deleters) and support for arrays. See unique_ptr for additional information.

#define DED_START_ENCODER(encoder_ptr) std::unique_ptr<CDataEncoder> encoder_ptr( new CDataEncoder() );
#define DED_GET_ENCODED_DATA(DataEncoderptr, value, length,pCompressedData,sizeofCompressedData) \
		int length;\
		__byte* value = (__byte*)DataEncoderptr->DataEncoder_GetData(iLengthOfTotalData);\
		DED_COMPRESS_DATA(value,length,pCompressedData,sizeofCompressedData);

#define DED_GET_DATAFRAME(DataEncoderptr, value, length,pCompressedData,sizeofCompressedData,_tempframe) \
		int length;\
		__byte* value = (__byte*)DataEncoderptr->DataEncoder_GetData(iLengthOfTotalData);\
		DED_COMPRESS_DATA(value,length,pCompressedData,sizeofCompressedData);\
        _tempframe.opcode = dataframe::binary_frame;\
        if(sizeoftmpCompressedData==0) sizeoftmpCompressedData = iLengthOfTotalData;\
            _tempframe.putBinaryInPayload(ptmpCompressedData,sizeoftmpCompressedData);

#define DED_GET_ENCODED_DATAFRAME(_encoder_ptr,_tempframe) \
        DED_GET_DATAFRAME(_encoder_ptr,data_ptr,iLengthOfTotalData,ptmpCompressedData,sizeoftmpCompressedData,_tempframe);

#define DED_PUT_STRUCT_END(DataEncoderptr, name) DataEncoderptr->EncodeStructEnd((std::string)name)

//conversion dataframe use when wsclient::dataframe is in use
#define DED_GET_WSC_DATAFRAME(DataEncoderptr, value, length,pCompressedData,sizeofCompressedData,___tempframe) \
		int length;\
		__byte* value = (__byte*)DataEncoderptr->DataEncoder_GetData(iLengthOfTotalData);\
		DED_COMPRESS_DATA(value,length,pCompressedData,sizeofCompressedData);\
        ___tempframe.opcode = wsclient::dataframe::operation_code::binary_frame;\
        if(sizeoftmpCompressData==0) sizeoftmpCompressData = iLengthOfTotalData;\
            ___tempframe.putBinaryInPayload(ptmpCompressData,sizeoftmpCompressData);

#define DED_GET_WSCLIENT_DATAFRAME(_encoder_ptr,__tempframe) \
        DED_GET_WSC_DATAFRAME(_encoder_ptr,data_ptr,iLengthOfTotalData,ptmpCompressData,sizeoftmpCompressData,__tempframe);

// Decode

#define DED_GET_STRUCT_START(DataEncoderptr, name) DataEncoderptr->DecodeStructStart((std::string)name)
#define DED_GET_METHOD(DataEncoderptr, name, value) DataEncoderptr->DecodeMethod((std::string)name,(std::string&)value)
#define DED_GET_USHORT(DataEncoderptr, name, value) DataEncoderptr->DecodeUShort((std::string)name,(uint16&)value)
#define DED_GET_BOOL(DataEncoderptr, name, value) DataEncoderptr->DecodeBool((std::string)name,(bool&)value)
#define DED_GET_CHAR(DataEncoderptr, name, value) DataEncoderptr->DecodeChar((std::string)name,(char&)value)
#define DED_GET_BYTE(DataEncoderptr, name, value) DataEncoderptr->DecodeByte((std::string)name,(__byte&)value)
#define DED_GET_LONG(DataEncoderptr, name, value) DataEncoderptr->DecodeLong((std::string)name,(long&)value)
#define DED_GET_ULONG(DataEncoderptr, name, value) DataEncoderptr->DecodeULong((std::string)name,(ulong&)value) //#define DED_GET_ULONG(DataEncoderptr, name, value) DataEncoderptr->GetElement((std::string)name,DED_ELEMENT_TYPE_ULONG,(ulong&)value)
#define DED_GET_FLOAT(DataEncoderptr, name, value) DataEncoderptr->DecodeFloat((std::string)name,(float&)value)
#define DED_GET_COBJECT(DataEncoderptr, name, value) DataEncoderptr->DecodeCBundleObject((std::string)name,(CBundleObject&)value)
#define DED_GET_STDSTRING(DataEncoderptr, name, value) DataEncoderptr->Decodestdstring((std::string)name,(std::string&)value)
#define DED_GET_STDVECTOR(DataEncoderptr, name, value) DataEncoderptr->Decodestdvector((std::string)name,(std::vector<unsigned char>&)value)
//#define DED_GET_CLASS(DataEncoderptr, name, value) DataEncoderptr->DecodeClassInstance((std::string)name,(CObject&)value)
//#define DED_GET_SVGFILE(DataEncoderptr, name, value) DataEncoderptr->DecodeSVGfile((std::string)name,(__byte**)&value)
//#define DED_GET_VA_LIST(DataEncoderptr, name, value) DataEncoderptr->DecodeVA_LIST((std::string)name,(va_list&)value)

#define DED_GET_ELEMENT(DataEncoderptr, entityname, value) DataEncoderptr->DecodeEntityElement((std::string) entityname,(Elements&)value)
#define DED_GET_TOAST( DataEncoderptr, entityname, _vecElements) DataEncoderptr->DecodeToast((std::string) entityname, (std::vector<Elements> &) _vecElements)

#define DED_PUT_DATA_IN_DECODER(decoder_ptr,pCompressedData,sizeofCompressedData) \
		DED_DECOMPRESS_DATA(pCompressedData,sizeofCompressedData,data_ptr_b,length);\
		std::unique_ptr<CDataEncoder> decoder_ptr;\
		decoder_ptr.reset( new CDataEncoder(data_ptr_b,length) );

#define DED_GET_STRUCT_END(DataEncoderptr, name) DataEncoderptr->DecodeStructEnd((std::string)name)


// Div. functions

#define	DED_STRING_TO___byte(__byteArray,str) memcpy(__byteArray,str.GetBuffer(),sizeof(__byteArray));
#define	DED_BYTE_TO_STRING(str,__byteArray) memcpy(str.GetBuffer(),__byteArray,sizeof(__byteArray));

#define DED_COMPRESS_DATA(data_ptr_c,sizeofdata,compresseddata,sizeofcompresseddata) \
    std::unique_ptr<__byte> m_ptotalCompresseddata;\
	m_ptotalCompresseddata.reset(new unsigned char[sizeofdata*2]);\
	int sizeofcompresseddata;\
	{kext_tools59 Okumura;\
	sizeofcompresseddata = Okumura.compress_lzss(m_ptotalCompresseddata.get(),sizeofdata*2,(unsigned char*)data_ptr_c,sizeofdata);}__byte* compresseddata = m_ptotalCompresseddata.get();

#define	DED_DECOMPRESS_DATA(pCompressedData,sizeofCompressedData,pUnCompressedData,sizeofUncompressedData) \
	std::unique_ptr<__byte> m_pUnCmpressed;\
	int iEstimatedLength=sizeofCompressedData * 8;\
	kext_tools59 Okumura;\
	m_pUnCmpressed.reset(new unsigned char[iEstimatedLength]);\
	ZeroMemory(m_pUnCmpressed.get(),iEstimatedLength);\
	int sizeofUncompressedData = Okumura.decompress_lzss(m_pUnCmpressed.get(),pCompressedData,sizeofCompressedData);\
	__byte* pUnCompressedData = m_pUnCmpressed.get();

