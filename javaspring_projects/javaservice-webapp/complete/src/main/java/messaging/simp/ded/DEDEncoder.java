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

/**
 * An encoder for DED frames.
 *
 * @author Johnny Serup
 */
public final class DEDEncoder  {

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
