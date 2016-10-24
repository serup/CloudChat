/**************************************************************
 DataEncoder.cpp -- Encodes / Decodes data
***************************************************************
    3/5/2013 Johnny Serup
    Use, distribute, and modify this program freely.
    Please send me your improved versions.
        johnny.serup@scanva.com
        use "Att.: Improvements to DataEncoder"

**************************************************************/
//#pragma comment( user, "Compiled on " __DATE__ " at " __TIME__ )
//#pragma message( "Compiling " __FILE__ )
//#pragma message( "Last modified on " __TIMESTAMP__ )
//#pragma once

#include "DataEncoder.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <boost/algorithm/string.hpp> // boost::to_upper / boost::to_lower

typedef unsigned short WORD;
#define uint8  unsigned char
#define uint16 unsigned short
#define int32  long
#define ulong  DWORD

CDataEncoder::CDataEncoder(void)
{
	m_pdata=NULL;
	m_ptotaldata=NULL;
	m_iLengthOfTotalData=0;

}

CDataEncoder::~CDataEncoder(void)
{
/*	CHandleSVG* ptr;
	POSITION oldpos;
	POSITION pos = svgObjectsForDeletion.GetHeadPosition();
	while(pos)
	{
		oldpos = pos;
		ptr = (CHandleSVG*)svgObjectsForDeletion.GetNext(pos);
		if(NULL!=ptr) delete ptr;
		svgObjectsForDeletion.RemoveAt(oldpos);
	}

	pos = va_list_ObjectsForDeletion.GetHeadPosition();
	while(pos)
	{
		oldpos = pos;
		va_list marker = (va_list)va_list_ObjectsForDeletion.GetNext(pos);
		if(NULL!=marker) va_end( marker );
		va_list_ObjectsForDeletion.RemoveAt(oldpos);
	}
*/
}

CDataEncoder::CDataEncoder(unsigned char* data_ptr, int iLengthOfdata)
{

	CASN1 asn1((DWORD)iLengthOfdata,(__byte*)data_ptr,iLengthOfdata+1);

    m_iLengthOfTotalData=0;
    asn1.FetchTotalASN1(m_iLengthOfTotalData,&m_pdata);

	if(m_iLengthOfTotalData>0)
	{
		pasn1data.reset( new __byte[m_iLengthOfTotalData+1] );
		ZeroMemory(pasn1data.get(),m_iLengthOfTotalData+1);
		memcpy(pasn1data.get(),(void*)m_pdata,m_iLengthOfTotalData);
		m_ptotaldata = pasn1data.get();
	}

}



void CDataEncoder::EncodeStructStart(std::string name)
{
	AddElement(name,DED_ELEMENT_TYPE_STRUCT);
}


void CDataEncoder::EncodeMethod(std::string method, std::string value)
{
	AddElement(method,DED_ELEMENT_TYPE_METHOD, value);
}


void CDataEncoder::Encodestdstring(std::string method, std::string value)
{
	AddElement(method,DED_ELEMENT_TYPE_STDSTRING, value);
}

void CDataEncoder::Encodestdvector(std::string method, std::vector<unsigned char> value)
{
	// avoid possible whitespace issue 0x20 -- compression can NOT compress if more than two contiguous 0x20 occurr in data - why ?? TODO: find a better solution than this ugly fix of adding 1 and subtracting 1
	for(int n=0;n<value.size();n++) value[n] = value[n] + 1;
	AddElement(method,DED_ELEMENT_TYPE_STDVECTOR, value);
}

void CDataEncoder::EncodeUShort(std::string name, unsigned short value)
{
	AddElement(name,DED_ELEMENT_TYPE_USHORT, value);
}


void CDataEncoder::EncodeBool(std::string name, bool value)
{
	AddElement(name,DED_ELEMENT_TYPE_BOOL, value);
}

void CDataEncoder::EncodeChar(std::string name, char value)
{
	AddElement(name,DED_ELEMENT_TYPE_CHAR, value);
}
void CDataEncoder::EncodeByte(std::string name, __byte value)
{
	AddElement(name,DED_ELEMENT_TYPE_BYTE, value);
}
void CDataEncoder::EncodeLong(std::string name, long value)
{
	AddElement(name,DED_ELEMENT_TYPE_LONG, value);
}
void CDataEncoder::EncodeULong(std::string name, ulong value)
{
	AddElement(name,DED_ELEMENT_TYPE_ULONG, value);
}

void CDataEncoder::EncodeFloat(std::string name, float value)
{
	AddElement(name,DED_ELEMENT_TYPE_FLOAT, value);
}


void CDataEncoder::EncodeCBundleObject(std::string name, CBundleObject* value)
{
	AddElement(name,DED_ELEMENT_TYPE_COBJECT, value);
}
void CDataEncoder::EncodeClassInstance(std::string name, void* value, int sizeofclass)
{
	AddElement(name,DED_ELEMENT_TYPE_CLASS, value, sizeofclass);
}

void CDataEncoder::EncodeElement(std::string entityname, std::string elementname, std::vector<unsigned char> elementvalue)
{
    struct _Elements
    {
        std::string strElementID;
        std::vector<unsigned char> ElementData;
    };

    _Elements element;
    element.strElementID = elementname;
    std::copy(elementvalue.begin(), elementvalue.end(), std::back_inserter(element.ElementData));


    std::string strentity_chunk_id    = boost::to_lower_copy(entityname) + "_chunk_id";
    std::string strentity_chunk_data  = boost::to_lower_copy(entityname) + "_chunk_data";

    Encodestdstring( strentity_chunk_id, (std::string)element.strElementID ); // key of particular item
    Encodestdvector( strentity_chunk_data, element.ElementData ); //
}

/*
void CDataEncoder::EncodeSVGfile(std::string name, std::string filename)
{
	// Fetch file
	struct __stat64 buf;
	int fd = _open(filename, _O_RDONLY);
	_fstat64(fd,&buf);
	int sizeoffile=(int)buf.st_size;
	_close(fd);
	if(buf.st_size > 0)
	{
		char* __byteArray = new char[sizeoffile];
		try{
			DWORD dw__bytesWritten = 0;
			HFILE hFile;
			OFSTRUCT ReOpenBuf;
			hFile = OpenFile(filename,&ReOpenBuf,OF_READ);
			ReadFile((HANDLE)hFile,(LPVOID)__byteArray,sizeoffile,&dw__bytesWritten,NULL);
			CloseHandle((HANDLE)hFile);
			Add(name, DED_ELEMENT_TYPE_SVGFILE, (__byte*)__byteArray,sizeoffile);
		}catch(...){}
		delete __byteArray;
	}
}
*/

void CDataEncoder::EncodeVA_LIST(std::string name, va_list value, int size)
{
	AddElement(name, DED_ELEMENT_TYPE_VA_LIST, value, size);
}

//...

void CDataEncoder::EncodeStructEnd(std::string name)
{
	AddElement(name,DED_ELEMENT_TYPE_STRUCT_END);
}


__byte* CDataEncoder::DataEncoder_GetData(int& iLengthOfTotalData)
{
	iLengthOfTotalData = m_iLengthOfTotalData;
	return m_ptotaldata;
}




//+ AddElement
void CDataEncoder::AddElement(std::string name, int ElementType)
{
	if(ElementType == DED_ELEMENT_TYPE_STRUCT)
	{ // First element in structure
		CASN1 asn1(name.length()+sizeof(int)+sizeof(__byte));

        int LengthOfAsn1= name.length();
        asn1.AppendASN1((DWORD)LengthOfAsn1,ElementType,name.data());

        m_iLengthOfData=0;
		asn1.FetchTotalASN1(m_iLengthOfData,&m_pdata);
		pasn1data.reset( new __byte[m_iLengthOfData+1] );
		ZeroMemory(pasn1data.get(),m_iLengthOfData+1);
		memcpy(pasn1data.get(),(void*)m_pdata,m_iLengthOfData);
		m_ptotaldata = pasn1data.get();
		m_iLengthOfTotalData = m_iLengthOfData;
	}
	else
	{

		try{
//		//ASSERT(pasn1data.get() != NULL);
		CASN1 asn1((DWORD)m_iLengthOfTotalData,
					(__byte*)pasn1data.get(),
					m_iLengthOfTotalData+name.length()+1);

		int LengthOfAsn1 = name.length();
//		VERIFY(asn1.appendASN1((DWORD)LengthOfAsn1,ElementType,(unsigned char*)&*name)==true);
//		VERIFY(asn1.appendASN1((DWORD)LengthOfAsn1,ElementType,name.data())==true);
		asn1.AppendASN1((DWORD)LengthOfAsn1,ElementType,name.data())==true;

		m_iLengthOfTotalData=0;
		asn1.FetchTotalASN1(m_iLengthOfTotalData,&m_pdata);

		if(m_iLengthOfTotalData>0)
		{
			pasn1data.reset( new __byte[m_iLengthOfTotalData+1] );
			ZeroMemory(pasn1data.get(),m_iLengthOfTotalData+1);
			memcpy(pasn1data.get(),(void*)m_pdata,m_iLengthOfTotalData);
			m_ptotaldata = pasn1data.get();
		}
		}catch(...){printf("\nFAIL DURING ADD TO ASN1 DATA AREA\n");}

	}

}


void CDataEncoder::AddElement(std::string name, int ElementType, std::string value)
{
	Add(name,ElementType,value.data(),value.length());
}

void CDataEncoder::AddElement(std::string name, int ElementType, std::vector<unsigned char> value)
{
	Add(name,ElementType,(const char*)(__byte*)&value[0],value.size());
}

void CDataEncoder::AddElement(std::string name, int ElementType, unsigned short value)
{
	Add(name, ElementType, (const char*)(__byte*)&value,sizeof(value));
}


void CDataEncoder::AddElement(std::string name, int ElementType, bool value)
{
	Add(name, ElementType, (const char*)(__byte*)&value,sizeof(value));
}

void CDataEncoder::AddElement(std::string name, int ElementType, char value)
{
	Add(name, ElementType, (const char*)(__byte*)&value,sizeof(value));
}

void CDataEncoder::AddElement(std::string name, int ElementType, __byte value)
{
	Add(name, ElementType, (const char*)(__byte*)&value,sizeof(value));
}

void CDataEncoder::AddElement(std::string name, int ElementType, long value)
{
    typedef struct
    {
	    union
		{
		 long LONG;
		 unsigned char ___byte[4];
		}_Long;
    }Buffer;

    Buffer tmp;

    tmp._Long.LONG = value;

	Add(name, ElementType, (const char*)tmp._Long.___byte,sizeof(value));
}

void CDataEncoder::AddElement(std::string name, int ElementType, ulong value)
{
    typedef struct
    {
	    union
		{
		 unsigned long __LONG;
		 unsigned char ____byte[4];
		}_Long;
    }Buffer;

    Buffer tmp;

    tmp._Long.__LONG = value;

	Add(name, ElementType, (const char*)tmp._Long.____byte,sizeof(value));
}

void CDataEncoder::AddElement(std::string name, int ElementType, float value)
{
    typedef struct
    {
	    union
		{
		 float FLOAT;
		 unsigned char ___byte[4];
		}_Float;
    }Buffer;

    Buffer tmp;

    tmp._Float.FLOAT = value;

	Add(name, ElementType, (const char*)tmp._Float.___byte,sizeof(value));
}


void CDataEncoder::AddElement(std::string name, int ElementType, CBundleObject* value)
{
	Add(name, ElementType, (const char*)(__byte*)value,sizeof(CBundleObject));
}

void CDataEncoder::AddElement(std::string name, int ElementType, void* value, int sizeofclass)
{
	Add(name, ElementType, (const char*)(__byte*)value,sizeofclass);
}

void CDataEncoder::AddElement(std::string name, int ElementType, va_list value, int size)
{
	Add(name, ElementType, (const char*)(__byte*)value,size);
}


void CDataEncoder::AddElement(std::string name, int ElementType,struct stat value)
{
	Add(name, ElementType, (const char*)(__byte*)&value,sizeof(struct stat));
}



//////////////////////////////////////////////
// GENERAL ADD ELEMENTS INTO ASN1 STRUCTURE //
//////////////////////////////////////////////
void CDataEncoder::Add(std::string name, int ElementType, const char* value,int iLength)
{
	try{
//	//ASSERT(pasn1data.get() != NULL);
	CASN1 asn1((DWORD)m_iLengthOfTotalData,
				(__byte*)pasn1data.get(),
				m_iLengthOfTotalData+name.length()+
				iLength+1);

    int LengthOfAsn1 = name.length();
//    VERIFY(asn1.appendASN1((DWORD)LengthOfAsn1,ElementType,(unsigned char*)&*name)==true);
    asn1.AppendASN1((DWORD)LengthOfAsn1,ElementType,name.data())==true;

	LengthOfAsn1 = iLength;
//	//ASSERT(LengthOfAsn1>0);
//    VERIFY(asn1.appendASN1((DWORD)LengthOfAsn1,ElementType,value));
    asn1.AppendASN1((DWORD)LengthOfAsn1,ElementType,value);

    m_iLengthOfTotalData=0;
    asn1.FetchTotalASN1(m_iLengthOfTotalData,&m_pdata);

	if(m_iLengthOfTotalData>0)
	{
		pasn1data.reset( new __byte[m_iLengthOfTotalData+1] );
		ZeroMemory(pasn1data.get(),m_iLengthOfTotalData+1);
		memcpy(pasn1data.get(),(void*)m_pdata,m_iLengthOfTotalData);
		m_ptotaldata = pasn1data.get();
	}
	}catch(...){printf("\nFAIL DURING ADD TO ASN1 DATA AREA\n");}
}
//-


bool CDataEncoder::DecodeStructStart(std::string name)
{
	return GetElement(name,DED_ELEMENT_TYPE_STRUCT);
}
bool CDataEncoder::DecodeMethod(std::string name, std::string& value)
{
	return GetElement(name,DED_ELEMENT_TYPE_METHOD, value);
}
bool CDataEncoder::DecodeUShort(std::string name,uint16& value)
{
	return GetElement(name,DED_ELEMENT_TYPE_USHORT, value);
}
bool CDataEncoder::DecodeBool(std::string name,bool& value)
{
	return GetElement(name,DED_ELEMENT_TYPE_BOOL, value);
}
bool CDataEncoder::DecodeChar(std::string name,char& value)
{
	return GetElement(name,DED_ELEMENT_TYPE_CHAR, value);
}
bool CDataEncoder::Decode__byte(std::string name,__byte& value)
{
	return GetElement(name,DED_ELEMENT_TYPE_BYTE, value);
}
bool CDataEncoder::DecodeLong(std::string name,long& value)
{
	return GetElement(name,DED_ELEMENT_TYPE_LONG, value);
}
bool CDataEncoder::DecodeULong(std::string name,ulong& value)
{
	return GetElement(name,DED_ELEMENT_TYPE_ULONG, value);
}
bool CDataEncoder::DecodeFloat(std::string name,float& value)
{
	return GetElement(name,DED_ELEMENT_TYPE_FLOAT, value);
}
bool CDataEncoder::DecodeCBundleObject(std::string name,CBundleObject& value)
{
	return GetElement(name,DED_ELEMENT_TYPE_COBJECT, (__byte**)&value);
}

bool CDataEncoder::Decodestdstring(std::string name, std::string& value)
{
	return GetElement(name,DED_ELEMENT_TYPE_STDSTRING, value);
}

bool CDataEncoder::Decodestdvector(std::string name, std::vector<unsigned char>& value)
{
	return GetElement(name,DED_ELEMENT_TYPE_STDVECTOR, value);
}
//bool CDataEncoder::DecodeClassInstance(std::string name,CObject& value)
//{
//	return GetElement(name,DED_ELEMENT_TYPE_CLASS, (__byte**)&value);
//}

/*
bool CDataEncoder::DecodeSVGfile(std::string name,__byte** value)
{
	std::string strSVGfile = "AD402D95.svg";
	__byte* __byteArray;
	bool  bResult=false;

	try{
	DWORD length=0;
	GetElement(name,DED_ELEMENT_TYPE_SVGFILE, &__byteArray,length);

	DWORD dw__bytesWritten = 0;
	HFILE hFile;
	OFSTRUCT ReOpenBuf;
	hFile = OpenFile(strSVGfile,&ReOpenBuf,OF_CREATE);
	WriteFile((HANDLE)hFile,(LPCVOID)__byteArray,length,&dw__bytesWritten,NULL);
	CloseHandle((HANDLE)hFile);

	CHandleSVG* pSVGapp = (CHandleSVG*)*value;

	FILE* fd;
	fd = fopen(strSVGfile, "r");
	if(NULL!=fd)
	{
		fclose(fd);
		pSVGapp->parse_svg(strSVGfile);
//		pSVGapp->init(m_pContentPictureHolder->GetSafeHwnd(),rcClient.Width(), rcClient.Height(), agg::window_resize );
	}
	bResult=true;

	}catch(...){TRACE0("\nERROR IN TRANSFER OF SVG\n");}

	remove(strSVGfile);
	return bResult;
}
*/

bool CDataEncoder::DecodeVA_LIST(std::string name, va_list& value)
{
	bool bResult=false;
	bResult = GetElement(name,DED_ELEMENT_TYPE_VA_LIST, (__byte**)&value);
//	if(bResult){
//		va_list_ObjectsForDeletion.AddTail(value);
//	}
	return bResult;
}


bool CDataEncoder::DecodeEntityElement(std::string entityname, Elements &element)
{
    bool bResult=false;
    //_Elements element;

    std::string strentity_chunk_id    = boost::to_lower_copy(entityname) + "_chunk_id";
    std::string strentity_chunk_data  = boost::to_lower_copy(entityname) + "_chunk_data";

    bResult = Decodestdstring( strentity_chunk_id, element.strElementID ); // key of particular item
    if(bResult==true) {
        bResult=false;
        bResult = Decodestdvector( strentity_chunk_data, element.ElementData ); //
    }
    return bResult;
}

bool CDataEncoder::DecodeToast(std::string entityname, std::vector<Elements> &_vecElements)
{
    bool bResult=false;
    std::string strStartToast = "";

    if(Decodestdstring( "STARTtoast", strStartToast ))
    {/// Toast area exists now put elements in structure
        Elements Element;
        bool bStillVectors=true;
        while(bStillVectors)
        {
            if(DecodeEntityElement(entityname, Element)) // TODO: FIX PROBLEM WITH LAST ELEMENT - EITHER ROLLBACK POINTER OR MOVE ONE POINTER AHEAD
            {
                _vecElements.push_back(Element);
                bResult=true;
            }
            else{
             // expect that toast area ends with "ENDtoast" "elements"
                // TEMPORARY FIX FOR ABOVE ISSUE WITH DecodeEntityElement !!!!!!!
                std::string strtmp = "";
                if(Decodestdstring( "alsdfjjsfwer", strtmp ) == false) // ignore "elements"  -- ONE STEP IN ASN1 WILL BE TAKEN AND TWO IF FIRST ITEM IS FOUND
                    std::cout << "[CDataEncoder::DecodeToast] : WARNING ENDtoast ignoring : elements  - this is a temporary fix -- please make proper fix" << std::endl;
                bStillVectors=false;
            }
        }
    }
    return bResult;
}
//TODO: Add more elementtypes here...

bool CDataEncoder::DecodeStructEnd(std::string name)
{
	return GetElement(name,DED_ELEMENT_TYPE_STRUCT_END);
}


//+ GetElement
bool CDataEncoder::GetElement(std::string name, int ElementType)
{
	bool bResult = false;
	if(ElementType == DED_ELEMENT_TYPE_STRUCT)
	{ // First element in structure
		//ASSERT(pasn1data.get() != NULL);
		pasn1FetchData.reset( new CASN1((DWORD)m_iLengthOfTotalData,(__byte*)pasn1data.get(),m_iLengthOfTotalData) );

		CASN1* pasn1 = pasn1FetchData.get();

		int length=0;
		__byte tag=0;
		__byte* pdata=NULL;
		if(pasn1->FetchNextASN1(length,tag,&pdata))
		{
			if(tag == DED_ELEMENT_TYPE_STRUCT)
			{
				std::string str;
				str.append((const char*)pdata,length);
				if(name == str)
					bResult = true;
			}
		}

	}

	if(ElementType == DED_ELEMENT_TYPE_STRUCT_END)
	{ // Last element in structure
		//ASSERT(pasn1FetchData.get() != NULL);
		CASN1* pasn1 = pasn1FetchData.get();

		int length=0;
		__byte tag=0;
		__byte* pdata=NULL;
		if(pasn1->FetchNextASN1(length,tag,&pdata))
		{
			if(tag == DED_ELEMENT_TYPE_STRUCT_END)
			{
				std::string str;
				str.append((const char*)pdata,length);
				if(name == str)
					bResult = true;
			}
		}

	}

	return bResult;
}

bool CDataEncoder::GetElement(std::string name, int ElementType, std::string& value)
{
	bool bResult = false;

	switch(ElementType)
	{
		case DED_ELEMENT_TYPE_METHOD:
		{
			//ASSERT(pasn1FetchData.get() != NULL);
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			__byte* pdata=NULL;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == DED_ELEMENT_TYPE_METHOD)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,&pdata))
						{
							if(tag == DED_ELEMENT_TYPE_METHOD)
							{
								str.append((const char*)pdata,length);
								value = str;
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;

		case DED_ELEMENT_TYPE_STDSTRING:
		{
			//ASSERT(pasn1FetchData.get() != NULL);
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			__byte* pdata=NULL;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == DED_ELEMENT_TYPE_STDSTRING)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,&pdata))
						{
							if(tag == DED_ELEMENT_TYPE_STDSTRING)
							{
								str.append((const char*)pdata,length);
								value = str;
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;
	}


	return bResult;
}

bool CDataEncoder::GetElement(std::string name, int ElementType, std::vector<unsigned char>& value)
{
	bool bResult = false;

	switch(ElementType)
	{
		case DED_ELEMENT_TYPE_STDVECTOR:
			{
				CASN1* pasn1 = pasn1FetchData.get();

				int length=0;
				__byte tag=0;
				__byte* pdata=NULL;
				if(pasn1->FetchNextASN1(length,tag,&pdata))
				{
					if(tag == DED_ELEMENT_TYPE_STDVECTOR)
					{
						std::string str;
						str.append((const char*)pdata,length);
						if(name == str)
						{
							length=0; tag=0; pdata = NULL; str="";
							if(pasn1->FetchNextASN1(length,tag,&pdata))
							{
								if(tag == DED_ELEMENT_TYPE_STDVECTOR)
								{
									value.clear();
									std::vector<unsigned char> vec(pdata, pdata+length);
									//redo whitespace issue 0x20	
									for(int n=0;n<vec.size();n++) vec[n] = vec[n] - 1;
									std::copy(vec.begin(), vec.end(), std::back_inserter(value));
									bResult = true;
								}
							}
						}
					}
				}
			}
			break;
	}

	return bResult;
}

bool CDataEncoder::Get(std::string name, int ElementType,__byte* pdata,WORD& length)
{
	bool bResult = false;
	switch(ElementType)
	{
		case DED_ELEMENT_TYPE_METHOD:
		{
			//ASSERT(pasn1FetchData.get() != NULL);
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == ElementType)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,&pdata))
						{
							if(tag == ElementType)
							{
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;

	}
	return bResult;
}

union CharToStruct {
    char charArray[2];
    unsigned short value;
};

short toShort(char* value){
    CharToStruct cs;
    cs.charArray[0] = value[1]; // most significant bit of short is not first bit of char array
    cs.charArray[1] = value[0];
    return cs.value;
}

bool CDataEncoder::GetElement(std::string name, int ElementType, uint16& value)
{
	bool bResult=false;

	switch(ElementType)
	{
		case DED_ELEMENT_TYPE_USHORT:
		{
			//ASSERT(pasn1FetchData.get() != NULL);
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			__byte* pdata=NULL;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == DED_ELEMENT_TYPE_USHORT)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,&pdata))
						{
							if(tag == DED_ELEMENT_TYPE_USHORT)
							{
								value = (uint16)*pdata; // MUST be in little Endian format
								//BigEndian to little Endian
								//unsigned short p = (pdata[0] << 8) | pdata[1];
								//unsigned short p = (((unsigned short)pdata[0])<<8) | pdata[1];
								//unsigned short p = toShort((char*)pdata);
								//value = p;
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;

	}

	return bResult;
}
bool CDataEncoder::GetElement(std::string name, int ElementType, bool& value)
{
	bool bResult=false;

	switch(ElementType)
	{
		case DED_ELEMENT_TYPE_BOOL:
		{
			//ASSERT(pasn1FetchData.get() != NULL); // NB! check that typecast of parameter is correct in the macro !!!
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			__byte* pdata=NULL;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == DED_ELEMENT_TYPE_BOOL)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,&pdata))
						{
							if(tag == DED_ELEMENT_TYPE_BOOL)
							{
								value = (bool)*pdata;
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;

	}

	return bResult;
}
bool CDataEncoder::GetElement(std::string name, int ElementType, char& value)
{
	bool bResult=false;

	switch(ElementType)
	{
		case DED_ELEMENT_TYPE_CHAR:
		{
			//ASSERT(pasn1FetchData.get() != NULL);
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			__byte* pdata=NULL;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == DED_ELEMENT_TYPE_CHAR)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,&pdata))
						{
							if(tag == DED_ELEMENT_TYPE_CHAR)
							{
								value = (char)*pdata;
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;

	}

	return bResult;
}
bool CDataEncoder::GetElement(std::string name, int ElementType, __byte& value)
{
	bool bResult=false;
	switch(ElementType)
	{
		case DED_ELEMENT_TYPE_BYTE:
		{
			//ASSERT(pasn1FetchData.get() != NULL);
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			__byte* pdata=NULL;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == DED_ELEMENT_TYPE_BYTE)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,&pdata))
						{
							if(tag == DED_ELEMENT_TYPE_BYTE)
							{
								value = (__byte)*pdata;
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;
	}


	return bResult;
}

bool CDataEncoder::GetElement(std::string name, int ElementType, long& value)
{
	bool bResult=false;

	switch(ElementType)
	{
		case DED_ELEMENT_TYPE_LONG:
		{
			//ASSERT(pasn1FetchData.get() != NULL);
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			__byte* pdata=NULL;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == DED_ELEMENT_TYPE_LONG)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,&pdata))
						{
							if(tag == DED_ELEMENT_TYPE_LONG)
							{

								typedef struct
								{
									union
									{
									long __LONG;
									unsigned char ____byte[4];
									}_Long;
								}Buffer;

								Buffer tmp;

								tmp._Long.__LONG = 0; // reset.
								__byte* pb = pdata;
								for(int n=0;n<4;n++)  tmp._Long.____byte[n] = *pb++;

								value = tmp._Long.__LONG;
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;

	}

	return bResult;
}

bool CDataEncoder::GetElement(std::string name, int ElementType, ulong& value)
{
	bool bResult=false;

	switch(ElementType)
	{
		case DED_ELEMENT_TYPE_ULONG:
		{
			//ASSERT(pasn1FetchData.get() != NULL);
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			__byte* pdata=NULL;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == DED_ELEMENT_TYPE_ULONG)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,&pdata))
						{
							if(tag == DED_ELEMENT_TYPE_ULONG)
							{
								typedef struct
								{
									union
									{
									unsigned long __LONG;
									unsigned char ____byte[4];
									}_Long;
								}Buffer;

								Buffer tmp;

								tmp._Long.__LONG = 0; // reset.
								__byte* pb = pdata;
								for(int n=0;n<4;n++)  tmp._Long.____byte[n] = *pb++;

								value = tmp._Long.__LONG;
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;

	}

	return bResult;
}
bool CDataEncoder::GetElement(std::string name, int ElementType, float& value)
{
	bool bResult=false;

	switch(ElementType)
	{
		case DED_ELEMENT_TYPE_FLOAT:
		{
			//ASSERT(pasn1FetchData.get() != NULL);
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			__byte* pdata=NULL;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == DED_ELEMENT_TYPE_FLOAT)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,&pdata))
						{
							if(tag == DED_ELEMENT_TYPE_FLOAT)
							{
								typedef struct
								{
									union
									{
									float FLOAT;
									unsigned char ___byte[4];
									}_Float;
								}Buffer;

								Buffer tmp;

								tmp._Float.FLOAT = 0; // reset.
								__byte* pb = pdata;
								for(int n=0;n<4;n++)  tmp._Float.___byte[n] = *pb++;

								value = tmp._Float.FLOAT;
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;

	}

	return bResult;
}
bool CDataEncoder::GetElement(std::string name, int ElementType, __byte** value)
{
	bool bResult=false;

	switch(ElementType)
	{
		case DED_ELEMENT_TYPE_COBJECT:
		{
			//ASSERT(pasn1FetchData.get() != NULL);
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			__byte* pdata=NULL;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == DED_ELEMENT_TYPE_COBJECT)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,&pdata))
						{
							if(tag == DED_ELEMENT_TYPE_COBJECT)
							{
								CBundleObject *x;
								x = new (pdata) CBundleObject();
								*value = (__byte*)x;

								//*value = (__byte*)malloc(sizeof(CBundleObject));
								//memcpy(*value,pdata,sizeof(CBundleObject));

								//TODO: This looks to good to be true :-)
								//and it is too good to be true, it is not possible to transfer an instantiated class (object) as simple as this
								//simply because of the design of C++ compilers/linkers
								//if you transfer this way between different architecture you run into big/little endian problems, plus
								//structure padding differences in compiler environtments !!!
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;

		case DED_ELEMENT_TYPE_CLASS:
		{
			//ASSERT(pasn1FetchData.get() != NULL);
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			__byte* pdata=NULL;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == DED_ELEMENT_TYPE_CLASS)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,value))
						{
							if(tag == DED_ELEMENT_TYPE_CLASS)
							{
								//TODO: This looks to good to be true :-)
								//and it is too good to be true, it is not possible to transfer an instantiated class (object) as simple as this
								//simply because of the design of C++ compilers/linkers
								//if you transfer this way between different architecture you run into big/little endian problems, plus
								//structure padding differences in compiler environtments !!!
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;

		case DED_ELEMENT_TYPE_SVGFILE:
		{
			//ASSERT(pasn1FetchData.get() != NULL);
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			__byte* pdata=NULL;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == DED_ELEMENT_TYPE_SVGFILE)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,value))
						{
							if(tag == DED_ELEMENT_TYPE_SVGFILE)
							{
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;

		case DED_ELEMENT_TYPE_VA_LIST:
		{
			//ASSERT(pasn1FetchData.get() != NULL);
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			__byte* pdata=NULL;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == DED_ELEMENT_TYPE_VA_LIST)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,value))
						{
							if(tag == DED_ELEMENT_TYPE_VA_LIST)
							{
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;


	}

	return bResult;
}

bool CDataEncoder::GetElement(std::string name, int ElementType, __byte** value,DWORD& filelength )
{
	bool bResult=false;

	switch(ElementType)
	{
		case DED_ELEMENT_TYPE_SVGFILE:
		{
			//ASSERT(pasn1FetchData.get() != NULL);
			CASN1* pasn1 = pasn1FetchData.get();

			int length=0;
			__byte tag=0;
			__byte* pdata=NULL;
			if(pasn1->FetchNextASN1(length,tag,&pdata))
			{
				if(tag == DED_ELEMENT_TYPE_SVGFILE)
				{
					std::string str;
					str.append((const char*)pdata,length);
					if(name == str)
					{
						length=0; tag=0; pdata = NULL; str="";
						if(pasn1->FetchNextASN1(length,tag,value))
						{
							if(tag == DED_ELEMENT_TYPE_SVGFILE)
							{
								filelength = length;
								bResult = true;
							}
						}
					}
				}
			}
		}
		break;
	}

	return bResult;
}

