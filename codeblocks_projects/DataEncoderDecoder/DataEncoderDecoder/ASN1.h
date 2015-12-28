/**************************************************************
 ASN1.h -- Abstract Syntax Notation protocol handler
***************************************************************
    3/5/2013 Johnny Serup
    Use, distribute, and modify this program freely.
    Please send me your improved versions.
        johnny.serup@scanva.com
        use "Att.: Improvements to ASN1"

**************************************************************/
//#pragma comment( user, "Compiled on " __DATE__ " at " __TIME__ )
//#pragma message( "Compiling " __FILE__ )
//#pragma message( "Last modified on " __TIMESTAMP__ )
//#pragma once
#include <stdarg.h> // va_list is defined in here and used in winbase.h
//#include <windef.h>
//#include <winbase.h>
#include <memory>
//#include <auto_ptr.h>
//using std::auto_ptr; // auto_ptr class definition
#include <bits/unique_ptr.h>
using std::unique_ptr; // unique_ptr class definition   // with -std=c++11 or -std=gnu++11

#include <string.h>  // memset

#define __byte unsigned char
//#define BYTE unsigned char
//#define NULL 0x0

#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))
#define ZeroMemory RtlZeroMemory

namespace HandleProtocol
{
class CASN1
{
public:
	unique_ptr<__byte> m_pASN1Data;
private:
    int     m_iLengthOfData;		// Length of ASN1 data, copied during construction.
    int     m_iTotalLengthOfData;	// Total length of ASN1 data.
    __byte*   m_pNextASN1;			// Will point at first ASN1 structure, and when FetchNextASN1 is called it will move to (point at) next ASN1 location.
									// The __byte that it points at will be the length __byte of the ASN1 data.
    __byte*   m_pAppendPosition;		// Pointer to end of current ASN1 data
    int     CurrentASN1Position, NextASN1Position;


public:

    CASN1(int iAppendMaxLength=255 )
    {
        try{
        m_pNextASN1=NULL;
        CurrentASN1Position=0;
        int iLength=iAppendMaxLength;
        m_iLengthOfData=0;
        m_iTotalLengthOfData=0;
        if(iLength!=0){
			  m_pASN1Data.reset(new __byte[iLength]);
			  if(m_pASN1Data.get() != NULL){
                m_iTotalLengthOfData=iLength;
				m_pNextASN1=m_pASN1Data.get(); // First ASN1
              }
              else
                 return;
        }
        else
            return;

        }catch(...){ return; }

        return;
    }

    CASN1(int  LengthOfData, __byte* m_pdata, int iAppendMaxLength=0 )
    {
        try{
        m_pNextASN1=NULL;
        CurrentASN1Position=0;
        int iLength=iAppendMaxLength+LengthOfData;
        m_iLengthOfData=LengthOfData;
        m_iTotalLengthOfData=0;
        if(iLength!=0){
			  m_pASN1Data.reset(new __byte[iLength]);
			  if(NULL!=m_pASN1Data.get()){
                ZeroMemory(m_pASN1Data.get(),iLength);
                m_iTotalLengthOfData=iLength;
				m_pNextASN1=m_pASN1Data.get(); // First ASN1
				memcpy(m_pASN1Data.get(),m_pdata,LengthOfData);
              }
              else
                 return;
        }
        else
            return;

        }catch(...){ return; }

        return;
    }

    ~CASN1(void)
    {
    }

    int WhatIsReadSize()
    {
        if(m_iLengthOfData>0)
            return (int)m_iLengthOfData;
        else
            return (int)0;
    }

    int WhatIsWriteSize()
    {
        int iResult=0;
        try{
            iResult=m_iTotalLengthOfData-m_iLengthOfData;
            if(iResult<0) iResult=0;
        }catch(...){ iResult=0; }
        return iResult;
    }




    bool AppendASN1(int LengthOfNewASN1Data, __byte Tag, const char* data )    // Append an ASN1 structure to a data area, fx. the data area of a datapacket.
    {
        bool bResult=true;

		if(m_iTotalLengthOfData<(m_iLengthOfData+LengthOfNewASN1Data)){
            bResult=false; // NO ROOM for new ASN1
		}
        else {
            try{
            m_pAppendPosition = m_pASN1Data.get()+m_iLengthOfData;
            *((int*)m_pAppendPosition) = LengthOfNewASN1Data;
            m_pAppendPosition+=sizeof(LengthOfNewASN1Data);
            *m_pAppendPosition++ = (__byte)Tag;

            memcpy(m_pAppendPosition,(void*)data,LengthOfNewASN1Data);
            m_iLengthOfData=m_iLengthOfData+sizeof(LengthOfNewASN1Data)+1+LengthOfNewASN1Data; // Add new ASN1 to length : Length+tag+SizeofData
            }catch(...){ bResult=false; m_iLengthOfData=m_iTotalLengthOfData; }
        }
        return bResult;
    }

    bool FetchNextASN1(int& Length, __byte& Tag, __byte** m_pdata) // Returns true if ASN1 was found, and false if not.
    {
        bool bResult=true;
        __byte* m_pPrevASN1;
        m_pPrevASN1 = 0;
        try{
            if(NULL!=m_pNextASN1)
            {
                m_pPrevASN1=m_pNextASN1;
                Length  = *(int*)m_pNextASN1;
                m_pNextASN1+=sizeof(Length);
                Tag     = *m_pNextASN1++;

                *m_pdata   = m_pNextASN1;
                NextASN1Position=CurrentASN1Position+Length+1+sizeof(Length);
                if(NextASN1Position>m_iTotalLengthOfData||NextASN1Position<0){
                    m_pNextASN1=NULL;
                    bResult=false; // ASN1 says it is longer than ASN1 allocated space ??? ASN1 has illegal size.
                }
                else {
                    CurrentASN1Position=NextASN1Position;
                    if(CurrentASN1Position>=m_iTotalLengthOfData)
                        m_pNextASN1=NULL;
					else
	                    m_pNextASN1+=Length;
                }
            }
            else
                bResult=false;
        }catch(...){ bResult=false; }
        return bResult;
    }


    bool FetchTotalASN1(int& Length, __byte** m_pdata) // Returns true if ASN1 was found, and false if not.
    {
        bool bResult=true;
        try{
            if(NULL!=m_pASN1Data.get())
            {
                Length   = (int)m_iLengthOfData;
                *m_pdata   = m_pASN1Data.get();
            }
            else
                bResult=false;
        }catch(...){ bResult=false; }
        return bResult;
    }


};


}
using namespace HandleProtocol;
