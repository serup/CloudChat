//TODO: change to fit DED ASN1 Okumura compress
/*
 * Copyright 2014-2016 SCANVA ApS
 *
 * This Encoder follows same structure as the Encoder for STOMP, however it is NOT the same
 * - DED is a completely different protocol
 */

package messaging.simp.ded;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import org.springframework.messaging.Message;
import org.springframework.messaging.simp.SimpMessageHeaderAccessor;
import org.springframework.messaging.simp.SimpMessageType;
import org.springframework.messaging.support.NativeMessageHeaderAccessor;
import org.springframework.util.Assert;

/*
//TODO: make functions for encoding data - example
        unsigned short trans_id = 1;
		bool action = true;

		DED_START_ENCODER(encoder_ptr);
		DED_PUT_STRUCT_START( encoder_ptr, "event" );
		  DED_PUT_METHOD  ( encoder_ptr, "name",  "MusicPlayer" );
		  DED_PUT_USHORT  ( encoder_ptr, "trans_id",      trans_id);
		  DED_PUT_BOOL    ( encoder_ptr, "startstop", action );
		DED_PUT_STRUCT_END( encoder_ptr, "event" );
*/



/**
 * An encoder for DED frames.
 *
 * @author Johnny Serup
 */
public class DEDEncoder  {

	/**
	 * Functions / methods for the decoder
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
		return  dedEncoder;
	}

    public static int PUT_STRUCT_START(DEDEncoder dedEncoder, String name)
	{
		int result=-1;
		if(dedEncoder != null)
			result = dedEncoder.EncodeStructStart(name);
		return result;
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

		class asn
		{
			int Length;
			int Tag;
			byte[] data;
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

		class data
		{
			int Length;
			byte[] data;
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

	/**
	 * Element types
	 */
	public static final int DED_ELEMENT_TYPE_STRUCT = 14;


	class param
	{
		public String name;
		public int ElementType;
	}

	private int EncodeStructStart(String name)
	{
		int result=-1;
		if(name.isEmpty()) {
			param element = new param();
			element.name = name;
			element.ElementType = DED_ELEMENT_TYPE_STRUCT;
			result = AddElement(element);
		}
		return result;
	}

	/**
	 * General function for adding elements to array as ASN1 elements
	 *
	 * @param element
	 * @return
     */
	private int AddElement(param element)
	{
		int result=-1;
        if (element.ElementType == DED_ELEMENT_TYPE_STRUCT)
		{
			// First element in structure
			int LengthOfAsn1 = element.name.length();

		}
		return result;
	}

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


