//TODO: change to fit DED ASN1 Okumura compress
/*
 * Copyright 2014-2016 SCANVA ApS
 *
 * This Decoder follows same structure as the Decoder for STOMP, however it is NOT the same
 * - DED is a completely different protocol
 */

package messaging.simp.ded;

import java.io.ByteArrayOutputStream;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import org.springframework.messaging.Message;
import org.springframework.messaging.support.MessageBuilder;
import org.springframework.messaging.support.MessageHeaderInitializer;
import org.springframework.messaging.support.NativeMessageHeaderAccessor;
import org.springframework.util.InvalidMimeTypeException;
import org.springframework.util.MultiValueMap;

/**
 * Decodes one or more DED frames contained in a {@link ByteBuffer}.
 *
 * <p>An attempt is made to read all complete DED frames from the buffer, which
 * could be zero, one, or more.
 * The caller is responsible for dealing with incomplete content buffering until there is more input available.
 *
 * @author Johnny Serup
 */
public class DEDDecoder {

	public class ByteUtils {
		private ByteBuffer buffer = ByteBuffer.allocate(Long.BYTES);

		public byte[] longToBytes(long x) {
			buffer.putLong(0, x);
			return buffer.array();
		}

		public long bytesToLong(byte[] bytes) {
			buffer.put(bytes, 0, bytes.length);
			buffer.flip();//need flip
			return buffer.getLong();
		}
	}

	public ByteUtils byteUtils;
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

	class _Elements
	{
		String strElementID;
		byte[] ElementData;
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
	CASN1 m_asn1; // used in decoder

	byte[] decompress_lzss(byte[] pCompressedData, int sizeofCompressedData)
	{
		byte[] result=null;
		//TODO: implement decompression
		return result;
	}


	private DEDDecoder PUT_DATA_IN_DECODER(byte[] pCompressedData, int sizeofCompressedData)
	{
		DEDDecoder decoder_ptr = new DEDDecoder();

		byte[] decmprsd = decompress_lzss(pCompressedData, sizeofCompressedData);
		if (decmprsd.length > 0) {
			decoder_ptr.ptotaldata = decmprsd;
			ptotaldata = decoder_ptr.ptotaldata;
			decoder_ptr.pdata = ptotaldata;
			decoder_ptr.iLengthOfTotalData = decmprsd.length;
			iLengthOfTotalData = decoder_ptr.iLengthOfTotalData;
		}
		return decoder_ptr;
	}


	private int GET_STRUCT_START(DEDDecoder decoder_ptr, String name)
	{
		int result = -1;
		if(!name.isEmpty()) {
			param DEDobject = new param();
			DEDobject.name = name;
			DEDobject.ElementType = DED_ELEMENT_TYPE_STRUCT;
			DEDobject.value = null;
			DEDobject.length = 0;
			result = decoder_ptr._GetElement(DEDobject);
		}
		return result;
	}

	private String GET_METHOD(DEDDecoder decoder_ptr, String name)
	{
		String result="##unknown##";
		if(!name.isEmpty()) {
			param DEDobject = new param();
			DEDobject.name = name;
			DEDobject.ElementType = DED_ELEMENT_TYPE_METHOD;
			DEDobject.value = null;
			DEDobject.length = 0;
			int found = decoder_ptr._GetElement(DEDobject);
			if (found == 1)
				result = DEDobject.value.toString();
		}
		return result;
	}

	private short GET_USHORT(DEDDecoder decoder_ptr, String name)
	{
		short result = -1;
		if(!name.isEmpty()) {
			param DEDobject = new param();
			DEDobject.name = name;
			DEDobject.ElementType = DED_ELEMENT_TYPE_USHORT;
			DEDobject.value = null;
			DEDobject.length = 0;
			int found = decoder_ptr._GetElement(DEDobject);
			if (found == 1)
				result = (short)byteUtils.bytesToLong(DEDobject.value);
		}
		return result;
	}

	private long GET_LONG(DEDDecoder decoder_ptr, String name)
	{
		long result = -1;
		if(!name.isEmpty()) {
			param DEDobject = new param();
			DEDobject.name = name;
			DEDobject.ElementType = DED_ELEMENT_TYPE_LONG;
			DEDobject.value = null;
			DEDobject.length = 0;
			int found = decoder_ptr._GetElement(DEDobject);
			if (found == 1)
				result = byteUtils.bytesToLong(DEDobject.value);
		}
		return result;
	}

	private boolean GET_BOOL(DEDDecoder decoder_ptr, String name)
	{
		boolean result = false;
		if(!name.isEmpty()) {
			param DEDobject = new param();
			DEDobject.name = name;
			DEDobject.ElementType = DED_ELEMENT_TYPE_BOOL;
			DEDobject.value = null;
			DEDobject.length = 0;
			int found = decoder_ptr._GetElement(DEDobject);
			if (found == 1) {
				if((int)byteUtils.bytesToLong(DEDobject.value)!=0)
					result=true;
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

	private String GET_STDSTRING(DEDDecoder decoder_ptr, String name)
	{
		String result = "";
		if(!name.isEmpty()) {
			param DEDobject = new param();
			DEDobject.name = name;
			DEDobject.ElementType = DED_ELEMENT_TYPE_STDSTRING;
			DEDobject.value = null;
			DEDobject.length = -1;
			int found = decoder_ptr._GetElement(DEDobject);
			if (found == 1)
				result = DEDobject.value.toString();
			result = emptycheck(result);
		}
		return result;
	}


	//TODO: 20140724 consider designing _GET_ so that if element is NOT found, then internal pointer is NOT moved as it is NOW!!!
	private int GET_ELEMENT(DEDDecoder decoder_ptr, String entityname, _Elements elementvalue)
	{
		int result = -1;

		String strentity_chunk_id = entityname.toLowerCase() + "_chunk_id";
		String strentity_chunk_data = entityname.toLowerCase() + "_chunk_data";

		param DEDobject1 = new param();
		DEDobject1.name = strentity_chunk_id;
		DEDobject1.ElementType = DED_ELEMENT_TYPE_STDSTRING;
		DEDobject1.value = null;
		DEDobject1.length = -1;

		param DEDobject2 = new param();
		DEDobject2.name = strentity_chunk_data;
		DEDobject2.ElementType = DED_ELEMENT_TYPE_STDVECTOR;
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
		elementvalue.strElementID = DEDobject1.value.toString();
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
				String strCmp;
				strCmp = param.data.toString();
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
									String str;
									str = param.data.toString();
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

	private int GET_STRUCT_END(DEDDecoder decoder_ptr, String name)
	{
		int result = -1;
		param DEDobject = new param();
		DEDobject.name = name;
		DEDobject.ElementType = DED_ELEMENT_TYPE_STRUCT_END;
		DEDobject.value = null;
		DEDobject.length = -1;
		result = decoder_ptr._GetElement(DEDobject);
		return result;
	}



	/////////////////////////////////////////////////////////

	static final Charset UTF8_CHARSET = Charset.forName("UTF-8");
	static final byte[] HEARTBEAT_PAYLOAD = new byte[] {'\n'};
	private static final Log logger = LogFactory.getLog(DEDDecoder.class);
	private MessageHeaderInitializer headerInitializer;

	/**
	 * Configure a {@link MessageHeaderInitializer} to apply to the headers of
	 * {@link Message}s from decoded DED frames.
	 */
	public void setHeaderInitializer(MessageHeaderInitializer headerInitializer) {
		this.headerInitializer = headerInitializer;
	}

	/**
	 * Return the configured {@code MessageHeaderInitializer}, if any.
	 */
	public MessageHeaderInitializer getHeaderInitializer() {
		return this.headerInitializer;
	}


	/**
	 * Decodes one or more DED frames from the given {@code ByteBuffer} into a
	 * list of {@link Message}s. If the input buffer contains partial DED frame
	 * content, or additional content with a partial DED frame, the buffer is
	 * reset and {@code null} is returned.
	 * @param buffer the buffer to decode the DED frame from
	 * @return the decoded messages, or an empty list if none
	 * @throws DEDConversionException raised in case of decoding issues
	 */
	public List<Message<byte[]>> decode(ByteBuffer buffer) {
		return decode(buffer, null);
	}

	/**
	 * Decodes one or more DED frames from the given {@code buffer} and returns
	 * a list of {@link Message}s.
	 * <p>If the given ByteBuffer contains only partial DED frame content and no
	 * complete DED frames, an empty list is returned, and the buffer is reset to
	 * to where it was.
	 * <p>If the buffer contains one ore more DED frames, those are returned and
	 * the buffer reset to point to the beginning of the unused partial content.
	 * <p>The output partialMessageHeaders map is used to store successfully parsed
	 * headers in case of partial content. The caller can then check if a
	 * "content-length" header was read, which helps to determine how much more
	 * content is needed before the next attempt to decode.
	 * @param buffer the buffer to decode the DED frame from
	 * @param partialMessageHeaders an empty output map that will store the last
	 * successfully parsed partialMessageHeaders in case of partial message content
	 * in cases where the partial buffer ended with a partial DED frame
	 * @return the decoded messages, or an empty list if none
	 * @throws DEDConversionException raised in case of decoding issues
	 */
	public List<Message<byte[]>> decode(ByteBuffer buffer, MultiValueMap<String, String> partialMessageHeaders) {
		List<Message<byte[]>> messages = new ArrayList<Message<byte[]>>();
		while (buffer.hasRemaining()) {
			Message<byte[]> message = decodeMessage(buffer, partialMessageHeaders);
			if (message != null) {
				messages.add(message);
			}
			else {
				break;
			}
		}
		return messages;
	}

	/**
	 * Decode a single DED frame from the given {@code buffer} into a {@link Message}.
	 */
	private Message<byte[]> decodeMessage(ByteBuffer buffer, MultiValueMap<String, String> headers) {
		Message<byte[]> decodedMessage = null;
		skipLeadingEol(buffer);
		buffer.mark();

		String command = readCommand(buffer);
		if (command.length() > 0) {
			DEDHeaderAccessor headerAccessor = null;
			byte[] payload = null;
			if (buffer.remaining() > 0) {
				DEDCommand dedCommand = DEDCommand.valueOf(command);
				headerAccessor = DEDHeaderAccessor.create(dedCommand);
				initHeaders(headerAccessor);
				readHeaders(buffer, headerAccessor);
				payload = readPayload(buffer, headerAccessor);
			}
			if (payload != null) {
				if (payload.length > 0 && !headerAccessor.getCommand().isBodyAllowed()) {
					throw new DEDConversionException(headerAccessor.getCommand() +
							" shouldn't have a payload: length=" + payload.length + ", headers=" + headers);
				}
				headerAccessor.updateSimpMessageHeadersFromDEDHeaders();
				headerAccessor.setLeaveMutable(true);
				decodedMessage = MessageBuilder.createMessage(payload, headerAccessor.getMessageHeaders());
				if (logger.isTraceEnabled()) {
					logger.trace("Decoded " + headerAccessor.getDetailedLogMessage(payload));
				}
			}
			else {
				if (logger.isTraceEnabled()) {
					logger.trace("Incomplete frame, resetting input buffer...");
				}
				if (headers != null && headerAccessor != null) {
					String name = NativeMessageHeaderAccessor.NATIVE_HEADERS;
					@SuppressWarnings("unchecked")
					MultiValueMap<String, String> map = (MultiValueMap<String, String>) headerAccessor.getHeader(name);
					if (map != null) {
						headers.putAll(map);
					}
				}
				buffer.reset();
			}
		}
		else {
			DEDHeaderAccessor headerAccessor = DEDHeaderAccessor.createForHeartbeat();
			initHeaders(headerAccessor);
			headerAccessor.setLeaveMutable(true);
			decodedMessage = MessageBuilder.createMessage(HEARTBEAT_PAYLOAD, headerAccessor.getMessageHeaders());
			if (logger.isTraceEnabled()) {
				logger.trace("Decoded " + headerAccessor.getDetailedLogMessage(null));
			}
		}

		return decodedMessage;
	}

	private void initHeaders(DEDHeaderAccessor headerAccessor) {
		MessageHeaderInitializer initializer = getHeaderInitializer();
		if (initializer != null) {
			initializer.initHeaders(headerAccessor);
		}
	}

	/**
	 * Skip one ore more EOL characters at the start of the given ByteBuffer.
	 * Those are DED heartbeat frames.
	 */
	protected void skipLeadingEol(ByteBuffer buffer) {
		while (true) {
			if (!tryConsumeEndOfLine(buffer)) {
				break;
			}
		}
	}

	private String readCommand(ByteBuffer buffer) {
		ByteArrayOutputStream command = new ByteArrayOutputStream(256);
		while (buffer.remaining() > 0 && !tryConsumeEndOfLine(buffer)) {
			command.write(buffer.get());
		}
		return new String(command.toByteArray(), UTF8_CHARSET);
	}

	private void readHeaders(ByteBuffer buffer, DEDHeaderAccessor headerAccessor) {
		while (true) {
			ByteArrayOutputStream headerStream = new ByteArrayOutputStream(256);
			boolean headerComplete = false;
			while (buffer.hasRemaining()) {
				if (tryConsumeEndOfLine(buffer)) {
					headerComplete = true;
					break;
				}
				headerStream.write(buffer.get());
			}
			if (headerStream.size() > 0 && headerComplete) {
				String header = new String(headerStream.toByteArray(), UTF8_CHARSET);
				int colonIndex = header.indexOf(':');
				if (colonIndex <= 0) {
					if (buffer.remaining() > 0) {
						throw new DEDConversionException("Illegal header: '" + header +
								"'. A header must be of the form <name>:[<value>].");
					}
				}
				else {
					String headerName = unescape(header.substring(0, colonIndex));
					String headerValue = unescape(header.substring(colonIndex + 1));
					try {
						headerAccessor.addNativeHeader(headerName, headerValue);
					}
					catch (InvalidMimeTypeException ex) {
						if (buffer.remaining() > 0) {
							throw ex;
						}
					}
				}
			}
			else {
				break;
			}
		}
	}

	/**
	 * handling escape chars
	 */
	//TODO: unescape is perhaps not needed for DED frame handling
	private String unescape(String inString) {
		StringBuilder sb = new StringBuilder(inString.length());
		int pos = 0;  // position in the old string
		int index = inString.indexOf("\\");

		while (index >= 0) {
			sb.append(inString.substring(pos, index));
			if (index + 1 >= inString.length()) {
				throw new DEDConversionException("Illegal escape sequence at index " + index + ": " + inString);
			}
			Character c = inString.charAt(index + 1);
			if (c == 'r') {
				sb.append('\r');
			}
			else if (c == 'n') {
				sb.append('\n');
			}
			else if (c == 'c') {
				sb.append(':');
			}
			else if (c == '\\') {
				sb.append('\\');
			}
			else {
				// should never happen
				throw new DEDConversionException("Illegal escape sequence at index " + index + ": " + inString);
			}
			pos = index + 2;
			index = inString.indexOf("\\", pos);
		}

		sb.append(inString.substring(pos));
		return sb.toString();
	}

	private byte[] readPayload(ByteBuffer buffer, DEDHeaderAccessor headerAccessor) {
		Integer contentLength;
		try {
			contentLength = headerAccessor.getContentLength();
		}
		catch (NumberFormatException ex) {
			logger.warn("Ignoring invalid content-length: '" + headerAccessor);
			contentLength = null;
		}

		if (contentLength != null && contentLength >= 0) {
			if (buffer.remaining() > contentLength) {
				byte[] payload = new byte[contentLength];
				buffer.get(payload);
				if (buffer.get() != 0) {
					throw new DEDConversionException("Frame must be terminated with a null octet");
				}
				return payload;
			}
			else {
				return null;
			}
		}
		else {
			ByteArrayOutputStream payload = new ByteArrayOutputStream(256);
			while (buffer.remaining() > 0) {
				byte b = buffer.get();
				if (b == 0) {
					return payload.toByteArray();
				}
				else {
					payload.write(b);
				}
			}
		}
		return null;
	}

	/**
	 * Try to read an EOL incrementing the buffer position if successful.
	 * @return whether an EOL was consumed
	 */
	private boolean tryConsumeEndOfLine(ByteBuffer buffer) {
		if (buffer.remaining() > 0) {
			byte b = buffer.get();
			if (b == '\n') {
				return true;
			}
			else if (b == '\r') {
				if (buffer.remaining() > 0 && buffer.get() == '\n') {
					return true;
				}
				else {
					throw new DEDConversionException("'\\r' must be followed by '\\n'");
				}
			}
			buffer.position(buffer.position() - 1);
		}
		return false;
	}

}
