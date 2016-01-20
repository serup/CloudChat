using System;
using System.Text;
using System.IO;
using csharpServices;

namespace DED
{

	public class ByteUtils {

		/**
    * Element types
    */
	    public static int DED_ELEMENT_TYPE_BOOL = 0;
		public static int DED_ELEMENT_TYPE_CHAR = 1;
		public static int DED_ELEMENT_TYPE_BYTE = 2;
		//public static int DED_ELEMENT_TYPE_WCHAR =  3;
		//public static int DED_ELEMENT_TYPE_SHORT =  4;
		public static int DED_ELEMENT_TYPE_USHORT = 5;
		public static int DED_ELEMENT_TYPE_LONG = 6;
		public static int DED_ELEMENT_TYPE_ULONG = 7;
		public static int DED_ELEMENT_TYPE_FLOAT = 8;
		//public static int DED_ELEMENT_TYPE_DOUBLE = 9;
		public static int DED_ELEMENT_TYPE_STRING = 10;
		//public static int DED_ELEMENT_TYPE_WSTRING = 11;
		//public static int DED_ELEMENT_TYPE_URI     = 12;
		public static int DED_ELEMENT_TYPE_METHOD = 13;
		public static int DED_ELEMENT_TYPE_STRUCT = 14;
		//public static int DED_ELEMENT_TYPE_LIST   = 15
		//public static int DED_ELEMENT_TYPE_ARRAY  = 16;
		public static int DED_ELEMENT_TYPE_ZERO = 17;
		public static int DED_ELEMENT_TYPE_CLASS = 18;
		public static int DED_ELEMENT_TYPE_STDSTRING = 19;
		public static int DED_ELEMENT_TYPE_SVGFILE = 20;
		public static int DED_ELEMENT_TYPE_VA_LIST = 21;
		//public static int DED_ELEMENT_TYPE_FILE    = 22;
		public static int DED_ELEMENT_TYPE_STDVECTOR = 23;
		public static int DED_ELEMENT_TYPE_STRUCT_END = 24;
		//...
		public static int DED_ELEMENT_TYPE_UNKNOWN = 0xff;

		public byte[] toBytes(char[] chars) {
			return Encoding.Unicode.GetBytes (chars);
			//Convert.FromBase64CharArray (chars, 0, chars.Length);
		}

		public short bytesToShort(byte[] byteArray)
		{
			return BitConverter.ToInt16 (byteArray,0);
		}

		public long bytesToLong(byte[] byteArray)
		{
			return BitConverter.ToInt64 (byteArray, 0);
		}

	}

	public class _Elements
	{
		public String strElementID;
		public byte[] ElementData;
	}

	class data
	{
		public int Length;
		public byte[] _data;
	}

	class asn
	{
		public int Length;
		public int Tag;
		public byte[] data;
	}

	/*
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
				if(ASN1Data.Length == iAppendMaxLength)
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
			if(data==null)
				return -1;
			iLengthOfData = LengthOfData;
			iTotalLengthOfData = iLength; // max room for data
			if(iLength != 0)
			{
				ASN1Data = new byte[iLength];
				if(ASN1Data.Length == iLength)
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
					System.Array.Copy(data,0,ASN1Data,0,LengthOfData); // copy data into new allocated space
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

		public Boolean AppendASN1(int LengthOfNewASN1Data, byte Tag, byte[] data)
		{
			Boolean bResult=true;
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

				if(data.Length > 0)
				{
					//for (int i = 0; i < LengthOfNewASN1Data; i++){
					//	ASN1Data[pAppendPosition + 4 + 1 + i] = data[i];
					//}
					System.Array.Copy(data,0,ASN1Data,pAppendPosition+4+1,LengthOfNewASN1Data);
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
		public Boolean FetchNextASN1(asn param) // Returns true if ASN1 was found, and false if not.
		{
			Boolean bResult = true;
			if (pNextASN1 >= 0)
			{
				param.Length = param.Length | (ASN1Data[pNextASN1 + 0] & 0x000000ff);
				param.Length = param.Length | (ASN1Data[pNextASN1 + 1] & 0x000000ff) << 8;
				param.Length = param.Length | (ASN1Data[pNextASN1 + 2] & 0x000000ff) << 16;
				param.Length = param.Length | (ASN1Data[pNextASN1 + 3] & 0x000000ff) << 24;

				pNextASN1 += 4; // sizeof(length) 32 bits
				param.Tag = ASN1Data[pNextASN1++] & 0x000000FF; // fetch byte tag

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
						param.data = new byte[param.Length];
						//for (int i = 0; i < param.Length; i++)
						//	param.data[i] = ASN1Data[pNextASN1 + i];
						System.Array.Copy(ASN1Data,pNextASN1,param.data,0,param.data.Length);
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


		public Boolean FetchTotalASN1(data param)
		{
			Boolean bResult = true;
			if (ASN1Data.Length > 0)
			{
				param.Length = iLengthOfData; // Array can be larger than amount of valid data inside
				param._data = new byte[param.Length];
				//for (int i = 0; i < param.Length; i++)
				//	param.data[i] = ASN1Data[i];
				System.Array.Copy(ASN1Data,0,param._data,0,param.Length);
			}
			else
				bResult = false;

			return bResult;
		}
	}

/*
  Decodes one or more DED frames contained in a {@link ByteBuffer}.

  <p>An attempt is made to read all complete DED frames from the buffer, which
  could be zero, one, or more.
  The caller is responsible for dealing with incomplete content buffering until there is more input available.

  example:

    short trans_id = 1;
    Boolean action = true;

    DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
    DED.PUT_STRUCT_START( DED, "event" );
       DED.PUT_METHOD  ( DED, "name",  "MusicPlayer" );
       DED.PUT_USHORT  ( DED, "trans_id",  trans_id);
       DED.PUT_BOOL    ( DED, "startstop", action );
    DED.PUT_STRUCT_END( DED, "event" );
    DED.GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);

    //..transmitting :-) simulation

	//..receiving :-) simulation

	   DEDDecoder DED = DEDDecoder.DED_START_DECODER();
	   DED.PUT_DATA_IN_DECODER(decoder_ptr,pCompressedData,sizeofCompressedData);

	   Boolean bDecoded=false;
	   String strValue;
	   short iValue;
	   Boolean bValue;

	   // decode data ...

	   if( DED.GET_STRUCT_START( DED, "event" ) &&
		          DED.GET_METHOD ( DED, "name", strValue ) &&
		          DED.GET_USHORT ( DED, "trans_id", iValue) &&
		          DED.GET_BOOL   ( DED, "startstop", bValue ) &&
		       DED.GET_STRUCT_END( DED, "event" ))
		   {
		     bDecoded=true;
		   }
	   else
		   {
		     bDecoded=false;
		   }


	*/
	public class DEDDecoder {

		public ByteUtils byteUtils;
		DEDDecoder decoder_ptr;
		/* */

		public DEDDecoder()
		{
			byteUtils = new ByteUtils();
		}

		public static DEDDecoder DED_START_DECODER()
		{
			DEDDecoder dedDecoder = new DEDDecoder();
			return dedDecoder;
		}

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


		byte[] pdata;
		int iLengthOfData;
		byte[] ptotaldata;
		int iLengthOfTotalData;
		CASN1 m_asn1; // used in decoder

		public byte[] decompress_lzss(byte[] pCompressedData, int sizeofCompressedData)
		{
			byte[] result=null;

			if(pCompressedData.Length != sizeofCompressedData)
				return result;
			try {
				MemoryStream byteArrayOutputStream = LZSS.Decompress(pCompressedData);
				if(byteArrayOutputStream!=null)
					result = byteArrayOutputStream.GetBuffer();
				else
					return null; // somehow data was not decompressed
			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());
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
			if (decmprsd.Length > 0) {
				decoder_ptr.ptotaldata = decmprsd;
				ptotaldata = decoder_ptr.ptotaldata;
				decoder_ptr.pdata = ptotaldata;
				decoder_ptr.iLengthOfTotalData = decmprsd.Length;
				iLengthOfTotalData = decoder_ptr.iLengthOfTotalData;
			}
			return decoder_ptr;
		}


		public int GET_STRUCT_START(String name)
		{
			int result = -1;
			if(this.decoder_ptr==null) return -1;
			if(name.Length != 0) {
				param DEDobject = new param();
				DEDobject.name = name;
				DEDobject.ElementType = ByteUtils.DED_ELEMENT_TYPE_STRUCT;
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
			if(name.Length != 0) {
				param DEDobject = new param();
				DEDobject.name = name;
				DEDobject.ElementType = ByteUtils.DED_ELEMENT_TYPE_METHOD;
				DEDobject.value = null;
				DEDobject.length = 0;
				int found = decoder_ptr._GetElement(DEDobject);
				if (found == 1) {
					for(int i=0;i<DEDobject.value.Length;i++)
						result = result + (char)DEDobject.value[i];
				}
			}
			return result;
		}

		public short GET_USHORT(String name)
		{
			short result = -1;
			if(this.decoder_ptr==null) return -1;
			if(name.Length != 0) {
				param DEDobject = new param();
				DEDobject.name = name;
				DEDobject.ElementType = ByteUtils.DED_ELEMENT_TYPE_USHORT;
				DEDobject.value = null;
				DEDobject.length = 0;
				int found = decoder_ptr._GetElement(DEDobject);
				if (found == 1) {
					result = byteUtils.bytesToShort(DEDobject.value);
				}
			}
			return result;
		}

		public int GET_LONG(String name, long value)
		{
			int result = -1;
			if(this.decoder_ptr==null) return -1;
			if(name.Length != 0) {
				param DEDobject = new param();
				DEDobject.name = name;
				DEDobject.ElementType = ByteUtils.DED_ELEMENT_TYPE_LONG;
				DEDobject.value = null;
				DEDobject.length = 0;
				int found = decoder_ptr._GetElement(DEDobject);
				if (found == 1) {
					value = byteUtils.bytesToLong(DEDobject.value);
					result = 1;
				}
			}
			return result;
		}

		public Boolean GET_BOOL(String name)
		{
			Boolean result = false;
			if(this.decoder_ptr==null) return false;
			if(name.Length != 0) {
				param DEDobject = new param();
				DEDobject.name = name;
				DEDobject.ElementType = ByteUtils.DED_ELEMENT_TYPE_BOOL;
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
			if(name.Length != 0) {
				param DEDobject = new param();
				DEDobject.name = name;
				DEDobject.ElementType = ByteUtils.DED_ELEMENT_TYPE_STDSTRING;
				DEDobject.value = null;
				DEDobject.length = -1;
				int found = decoder_ptr._GetElement(DEDobject);
				if (found == 1) {
					String strtest = BitConverter.ToString(DEDobject.value);
					if(strtest.Length > 0)
						result = strtest;
				}
				result = emptycheck(result);
			}
			return result;
		}


		//TODO: 20140724 consider designing _GET_ so that if element is NOT found, then internal pointer is NOT moved as it is NOW!!!
		public int GET_ELEMENT(String entityname, _Elements elementvalue)
		{
			int result = -1;
			if(this.decoder_ptr==null) return -1;

			String strentity_chunk_id = entityname.ToLower() + "_chunk_id";
			String strentity_chunk_data = entityname.ToLower() + "_chunk_data";

			param DEDobject1 = new param();
			DEDobject1.name = strentity_chunk_id;
			DEDobject1.ElementType = ByteUtils.DED_ELEMENT_TYPE_STDSTRING;
			DEDobject1.value = null;
			DEDobject1.length = -1;

			param DEDobject2 = new param();
			DEDobject2.name = strentity_chunk_data;
			DEDobject2.ElementType = ByteUtils.DED_ELEMENT_TYPE_STDVECTOR;
			DEDobject2.value = null;
			DEDobject2.length = -1;

			result = decoder_ptr._GetElement(DEDobject1);
			if (result == 1)
				result = (int)byteUtils.bytesToLong(DEDobject1.value);
			else
				result = -1;
			if (result != -1){
				result = decoder_ptr._GetElement(DEDobject2);
				if (result == 1)
					result = (int)byteUtils.bytesToLong(DEDobject2.value);
				else
					result = -1;
			}
			elementvalue.strElementID = DEDobject1.value.ToString();
			elementvalue.ElementData = DEDobject2.value;
			if(result != -1) result = 1;
			return result;
		}

		////////////////////////////////////////////////////////////////
		// FETCH ELEMENTS FROM ASN1 DATAENCODER
		////////////////////////////////////////////////////////////////
		private int _GetElement(param DEDobject)
		{
			int result=-1;
			if (DEDobject.ElementType == ByteUtils.DED_ELEMENT_TYPE_STRUCT)
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
					//strCmp = param.data.toString();
					if (DEDobject.name.Equals(strCmp))
					{
						if (param.Tag == ByteUtils.DED_ELEMENT_TYPE_STRUCT || param.Tag == ByteUtils.DED_ELEMENT_TYPE_STRUCT_END)
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
									if (ElementType == ByteUtils.DED_ELEMENT_TYPE_METHOD || ElementType == ByteUtils.DED_ELEMENT_TYPE_STRING || ElementType == ByteUtils.DED_ELEMENT_TYPE_STDSTRING)
									{
										String str="";
										for(int i=0;i<param.Length;i++)
											str = str + (char)param.data[i];
										//str = param.data.toString();
										DEDobject.value = byteUtils.toBytes(str.ToCharArray());
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
			DEDobject.ElementType = ByteUtils.DED_ELEMENT_TYPE_STRUCT_END;
			DEDobject.value = null;
			DEDobject.length = -1;
			result = decoder_ptr._GetElement(DEDobject);
			if(result==-1) // last element received
				result=1;
			return result;
		}
	}

	public class DEDEncoder
	{

		public ByteUtils byteUtils;
		DEDEncoder dedEncoder;
		public DEDobject dedobject;

		public DEDEncoder()
		{
			byteUtils = new ByteUtils();
			dedEncoder = this;
			dedobject = new DEDobject();
			//DED_START_ENCODER();
		}



		byte[] pdata;
		int iLengthOfData;
		byte[] ptotaldata;
		int iLengthOfTotalData;


		/**
	 * Functions / methods for the encoder
	 * inorder to be able to make the usage look something like this:
	 *
	 * DEDEncoder DED = DEDEncoder.DED_START_ENCODER();
	 * DED.PUT_STRUCT_START( DED, "event" );
	 *  DED.PUT_METHOD  ( DED, "name",  "MusicPlayer" );
	 *  DED.PUT_USHORT  ( DED, "trans_id",      trans_id);
	 *  DED.PUT_BOOL    ( DED, "startstop", action );
	 * DED.PUT_STRUCT_END( DED, "event" );
	 */
		public static DEDEncoder DED_START_ENCODER()
		{
	        DEDEncoder dedEncoder = new DEDEncoder();
			return dedEncoder;
		}

		public int PUT_STRUCT_START(String name)
		{
			int result=-1;
			if(this.dedEncoder != null)
				result = this.dedEncoder.EncodeStructStart(name);
			return result;
		}

		public int PUT_STRUCT_END(String name)
		{
			int result = -1;
			if (this.dedEncoder != null)
				result = this.dedEncoder.EncodeStructEnd(name);

			return result;
		}

		public int PUT_METHOD(String name, String value)
		{
			int result = -1;
			if (this.dedEncoder != null)
				result = this.dedEncoder.EncodeMethod(name, byteUtils.toBytes(value.ToCharArray()), value.Length);

			return result;
		}

		public int PUT_USHORT(String name, short value)
		{
			int result = -1;
			if (this.dedEncoder != null)
				result = this.dedEncoder.EncodeUShort(name, value,  2 );

			return result;
		}

		public int PUT_LONG(String name, long value)
		{
			int result = -1;
			if (this.dedEncoder != null){
				result = this.dedEncoder.EncodeLong(name, value, 8 );

			}

			return result;
		}

		public int PUT_BOOL(String name, Boolean value)
		{
			int result = -1;
			if (this.dedEncoder != null) {
				result = this.dedEncoder.EncodeBool(name, value, 1);
			}
			return result;
		}

		public int PUT_STDSTRING(String name, String value)
		{
			int result = -1;
			if (this.dedEncoder != null) {
				String strEmpty = "##empty##";
				if(value.Length <= 0)
					value = strEmpty;

				result = this.dedEncoder.EncodeStdString(name, value, value.Length);
			}
			return result;
		}

		public int PUT_ELEMENT(String entityname, String elementname, byte[] elementvalue)
		{
			int result = -1;
			if (this.dedEncoder != null) {
				result = this.dedEncoder.EncodeElement(entityname, elementname, elementvalue); // will add _chunk_id and _chunk_data
			}
			return result;
		}

		public byte[] compress_lzss(byte[] uncompressedData, long iLengthUncompressedData  )
		{
			byte[] result=null;
			if(uncompressedData.Length != iLengthUncompressedData)
				return result;
			try {
				Stream byteArrayInputStream = new MemoryStream(uncompressedData);
				MemoryStream byteArrayOutputStream = LZSS.Compress(ref byteArrayInputStream);
				result = byteArrayOutputStream.GetBuffer();
			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());
			}

			return result;
		}

		public int GET_ENCODED_DATA(DEDobject obj)
		{
			int result = -1;
			if (this.dedEncoder != null) {
				obj.uncompresseddata = this.dedEncoder.DataEncoder_GetData(obj);

				// Do compression - okumura style
				// First make sure byte are in same format !!!!
				byte[] uncmpdata = new byte[obj.uncompresseddata.Length];
				//for (int i = 0; i < object.uncompresseddata.Length; i++) {
				//	uncmpdata[i] = object.uncompresseddata[i];
				//}
				System.Array.Copy(obj.uncompresseddata,0,uncmpdata,0,obj.uncompresseddata.Length);
				// now make room for case where compression yields lager size - when trying to compress an image for example.
				//byte[] tmpCompressedData = new byte[uncmpdata.length * 2];
				byte[] tmpCompressedData;
				// now compress
				//int compressedSize = compress_lzss(tmpCompressedData, tmpCompressedData.length * 2, uncmpdata, uncmpdata.length);
				tmpCompressedData = compress_lzss(uncmpdata, uncmpdata.Length);
				int compressedSize=-1;
				if(tmpCompressedData != null)
					compressedSize = tmpCompressedData.Length;
				if (compressedSize > 0) {
					obj.pCompressedData = new byte[compressedSize];
					obj.sizeofCompressedData = compressedSize;
					//for (int i = 0; i < compressedSize; i++)
					//	object.pCompressedData[i] = tmpCompressedData[i];
					System.Array.Copy(tmpCompressedData,0,obj.pCompressedData,0,compressedSize);
					result = 1;
				} else {
					// somehow compression went wrong !!!! ignore and just use uncompressed data - perhaps data was already compressed !?
					obj.pCompressedData = new byte[uncmpdata.Length];
					obj.sizeofCompressedData = uncmpdata.Length;
					//for (int i = 0; i < object.uncompresseddata.Length; i++)
					//	object.pCompressedData[i] = uncmpdata[i];
					System.Array.Copy(uncmpdata,0,obj.pCompressedData,0,obj.uncompresseddata.Length);
					result = 1;
				}
			}
			return result;
		}

//		public ByteBuffer GET_ENCODED_BYTEBUFFER_DATA()
//		{
//			ByteBuffer data;
//			DEDobject dedobject = new DEDobject();
//			this.GET_ENCODED_DATA(dedobject);
//			if(dedobject.getiLengthOfTotalData()>0)
//				data = ByteBuffer.wrap(dedobject.getpCompressedData());
//			else {
//				data = null;
//			}
//			return data;
//		}

		public byte[] GET_ENCODED_BYTEARRAY_DATA()
		{
			DEDobject dedobject = new DEDobject();
			this.GET_ENCODED_DATA(dedobject);
			if (dedobject.iLengthOfTotalData > 0) {
				if (dedobject.pCompressedData.Length > 0)
					return dedobject.pCompressedData;
				else
					return dedobject.uncompresseddata;
			}
			else
				return null;
		}


		class param
		{
			public String name;
			public int ElementType;
			public byte[] value;
			public int length;
		}

		/**
	 * General function for adding elements to array as ASN1 elements
	 *
	 * @param element
	 * @return
     */
		//TODO: redesign with ByteBuffer or ByteArrayOutputStream - to handle larger than byte[int] aka. byte[long]
		private int AddElement(param element)
		{
			int result;
			if (element.ElementType == ByteUtils.DED_ELEMENT_TYPE_STRUCT)
			{
				// First element in structure
				iLengthOfData = 0;
				ptotaldata = null;
				iLengthOfTotalData = 0;
				CASN1 asn1 = new CASN1();
				result = asn1.CASN1p1(element.name.Length + 4 + 1);

				int LengthOfAsn1 = element.name.Length;
				asn1.AppendASN1(LengthOfAsn1, (byte)element.ElementType, byteUtils.toBytes(element.name.ToCharArray()));

				data paramasn1 = new data();
				asn1.FetchTotalASN1(paramasn1);
				iLengthOfTotalData = paramasn1.Length;

				iLengthOfData = iLengthOfTotalData; // First element in structure
				pdata = new byte[iLengthOfData];
				//for (int i = 0; i < paramasn1.Length; i++)
				//	pdata[i] = paramasn1.data[i];
				System.Array.Copy(paramasn1._data,0,pdata,0,paramasn1.Length);
				ptotaldata = pdata;
				//ptotaldata = null; // DED_ELEMENT_TYPE_STRUCT does NOT have a value
			}
			else
			{
				CASN1 asn1 = new CASN1();
				result = asn1.CASN1p3(iLengthOfTotalData, pdata, iLengthOfTotalData + element.name.Length + element.length + 1);

				// 1. asn  "name"
				int LengthOfAsn1 = element.name.Length;
				asn1.AppendASN1(LengthOfAsn1, (byte)element.ElementType, byteUtils.toBytes(element.name.ToCharArray()));
				// 2. asn "value"
				LengthOfAsn1 = element.length;
				if (LengthOfAsn1 > 0)
					asn1.AppendASN1(LengthOfAsn1, (byte)element.ElementType, element.value);

				data paramasn1 = new data();
				asn1.FetchTotalASN1(paramasn1);
				iLengthOfTotalData = paramasn1.Length;
				if(iLengthOfTotalData > 0)
				{
					pdata = new byte[iLengthOfTotalData];
					//for (int i = 0; i < iLengthOfTotalData; i++)
					//	pdata[i] = paramasn1.data[i];
					System.Array.Copy(paramasn1._data,0,pdata,0,paramasn1.Length);
					ptotaldata = pdata;
				}
			}
			if(iLengthOfTotalData > 0)
				result = iLengthOfTotalData;
			return result;
		}

		private int EncodeStructStart(String name)
		{
			int result=-1;
			if(name.Length != 0) {
				param element = new param();
				element.name = name;
				element.ElementType = ByteUtils.DED_ELEMENT_TYPE_STRUCT;
				element.value = null;
				element.length = 0;
				result = AddElement(element);
			}
			return result;
		}

		private int EncodeStructEnd(String name)
		{
			int result = -1;
			if(name.Length != 0) {
				param element = new param();
				element.name = name;
				element.ElementType = ByteUtils.DED_ELEMENT_TYPE_STRUCT_END;
				element.value = null;
				element.length = 0;
				result = AddElement(element);
			}
			return result;
		}

		private int EncodeMethod(String name, byte[] value, int length)
		{
			int result = -1;
			if(name.Length != 0) {
				param element = new param();
				element.name = name;
				element.ElementType = ByteUtils.DED_ELEMENT_TYPE_METHOD;
				element.value = value;
				element.length = length;
				result = AddElement(element);
			}
			return result;
		}

		private int EncodeUShort(String name, short value, int length)
		{
			int result = -1;
			if(name.Length != 0) {
				param element = new param();
				element.name = name;
				element.ElementType = ByteUtils.DED_ELEMENT_TYPE_USHORT;
//				element.value = byteUtils.shortToBytes(value);
				element.value = BitConverter.GetBytes(value);
				element.length = length;
				result = AddElement(element);
			}
			return result;
		}


		private int EncodeLong(String name, long value, int length)
		{
			int result = -1;
			if(name.Length != 0) {
				param element = new param();
				element.name = name;
				element.ElementType = ByteUtils.DED_ELEMENT_TYPE_LONG;
				//element.value = byteUtils.longToBytes(value);
				element.value = BitConverter.GetBytes(value);
				element.length = length;
				result = AddElement(element);
			}
			return result;
		}

		private int EncodeBool(String name, Boolean value, int length)
		{
			int result = -1;
			if(name.Length != 0) {
				param element = new param();
				element.name = name;
				element.ElementType = ByteUtils.DED_ELEMENT_TYPE_BOOL;
				element.value = new byte[1];
				element.value[0] = (value == true) ? (byte)1 : (byte)0;
				element.length = length;
				result = AddElement(element);
			}
			return result;
		}

		private int EncodeStdString(String name, String value, int length)
		{
			int result = -1;
			if(name.Length != 0) {
				param element = new param();
				element.name = name;
				element.ElementType = ByteUtils.DED_ELEMENT_TYPE_STDSTRING;
				element.value = byteUtils.toBytes(value.ToCharArray());
				element.length = length;
				result = AddElement(element);
			}
			return result;
		}

		private int Encodestdvector(String name, byte[] value, int length)
		{
			int result = -1;
			if(name.Length == 0) {
				param element = new param();
				element.name = name;
				element.ElementType = ByteUtils.DED_ELEMENT_TYPE_STDVECTOR;
				element.value = value;
				element.length = length;
				result = AddElement(element);
			}
			return result;
		}


		private int EncodeElement(String entityname, String elementname, byte[] elementvalue)
		{
			int result;
			_Elements element = new _Elements();
			element.strElementID = elementname;
			element.ElementData = elementvalue;
			String strentity_chunk_id = entityname.ToLower() + "_chunk_id";
			String strentity_chunk_data = entityname.ToLower() + "_chunk_data";

			result = EncodeStdString(strentity_chunk_id, element.strElementID, element.strElementID.Length); // key of particular item
			if(result != -1)
				result = Encodestdvector(strentity_chunk_data, element.ElementData, elementvalue.Length); //
			return result;
		}

		public class DEDobject
		{
			public DEDEncoder encoder_ptr;
			public byte[] uncompresseddata;
			public long iLengthOfTotalData;
			public byte[] pCompressedData;
			public long sizeofCompressedData;
		}

		private byte[] DataEncoder_GetData(DEDobject obj)
		{
			byte[] result = null;
			if (ptotaldata.Length > 0) {
				obj.iLengthOfTotalData = iLengthOfTotalData;
				result = ptotaldata;
			}
			return result;
		}


	}
}

