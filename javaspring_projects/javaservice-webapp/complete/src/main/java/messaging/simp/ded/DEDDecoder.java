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
