//TODO: change to fit DED ASN1 Okumura compress
/*
 * Copyright 2014-2016 SCANVA ApS
 *
 * This Encoder follows same structure as the Encoder for STOMP, however it is NOT the same
 * - DED is a completely different protocol
 */

package messaging.simp.ded;

import java.io.*;
import java.nio.*;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import messaging.simp.ded.compression.LZSS;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import org.springframework.messaging.Message;
import org.springframework.messaging.simp.SimpMessageHeaderAccessor;
import org.springframework.messaging.simp.SimpMessageType;
import org.springframework.messaging.support.NativeMessageHeaderAccessor;
import org.springframework.util.Assert;
import org.springframework.util.ExceptionTypeFilter;
import org.springframework.util.SystemPropertyUtils;




/**
 * An encoder for DED frames.
 *
 * @author Johnny Serup
 */
public class DEDEncoder  {


	public class ByteUtils {
		private ByteBuffer buffer = ByteBuffer.allocate(Long.BYTES);

		public byte[] shortToBytes(short x)
		{
			ByteBuffer buffershort = ByteBuffer.allocate(2);
			buffershort.putShort(x);
			return buffershort.array();
		}

		public byte[] longToBytes(long x) {
			buffer.putLong(0, x);
			return buffer.array();
		}

		public long bytesToLong(byte[] bytes) {
			buffer.put(bytes, 0, bytes.length);
			buffer.flip();//need flip
			return buffer.getLong();
		}

		public short bytesToShort(byte[] bytes) {
            ByteBuffer buffershort = ByteBuffer.allocate(2);
			buffershort.put(bytes, 0, 2);
			buffershort.flip();//need flip
			return buffershort.getShort();
		}

	}

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
			if(data==null)
				return -1;
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
					//for (int i = 0; i < LengthOfNewASN1Data; i++){
					//	ASN1Data[pAppendPosition + 4 + 1 + i] = data[i];
					//}
					System.arraycopy(data,0,ASN1Data,pAppendPosition+4+1,LengthOfNewASN1Data);
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
	public DEDEncoder DED_START_ENCODER()
	{
		this.dedEncoder = new DEDEncoder();
		return this.dedEncoder;
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
			result = this.dedEncoder.EncodeMethod(name, value.getBytes(), value.length());

		return result;
	}

	public int PUT_USHORT(String name, short value)
	{
		int result = -1;
		if (this.dedEncoder != null)
			result = this.dedEncoder.EncodeUShort(name, this.dedEncoder.byteUtils.shortToBytes(value), 2);

		return result;
	}

	public int PUT_LONG(String name, long value)
	{
		int result = -1;
		if (this.dedEncoder != null){
			result = this.dedEncoder.EncodeLong(name, value, 1);

		}

		return result;
	}

	public int PUT_BOOL(String name, boolean value)
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
			if(value.length() <= 0)
				value = strEmpty;

			result = this.dedEncoder.EncodeStdString(name, value, value.length());
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

	//TODO: change to fit JAVA style of programming -- reference in parameter does not really work onless you use objects
	//TODO: change to return compressed array instead of changing parameters!!
	int compress_lzss(byte[] compressedData ,long iLengthCompressedData, byte[] uncompressedData, long iLengthUncompressedData  )
	{
		int result=-1;
		if(uncompressedData.length != iLengthUncompressedData)
			return result;
		try {
			ByteArrayInputStream byteArrayInputStream = new ByteArrayInputStream(uncompressedData);
			LZSS lzss = new LZSS(byteArrayInputStream);
			ByteArrayOutputStream byteArrayOutputStream = lzss.compress();
			result = byteArrayInputStream.available();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
		return result;
	}

	public int GET_ENCODED_DATA(DEDobject object)
	{
		int result = -1;
		if (this.dedEncoder != null) {
			object.uncompresseddata = this.dedEncoder.DataEncoder_GetData(object);

			// Do compression - okumura style
			// First make sure byte are in same format !!!!
			byte[] uncmpdata = new byte[object.uncompresseddata.length];
			//for (int i = 0; i < object.uncompresseddata.length; i++) {
			//	uncmpdata[i] = object.uncompresseddata[i];
			//}
			System.arraycopy(object.uncompresseddata,0,uncmpdata,0,object.uncompresseddata.length);
			// now make room for case where compression yields lager size - when trying to compress an image for example.
			byte[] tmpCompressedData = new byte[uncmpdata.length * 2];
			// now compress
			int compressedSize = compress_lzss(tmpCompressedData, tmpCompressedData.length * 2, uncmpdata, uncmpdata.length);
			if (compressedSize > 0) {
				object.pCompressedData = new byte[compressedSize];
				object.sizeofCompressedData = compressedSize;
				//for (int i = 0; i < compressedSize; i++)
				//	object.pCompressedData[i] = tmpCompressedData[i];
				System.arraycopy(tmpCompressedData,0,object.pCompressedData,0,compressedSize);
				result = 1;
			} else {
				// somehow compression went wrong !!!! ignore and just use uncompressed data - perhaps data was already compressed !?
				object.pCompressedData = new byte[uncmpdata.length];
				object.sizeofCompressedData = uncmpdata.length;
				//for (int i = 0; i < object.uncompresseddata.length; i++)
				//	object.pCompressedData[i] = uncmpdata[i];
				System.arraycopy(uncmpdata,0,object.pCompressedData,0,object.uncompresseddata.length);
				result = 1;
			}
		}
		return result;
	}

	public ByteBuffer GET_ENCODED_BYTEBUFFER_DATA()
	{
		ByteBuffer data;
        DEDobject dedobject = new DEDobject();
		this.GET_ENCODED_DATA(dedobject);
		if(dedobject.getiLengthOfTotalData()>0)
			data = ByteBuffer.wrap(dedobject.getpCompressedData());
		else {
			data = null;
		}
		return data;
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

	class param
	{
		public String name;
		public int ElementType;
		public byte[] value;
		public int length;
	}
	class data
	{
		int Length;
		byte[] data;
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
        if (element.ElementType == DED_ELEMENT_TYPE_STRUCT)
		{
			// First element in structure
			iLengthOfData = 0;
			ptotaldata = null;
			iLengthOfTotalData = 0;
			CASN1 asn1 = new CASN1();
			result = asn1.CASN1p1(element.name.length() + 4 + 1);

			int LengthOfAsn1 = element.name.length();
			asn1.AppendASN1(LengthOfAsn1, (byte)element.ElementType, element.name.getBytes());

			data paramasn1 = new data();
			asn1.FetchTotalASN1(paramasn1);
			iLengthOfTotalData = paramasn1.Length;

			iLengthOfData = iLengthOfTotalData; // First element in structure
			pdata = new byte[iLengthOfData];
			//for (int i = 0; i < paramasn1.Length; i++)
			//	pdata[i] = paramasn1.data[i];
			System.arraycopy(paramasn1.data,0,pdata,0,paramasn1.Length);
			ptotaldata = pdata;


			//ptotaldata = null; // DED_ELEMENT_TYPE_STRUCT does NOT have a value
		}
		else
		{
			CASN1 asn1 = new CASN1();
			result = asn1.CASN1p3(iLengthOfTotalData, pdata, iLengthOfTotalData + element.name.length() + element.length + 1);

			// 1. asn  "name"
			int LengthOfAsn1 = element.name.length();
			asn1.AppendASN1(LengthOfAsn1, (byte)element.ElementType, element.name.getBytes());
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
				System.arraycopy(paramasn1.data,0,pdata,0,paramasn1.Length);
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
		if(!name.isEmpty()) {
			param element = new param();
			element.name = name;
			element.ElementType = DED_ELEMENT_TYPE_STRUCT;
			element.value = null;
			element.length = 0;
			result = AddElement(element);
		}
		return result;
	}

	private int EncodeStructEnd(String name)
	{
		int result = -1;
		if(!name.isEmpty()) {
			param element = new param();
			element.name = name;
			element.ElementType = DED_ELEMENT_TYPE_STRUCT_END;
			element.value = null;
			element.length = 0;
			result = AddElement(element);
		}
		return result;
	}

	private int EncodeMethod(String name, byte[] value, int length)
	{
		int result = -1;
		if(!name.isEmpty()) {
			param element = new param();
			element.name = name;
			element.ElementType = DED_ELEMENT_TYPE_METHOD;
			element.value = value;
			element.length = length;
			result = AddElement(element);
		}
		return result;
	}

	private int EncodeUShort(String name, byte[] value, int length)
	{
		int result = -1;
		if(!name.isEmpty()) {
			param element = new param();
			element.name = name;
			element.ElementType = DED_ELEMENT_TYPE_USHORT;
			element.value = value;
			element.length = length;
			result = AddElement(element);
		}
		return result;
	}


	private int EncodeLong(String name, long value, int length)
	{
		int result = -1;
		if(!name.isEmpty()) {
			param element = new param();
			element.name = name;
			element.ElementType = DED_ELEMENT_TYPE_LONG;
			element.value = byteUtils.longToBytes(value);
			element.length = length;
			result = AddElement(element);
		}
		return result;
	}

	private int EncodeBool(String name, boolean value, int length)
	{
		int result = -1;
		if(!name.isEmpty()) {
			param element = new param();
			element.name = name;
			element.ElementType = DED_ELEMENT_TYPE_BOOL;
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
		if(!name.isEmpty()) {
			param element = new param();
			element.name = name;
			element.ElementType = DED_ELEMENT_TYPE_STDSTRING;
			element.value = value.getBytes();
			element.length = length;
			result = AddElement(element);
		}
		return result;
	}

	private int Encodestdvector(String name, byte[] value, int length)
	{
		int result = -1;
		if(name.isEmpty()) {
			param element = new param();
			element.name = name;
			element.ElementType = DED_ELEMENT_TYPE_STDVECTOR;
			element.value = value;
			element.length = length;
			result = AddElement(element);
		}
		return result;
	}

	class _Elements
	{
		String strElementID;
		byte[] ElementData;
	}

	private int EncodeElement(String entityname, String elementname, byte[] elementvalue)
	{
		int result;
		_Elements element = new _Elements();
		element.strElementID = elementname;
		element.ElementData = elementvalue;
		String strentity_chunk_id = entityname.toLowerCase() + "_chunk_id";
		String strentity_chunk_data = entityname.toLowerCase() + "_chunk_data";

		result = EncodeStdString(strentity_chunk_id, element.strElementID, element.strElementID.length()); // key of particular item
		if(result != -1)
			result = Encodestdvector(strentity_chunk_data, element.ElementData, elementvalue.length); //
		return result;
	}

    class _DEDobject
	{
       DEDEncoder encoder_ptr;
       byte[] uncompresseddata;
       long iLengthOfTotalData;
       byte[] pCompressedData;
       long sizeofCompressedData;
    }

	private byte[] DataEncoder_GetData(DEDobject object)
	{
		byte[] result = null;
		if (ptotaldata.length > 0) {
			object.iLengthOfTotalData = iLengthOfTotalData;
			result = ptotaldata;
		}
		return result;
	}


	//////////////////////////////////////////////
	private static final byte LF = '\n';
	private static final byte COLON = ':';
	private final Log logger = LogFactory.getLog(DEDEncoder.class);

	/**
	 * Encodes the given DED {@code message} into a {@code byte[]}
	 * @param message the message to encode
	 * @return the encoded message
	 */
	public byte[] encode(Message<byte[]> message) {
		return encode(message.getHeaders(), message.getPayload());
	}

	/**
	 * Encodes the given payload and headers into a {@code byte[]}.
	 * @param headers the headers
	 * @param payload the payload
	 * @return the encoded message
	 */
	public byte[] encode(Map<String, Object> headers, byte[] payload) {
		Assert.notNull(headers, "'headers' is required");
		Assert.notNull(payload, "'payload' is required");

		try {
			ByteArrayOutputStream baos = new ByteArrayOutputStream(128 + payload.length);
			DataOutputStream output = new DataOutputStream(baos);

			if (SimpMessageType.HEARTBEAT.equals(SimpMessageHeaderAccessor.getMessageType(headers))) {
				if (logger.isTraceEnabled()) {
					logger.trace("Encoding heartbeat");
				}
				output.write(DEDDecoder.HEARTBEAT_PAYLOAD);
			}
			else {
				DEDCommand command = DEDHeaderAccessor.getCommand(headers);
				Assert.notNull(command, "Missing DED command: " + headers);
				output.write(command.toString().getBytes(DEDDecoder.UTF8_CHARSET));
				output.write(LF);
				writeHeaders(command, headers, payload, output);
				output.write(LF);
				writeBody(payload, output);
				output.write((byte) 0);
			}

			return baos.toByteArray();
		}
		catch (IOException ex) {
			throw new DEDConversionException("Failed to encode DED frame, headers=" + headers,  ex);
		}
	}

	private void writeHeaders(DEDCommand command, Map<String, Object> headers, byte[] payload, DataOutputStream output)
			throws IOException {

		@SuppressWarnings("unchecked")
		Map<String,List<String>> nativeHeaders =
				(Map<String, List<String>>) headers.get(NativeMessageHeaderAccessor.NATIVE_HEADERS);

		if (logger.isTraceEnabled()) {
			logger.trace("Encoding DED " + command + ", headers=" + nativeHeaders);
		}

		if (nativeHeaders == null) {
			return;
		}

		boolean shouldEscape = (command != DEDCommand.CONNECT && command != DEDCommand.CONNECTED);

		for (Entry<String, List<String>> entry : nativeHeaders.entrySet()) {
			byte[] key = encodeHeaderString(entry.getKey(), shouldEscape);
			if (command.requiresContentLength() && "content-length".equals(entry.getKey())) {
				continue;
			}
			List<String> values = entry.getValue();
			if (DEDCommand.CONNECT.equals(command) &&
					DEDHeaderAccessor.DED_PASSCODE_HEADER.equals(entry.getKey())) {
				values = Arrays.asList(DEDHeaderAccessor.getPasscode(headers));
			}
			for (String value : values) {
				output.write(key);
				output.write(COLON);
				output.write(encodeHeaderString(value, shouldEscape));
				output.write(LF);
			}
		}
		if (command.requiresContentLength()) {
			int contentLength = payload.length;
			output.write("content-length:".getBytes(DEDDecoder.UTF8_CHARSET));
			output.write(Integer.toString(contentLength).getBytes(DEDDecoder.UTF8_CHARSET));
			output.write(LF);
		}
	}

	private byte[] encodeHeaderString(String input, boolean escape) {
		String inputToUse = (escape ? escape(input) : input);
		return inputToUse.getBytes(DEDDecoder.UTF8_CHARSET);
	}

	/**
	 * handling escape chars
	 */
	//TODO: escape is perhaps not needed for DED frame handling
	private String escape(String inString) {
		StringBuilder sb = new StringBuilder(inString.length());
		for (int i = 0; i < inString.length(); i++) {
			char c = inString.charAt(i);
			if (c == '\\') {
				sb.append("\\\\");
			}
			else if (c == ':') {
				sb.append("\\c");
			}
			else if (c == '\n') {
				 sb.append("\\n");
			}
			else if (c == '\r') {
				sb.append("\\r");
			}
			else {
				sb.append(c);
			}
		}
		return sb.toString();
	}

	private void writeBody(byte[] payload, DataOutputStream output) throws IOException {
		output.write(payload);
	}

}


