/*
 * Copyright 2014-2016 SCANVA ApS
 *
 * This Decoder follows same structure as the Decoder for STOMP, however it is NOT the same
 * - DED is a completely different protocol
 */
package dops.protocol.ded;

import dops.protocol.compression.LZSS;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.Charset;

/**
 * Decodes one or more DED frames contained in a {@link ByteBuffer}.
 *
 * <p>An attempt is made to read all complete DED frames from the buffer, which
 * could be zero, one, or more.
 * The caller is responsible for dealing with incomplete content buffering until there is more input available.
 *
 * example:
 *
 *   short trans_id = 1;
 *   boolean action = true;
 *
 *   DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
 *   DED.PUT_STRUCT_START( DED, "event" );
 *      DED.PUT_METHOD  ( DED, "name",  "MusicPlayer" );
 *      DED.PUT_USHORT  ( DED, "trans_id",  trans_id);
 *      DED.PUT_BOOL    ( DED, "startstop", action );
 *   DED.PUT_STRUCT_END( DED, "event" );
 *   DED.GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);
 *
 *   //..transmitting :-) simulation
 *
 *   //..receiving :-) simulation
 *
 *   DEDDecoder DED = DEDDecoder.DED_START_DECODER();
 *   DED.PUT_DATA_IN_DECODER(decoder_ptr,pCompressedData,sizeofCompressedData);
 *
 *   boolean bDecoded=false;
 *   String strValue;
 *   short iValue;
 *   boolean bValue;
 *
 *   // decode data ...
 *
 *   if( DED.GET_STRUCT_START( DED, "event" ) &&
 *          DED.GET_METHOD ( DED, "name", strValue ) &&
 *          DED.GET_USHORT ( DED, "trans_id", iValue) &&
 *          DED.GET_BOOL   ( DED, "startstop", bValue ) &&
 *       DED.GET_STRUCT_END( DED, "event" ))
 *   {
 *     bDecoded=true;
 *   }
 *   else
 *   {
 *     bDecoded=false;
 *   }
 *
 * @author Johnny Serup
 *  */
public class DEDDecoder {

	public class ByteUtils {

		public long bytesToLong(byte[] bytes) {
			//
			byte[] dest = new byte[8]; // Make sure that buffer has 8 bytes for 64bit long -- NB! long in C++ is normally 4 bytes, so DED could have long values with only 4 bytes
			try {
				System.arraycopy(bytes,0,dest, 0,bytes.length);
			} catch (Exception e) {
				e.printStackTrace();
			}

			ByteBuffer buffer = ByteBuffer.wrap(dest);
			buffer.order(ByteOrder.LITTLE_ENDIAN);  // coming from server which, when running on linux, is a Little Endian architecture

			long v=0;
			while( buffer.hasRemaining()){
				v = buffer.getLong(); // Reads the next eight bytes at this buffer's current position
			}
			return v;
		}

		public short bytesToShort(byte[] bytes) {
			ByteBuffer buffer = ByteBuffer.wrap(bytes);
			buffer.order(ByteOrder.LITTLE_ENDIAN); // coming from server which, when running on linux, is a Little Endian architecture
			short v=0;
			while( buffer.hasRemaining()) {
				v = buffer.getShort();
			}
			return v;
		}
	}

	public ByteUtils byteUtils;
    DEDDecoder decoder_ptr;

	public DEDDecoder()
	{
		byteUtils = new ByteUtils();
	}

	/**
	 * Element types
	 */
	public static final int DED_ELEMENT_TYPE_BOOL = 0;
	public static final int DED_ELEMENT_TYPE_CHAR = 1;
	public static final int DED_ELEMENT_TYPE_BYTE = 2;
	//public static final int DED_ELEMENT_TYPE_WCHAR =  3;
	//public static final int DED_ELEMENT_TYPE_SHORT =  4;
	public static final int DED_ELEMENT_TYPE_USHORT = 5;
	public static final int DED_ELEMENT_TYPE_LONG = 6;
	public static final int DED_ELEMENT_TYPE_ULONG = 7;
	public static final int DED_ELEMENT_TYPE_FLOAT = 8;
	//public static final int DED_ELEMENT_TYPE_DOUBLE = 9;
	public static final int DED_ELEMENT_TYPE_STRING = 10;
	//public static final int DED_ELEMENT_TYPE_WSTRING = 11;
	//public static final int DED_ELEMENT_TYPE_URI     = 12;
	public static final int DED_ELEMENT_TYPE_METHOD = 13;
	public static final int DED_ELEMENT_TYPE_STRUCT = 14;
	//public static int DED_ELEMENT_TYPE_LIST   = 15
	//public static int DED_ELEMENT_TYPE_ARRAY  = 16;
	public static final int DED_ELEMENT_TYPE_ZERO = 17;
	public static final int DED_ELEMENT_TYPE_CLASS = 18;
	public static final int DED_ELEMENT_TYPE_STDSTRING = 19;
	public static final int DED_ELEMENT_TYPE_SVGFILE = 20;
	public static final int DED_ELEMENT_TYPE_VA_LIST = 21;
	//public static final int DED_ELEMENT_TYPE_FILE    = 22;
	public static final int DED_ELEMENT_TYPE_STDVECTOR = 23;
	public static final int DED_ELEMENT_TYPE_STRUCT_END = 24;
	//...
	public static final int DED_ELEMENT_TYPE_UNKNOWN = 0xff;

    class _DEDobject
	{
       DEDEncoder encoder_ptr;
       byte[] uncompresseddata;
       long iLengthOfTotalData;
       byte[] pCompressedData;
       long sizeofCompressedData;
    }

	class param
	{
		public String name;
		public int ElementType;
		public byte[] value;
		public int length;
	}

	public class _Elements
	{
		public String strElementID;
		public byte[] ElementData;
	}

	class data
	{
		int Length;
		byte[] data;
	}



	/**
	 * ASN1 - limited version
	 * sequence of abstract syntax notation
	 * tag-byte,length-byte,data,tag-byte,length-byte,data,....
	 *
	 *
	 */
	class CASN1
	{
		byte[] ASN1Data;		// will be allocated an contain the data being processed
		int iLengthOfData;  	// Length of ASN1 data, copied during construction.
		int iTotalLengthOfData; // Total length of ASN1 data.
		int pNextASN1;			// Will point at first ASN1 structure, and when FetchNextASN1 is called it will move to (point at) next ASN1 location.
		int pAppendPosition;	// Pointer to end of current ASN1 data
		int CurrentASN1Position;
		int NextASN1Position;

		public int CASN1p1(int iAppendMaxLength)
		{
			int result = -1;
			if(iAppendMaxLength > 0)
			{
				pNextASN1 = -1;
				CurrentASN1Position = 0;
				iLengthOfData = 0;
				ASN1Data = new byte[iAppendMaxLength];
				if(ASN1Data.length == iAppendMaxLength)
				{
					iTotalLengthOfData = iAppendMaxLength;
					pNextASN1 = 0; // First ASN1
					pAppendPosition = 0;
					for(int i=0; i < iAppendMaxLength; i++)
					{
						ASN1Data[i] = 0;
					}
					result=1;
				}
			}
			return result;
		}

		public int CASN1p3(int LengthOfData,byte[] data, int iAppendMaxLength)
		{
			int result = -1;
			pNextASN1 = -1;
			CurrentASN1Position = 0;
			int iLength = iAppendMaxLength + LengthOfData;
			iLengthOfData = LengthOfData;
			iTotalLengthOfData = iLength; // max room for data
			if(iLength != 0)
			{
				ASN1Data = new byte[iLength];
				if(ASN1Data.length == iLength)
				{
					for(int i=0; i < iLength; i++)
					{
						ASN1Data[i] = 0;
					}
					pNextASN1 = 0; // First ASN1
					/*for(int n=0; n < LengthOfData; n++)
					{
						ASN1Data[n] = data[n]; // copy data into new allocated space
					}*/
					System.arraycopy(data,0,ASN1Data,0,LengthOfData); // copy data into new allocated space
					pAppendPosition = 0;
					result = 1;
				}
				else
					result = -2;
			}
			return result;
		}

		public int WhatIsReadSize()
		{
			return iLengthOfData;
		}

		public int WhatIsWriteSize()
		{
			int result=0;
			result = iTotalLengthOfData - iLengthOfData;
			if(result<0)
				result=0;
			return result;
		}

		public boolean AppendASN1(int LengthOfNewASN1Data, byte Tag, byte[] data)
		{
			boolean bResult=true;
			if(iTotalLengthOfData < (iLengthOfData + LengthOfNewASN1Data))
				bResult = false;
			else
			{
				pAppendPosition = iLengthOfData;
				ASN1Data[pAppendPosition + 0] = (byte)(LengthOfNewASN1Data     & 0x000000ff);
				ASN1Data[pAppendPosition + 1] = (byte)(LengthOfNewASN1Data>>8  & 0x000000ff);
				ASN1Data[pAppendPosition + 2] = (byte)(LengthOfNewASN1Data>>16 & 0x000000ff);
				ASN1Data[pAppendPosition + 3] = (byte)(LengthOfNewASN1Data>>24 & 0x000000ff);
				ASN1Data[pAppendPosition + 4] = (byte)(Tag & 0x000000FF); // unsigned char 8 bit  -- tag byte

				if(data.length > 0)
				{
					for (int i = 0; i < LengthOfNewASN1Data; i++){
						ASN1Data[pAppendPosition + 4 + 1 + i] = data[i];
					}

					iLengthOfData = iLengthOfData + 4 +1 + LengthOfNewASN1Data; // Add new ASN1 to length : Length+tag+SizeofData
				}
				else
					bResult=false;
			}
			return bResult;
		}


		/*
 		 param = {
 			length:0,
 			tag:0,
 			data:0
 			};
 		*/
		public boolean FetchNextASN1(asn param) // Returns true if ASN1 was found, and false if not.
		{
			boolean bResult = true;
			if (pNextASN1 >= 0)
			{
				param.Length = param.Length | (ASN1Data[pNextASN1 + 0] & 0x000000ff);
				param.Length = param.Length | (ASN1Data[pNextASN1 + 1] & 0x000000ff) << 8;
				param.Length = param.Length | (ASN1Data[pNextASN1 + 2] & 0x000000ff) << 16;
				param.Length = param.Length | (ASN1Data[pNextASN1 + 3] & 0x000000ff) << 24;

				pNextASN1 += 4; // sizeof(length) 32 bits
				param.Tag = ASN1Data[pNextASN1++] & 0x000000FF; // fetch byte tag

				param.data = new byte[param.Length];
				if (param.Length == 1)
				{
					param.data[0] = ASN1Data[pNextASN1];
				}
				else
				{
					if (param.Length == 2) {
						param.data[0] = ASN1Data[pNextASN1];
						param.data[1] = ASN1Data[pNextASN1 + 1];
					}
					else {
						//for (int i = 0; i < param.Length; i++)
						//	param.data[i] = ASN1Data[pNextASN1 + i];
						System.arraycopy(ASN1Data,pNextASN1,param.data,0,param.Length);
					}
				}
				NextASN1Position = CurrentASN1Position + param.Length + 1 + 4;
				if (NextASN1Position > iTotalLengthOfData || NextASN1Position < 0) {
					pNextASN1 = 0;
					bResult = false; // ASN1 says it is longer than ASN1 allocated space ??? ASN1 has illegal size.
				}
				else {
					CurrentASN1Position = NextASN1Position;
					if (CurrentASN1Position >= iTotalLengthOfData)
						pNextASN1 = 0;
					else
						pNextASN1 += param.Length;
				}
			}
			else
				bResult = false;

			return bResult;
		}


		public boolean FetchTotalASN1(data param)
		{
			boolean bResult = true;
			if (ASN1Data.length > 0)
			{
				param.Length = iLengthOfData; // Array can be larger than amount of valid data inside
				param.data = new byte[param.Length];
				//for (int i = 0; i < param.Length; i++)
				//	param.data[i] = ASN1Data[i];
				System.arraycopy(ASN1Data,0,param.data,0,param.Length);
			}
			else
				bResult = false;

			return bResult;
		}
	}

	class asn
	{
		int Length;
		int Tag;
		byte[] data;
	}

	byte[] pdata;
	int iLengthOfData;
	byte[] ptotaldata;
	int iLengthOfTotalData;
	CASN1 m_asn1; // used in decoder

	public byte[] decompress_lzss(byte[] pCompressedData, int sizeofCompressedData)
	{
		byte[] result=null;

		if(pCompressedData.length != sizeofCompressedData)
			return result;
		try {
			ByteArrayInputStream byteArrayInputStream = new ByteArrayInputStream(pCompressedData);
			LZSS lzss = new LZSS(byteArrayInputStream);
			ByteArrayOutputStream byteArrayOutputStream = lzss.uncompress();
			if(byteArrayOutputStream!=null)
				result = byteArrayOutputStream.toByteArray();
			else
				return null; // somehow data was not compressed
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}

		return result;
	}


	public DEDDecoder PUT_DATA_IN_DECODER(byte[] pCompressedData, int sizeofCompressedData)
	{
		if(pCompressedData==null) return null;
		decoder_ptr = new DEDDecoder();
		if(decoder_ptr==null) return null;
		byte[] decmprsd = decompress_lzss(pCompressedData, sizeofCompressedData);
		if(decmprsd==null) {
			// decompressed failed, perhaps it is not compressed
			decmprsd=pCompressedData;
		}
		if (decmprsd.length > 0) {
			decoder_ptr.ptotaldata = decmprsd;
			ptotaldata = decoder_ptr.ptotaldata;
			decoder_ptr.pdata = ptotaldata;
			decoder_ptr.iLengthOfTotalData = decmprsd.length;
			iLengthOfTotalData = decoder_ptr.iLengthOfTotalData;
		}
		return decoder_ptr;
	}


	public int GET_STRUCT_START(String name)
	{
		int result = -1;
		if(this.decoder_ptr==null) return -1;
		if(!name.isEmpty()) {
			param DEDobject = new param();
			DEDobject.name = name;
			DEDobject.ElementType = DED_ELEMENT_TYPE_STRUCT;
			DEDobject.value = null;
			DEDobject.length = 0;
			result = this.decoder_ptr._GetElement(DEDobject);
		}
		return result;
	}


	public String GET_METHOD(String name)
	{
		String result="";
		if(this.decoder_ptr==null) return "";
		if(!name.isEmpty()) {
			param DEDobject = new param();
			DEDobject.name = name;
			DEDobject.ElementType = DED_ELEMENT_TYPE_METHOD;
			DEDobject.value = null;
			DEDobject.length = 0;
			int found = decoder_ptr._GetElement(DEDobject);
			if (found == 1) {
				for(int i=0;i<DEDobject.value.length;i++)
					result = result + (char)DEDobject.value[i];
			}
		}
		return result;
	}

	public short GET_USHORT(String name)
	{
		short result = -1;
		if(this.decoder_ptr==null) return -1;
		if(!name.isEmpty()) {
			param DEDobject = new param();
			DEDobject.name = name;
			DEDobject.ElementType = DED_ELEMENT_TYPE_USHORT;
			DEDobject.value = null;
			DEDobject.length = 0;
			int found = decoder_ptr._GetElement(DEDobject);
			if (found == 1) {
				result = byteUtils.bytesToShort(DEDobject.value);
			}
		}
		return result;
	}

	public long GET_ULONG(String name)
	{
		long result = -1;
		if(this.decoder_ptr==null) return -1;
		if(!name.isEmpty()) {
			param DEDobject = new param();
			DEDobject.name = name;
			DEDobject.ElementType = DED_ELEMENT_TYPE_ULONG;
			DEDobject.value = null;
			DEDobject.length = 0;
			int found = decoder_ptr._GetElement(DEDobject);
			if (found == 1) {
				result = byteUtils.bytesToLong(DEDobject.value);
			}
		}
		return result;
	}

	public long GET_LONG(String name)
	{
		long result = -1;
		if(this.decoder_ptr==null) return -1;
		if(!name.isEmpty()) {
			param DEDobject = new param();
			DEDobject.name = name;
			DEDobject.ElementType = DED_ELEMENT_TYPE_LONG;
			DEDobject.value = null;
			DEDobject.length = 0;
			int found = decoder_ptr._GetElement(DEDobject);
			if (found == 1) {
				result = byteUtils.bytesToLong(DEDobject.value);
			}
		}
		return result;
	}

	public boolean GET_BOOL(String name)
	{
		boolean result = false;
		if(this.decoder_ptr==null) return false;
		if(!name.isEmpty()) {
			param DEDobject = new param();
			DEDobject.name = name;
			DEDobject.ElementType = DED_ELEMENT_TYPE_BOOL;
			DEDobject.value = null;
			DEDobject.length = 0;
			int found = decoder_ptr._GetElement(DEDobject);
			if (found == 1) {
				if(DEDobject.value[0]!=0)
					result=true;
				else
				    result=false;
			}
		}
		return result;
	}

	String emptycheck(String str)
	{
		String strreturn="";
		if(str=="##empty##")
			strreturn = "";
		else
			strreturn = str;
		return strreturn;
	}

	public String GET_STDSTRING(String name)
	{
		String result = "##empty##";
		if(this.decoder_ptr==null) return "##error null decoder_ptr##";
		if(!name.isEmpty()) {
			param DEDobject = new param();
			DEDobject.name = name;
			DEDobject.ElementType = DED_ELEMENT_TYPE_STDSTRING;
			DEDobject.value = null;
			DEDobject.length = -1;
			int found = decoder_ptr._GetElement(DEDobject);
			if (found == 1) {
				String strtest = new String(DEDobject.value, UTF8_CHARSET);
				if(strtest.length()>0)
					result = strtest;
				//result="";
				//for(int i=0;i<DEDobject.value.length;i++)
				//	result = result + (char)DEDobject.value[i];
			}
			result = emptycheck(result);
		}
		return result;
	}

	public byte[] GET_STDVECTOR(String name)
	{
		byte[] result = null;
		if(this.decoder_ptr==null) return null;
		if(!name.isEmpty()) {
			param DEDobject = new param();
			DEDobject.name = name;
			DEDobject.ElementType = DED_ELEMENT_TYPE_STDVECTOR;
			DEDobject.value = null;
			DEDobject.length = -1;
			int found = decoder_ptr._GetElement(DEDobject);
			if (found == 1) {
				result = DEDobject.value;
			}
		}
		return result;
	}


	//TODO: 20140724 consider designing _GET_ so that if element is NOT found, then internal pointer is NOT moved as it is NOW!!!
	public DEDDecoder._Elements GET_ELEMENT(String entityname)
	{
		param DEDobject1;
		param DEDobject2;
		int result = -1;

		_Elements elementvalue = new _Elements();
		try {
			if (this.decoder_ptr == null) return null;


			String strentity_chunk_id = entityname.toLowerCase() + "_chunk_id";
			String strentity_chunk_data = entityname.toLowerCase() + "_chunk_data";

			DEDobject1 = new param();
			DEDobject1.name = strentity_chunk_id;
			DEDobject1.ElementType = DED_ELEMENT_TYPE_STDSTRING;
			DEDobject1.value = null;
			DEDobject1.length = -1;

			DEDobject2 = new param();
			DEDobject2.name = strentity_chunk_data;
			DEDobject2.ElementType = DED_ELEMENT_TYPE_STDVECTOR;
			DEDobject2.value = null;
			DEDobject2.length = -1;

			result = decoder_ptr._GetElement(DEDobject1);
			if (result != -1) {
				result = decoder_ptr._GetElement(DEDobject2);
				if(result != -1) {
					elementvalue.strElementID = new String(DEDobject1.value, UTF8_CHARSET);
					elementvalue.ElementData = new byte[DEDobject2.value.length];
					System.arraycopy(DEDobject2.value, 0, elementvalue.ElementData, 0, DEDobject2.value.length);
					//elementvalue.ElementData = DEDobject2.value;
				}
				else
					return null;
			}
			else
				return null;
		}
		catch (Exception e){
		    elementvalue = null;
			e.printStackTrace();
		}
		return elementvalue;
	}

	////////////////////////////////////////////////////////////////
	// FETCH ELEMENTS FROM ASN1 DATAENCODER
	////////////////////////////////////////////////////////////////
	private int _GetElement(param DEDobject)
	{
		int result=-1;
		if (DEDobject.ElementType == DED_ELEMENT_TYPE_STRUCT)
		{
			m_asn1 = new CASN1();
			result = m_asn1.CASN1p3(iLengthOfTotalData, pdata, iLengthOfTotalData + 1);
		}

		int ElementType = DEDobject.ElementType;
		asn param = new asn();
		if (m_asn1.FetchNextASN1(param))
		{
			if (param.Tag == ElementType)
			{
				String strCmp="";
				for(int i=0;i<param.Length;i++)
					strCmp = strCmp + (char)param.data[i];
				if (DEDobject.name.equals(strCmp))
				{
					if (param.Tag == DED_ELEMENT_TYPE_STRUCT || param.Tag == DED_ELEMENT_TYPE_STRUCT_END)
					{
						// start and end elements does NOT have value, thus no need to go further
						result = 1;
					}
					else {
						param.Length = 0;
						param.Tag = 0;
						param.data = null;
						if (m_asn1.FetchNextASN1(param))
						{
							if (param.Tag == ElementType)
							{
								if (ElementType == DED_ELEMENT_TYPE_METHOD || ElementType == DED_ELEMENT_TYPE_STRING || ElementType == DED_ELEMENT_TYPE_STDSTRING)
								{
									String str="";
									for(int i=0;i<param.Length;i++) {
										try {
											str = str + (char) param.data[i];
											Thread.sleep(0,100);
										} catch (InterruptedException e) {
											e.printStackTrace();
										}
									}
									DEDobject.value = str.getBytes();
								}
								else
								{
									DEDobject.value = param.data;
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


// ...
//
//

	public int GET_STRUCT_END(String name)
	{
		int result = 0;
		if(this.decoder_ptr==null) return -1;
		param DEDobject = new param();
		DEDobject.name = name;
		DEDobject.ElementType = DED_ELEMENT_TYPE_STRUCT_END;
		DEDobject.value = null;
		DEDobject.length = -1;
		result = decoder_ptr._GetElement(DEDobject);
		if(result==-1) // last element received
			result=1;
		return result;
	}


	static final Charset UTF8_CHARSET = Charset.forName("UTF-8");
	private static final Log logger = LogFactory.getLog(DEDDecoder.class);


}
