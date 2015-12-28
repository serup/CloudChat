//TODO: change to fit DED ASN1 Okumura compress
/*
 * Copyright 2014-2016 SCANVA ApS
 *
 * This Encoder/Decoder follows same structure as the Encoder for STOMP, however it is NOT the same
 * - DED is a completely different protocol
 */

package messaging.simp.ded;

import java.nio.charset.Charset;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.atomic.AtomicLong;

import org.springframework.messaging.Message;
import org.springframework.messaging.simp.SimpMessageHeaderAccessor;
import org.springframework.messaging.simp.SimpMessageType;
import org.springframework.messaging.support.MessageHeaderAccessor;
import org.springframework.util.Assert;
import org.springframework.util.MimeType;
import org.springframework.util.MimeTypeUtils;
import org.springframework.util.StringUtils;

/**
 * A {@code MessageHeaderAccessor} to use when creating a {@code Message} from a
 * decoded DED frame, or when encoding a {@code Message} to a DED frame.
 *
 * <p>When created from DED frame content, the actual DED headers are stored
 * in the native header sub-map managed by the parent class
 * {@link org.springframework.messaging.support.NativeMessageHeaderAccessor}
 * while the parent class
 * {@link org.springframework.messaging.simp.SimpMessageHeaderAccessor} manages
 * common processing headers some of which are based on DED headers (e.g.
 * destination, content-type, etc).
 *
 * <p>An instance of this class can also be created by wrapping an existing
 * {@code Message}. That message may have been created with the more generic
 * {@link org.springframework.messaging.simp.SimpMessageHeaderAccessor} in
 * which case DED headers are created from common processing headers.
 * In this case it is also necessary to invoke either
 * {@link #updateDEDCommandAsClientMessage()} or
 * {@link #updateDEDCommandAsServerMessage()} if sending a message and
 * depending on whether a message is sent to a client or the message broker.
 *
 */
//TODO: message headers perhaps not needed for DED, since DED is not a communication handling protocol, only a protocol for encoding / decoding data
public class DEDHeaderAccessor extends SimpMessageHeaderAccessor {

	private static final AtomicLong messageIdCounter = new AtomicLong();

	private static final long[] DEFAULT_HEARTBEAT = new long[] {0, 0};


	// DED header names

	public static final String DED_ID_HEADER = "id";

	public static final String DED_HOST_HEADER = "host";

	public static final String DED_ACCEPT_VERSION_HEADER = "accept-version";

	public static final String DED_MESSAGE_ID_HEADER = "message-id";

	public static final String DED_RECEIPT_HEADER = "receipt"; // any client frame except CONNECT

	public static final String DED_RECEIPT_ID_HEADER = "receipt-id"; // RECEIPT frame

	public static final String DED_SUBSCRIPTION_HEADER = "subscription";

	public static final String DED_VERSION_HEADER = "version";

	public static final String DED_MESSAGE_HEADER = "message";

	public static final String DED_ACK_HEADER = "ack";

	public static final String DED_NACK_HEADER = "nack";

	public static final String DED_LOGIN_HEADER = "login";

	public static final String DED_PASSCODE_HEADER = "passcode";

	public static final String DED_DESTINATION_HEADER = "destination";

	public static final String DED_CONTENT_TYPE_HEADER = "content-type";

	public static final String DED_CONTENT_LENGTH_HEADER = "content-length";

	public static final String DED_HEARTBEAT_HEADER = "heart-beat";

	// Other header names

	private static final String COMMAND_HEADER = "dedCommand";

	private static final String CREDENTIALS_HEADER = "dedCredentials";


	/**
	 * A constructor for creating message headers from a parsed DED frame.
	 */
	DEDHeaderAccessor(DEDCommand command, Map<String, List<String>> externalSourceHeaders) {
		super(command.getMessageType(), externalSourceHeaders);
		setHeader(COMMAND_HEADER, command);
		updateSimpMessageHeadersFromDEDHeaders();
	}

	/**
	 * A constructor for accessing and modifying existing message headers.
	 * Note that the message headers may not have been created from a DED frame
	 * but may have rather originated from using the more generic
	 * {@link org.springframework.messaging.simp.SimpMessageHeaderAccessor}.
	 */
	DEDHeaderAccessor(Message<?> message) {
		super(message);
		updateDEDHeadersFromSimpMessageHeaders();
	}

	DEDHeaderAccessor() {
		super(SimpMessageType.HEARTBEAT, null);
	}


	void updateSimpMessageHeadersFromDEDHeaders() {
		if (getNativeHeaders() == null) {
			return;
		}
		String value = getFirstNativeHeader(DED_DESTINATION_HEADER);
		if (value != null) {
			super.setDestination(value);
		}
		value = getFirstNativeHeader(DED_CONTENT_TYPE_HEADER);
		if (value != null) {
			super.setContentType(MimeTypeUtils.parseMimeType(value));
		}
		DEDCommand command = getCommand();
		if (DEDCommand.MESSAGE.equals(command)) {
			value = getFirstNativeHeader(DED_SUBSCRIPTION_HEADER);
			if (value != null) {
				super.setSubscriptionId(value);
			}
		}
		else if (DEDCommand.SUBSCRIBE.equals(command) || DEDCommand.UNSUBSCRIBE.equals(command)) {
			value = getFirstNativeHeader(DED_ID_HEADER);
			if (value != null) {
				super.setSubscriptionId(value);
			}
		}
		else if (DEDCommand.CONNECT.equals(command)) {
			protectPasscode();
		}
	}

	void updateDEDHeadersFromSimpMessageHeaders() {
		if (getDestination() != null) {
			setNativeHeader(DED_DESTINATION_HEADER, getDestination());
		}
		if (getContentType() != null) {
			setNativeHeader(DED_CONTENT_TYPE_HEADER, getContentType().toString());
		}
		trySetDEDHeaderForSubscriptionId();
	}


	@Override
	protected MessageHeaderAccessor createAccessor(Message<?> message) {
		return wrap(message);
	}

	Map<String, List<String>> getNativeHeaders() {
		@SuppressWarnings("unchecked")
		Map<String, List<String>> map = (Map<String, List<String>>) getHeader(NATIVE_HEADERS);
		return (map != null ? map : Collections.<String, List<String>>emptyMap());
	}

	public DEDCommand updateDEDCommandAsClientMessage() {
		Assert.state(SimpMessageType.MESSAGE.equals(getMessageType()), "Unexpected message type " + getMessage());
		if (getCommand() == null) {
			setHeader(COMMAND_HEADER, DEDCommand.SEND);
		}
		else if (!getCommand().equals(DEDCommand.SEND)) {
			throw new IllegalStateException("Unexpected DED command " + getCommand());
		}
		return getCommand();
	}

	public void updateDEDCommandAsServerMessage() {
		Assert.state(SimpMessageType.MESSAGE.equals(getMessageType()), "Unexpected message type " + getMessage());
		DEDCommand command = getCommand();
		if ((command == null) || DEDCommand.SEND.equals(command)) {
			setHeader(COMMAND_HEADER, DEDCommand.MESSAGE);
		}
		else if (!DEDCommand.MESSAGE.equals(command)) {
			throw new IllegalStateException("Unexpected DED command " + command);
		}
		trySetDEDHeaderForSubscriptionId();
		if (getMessageId() == null) {
			String messageId = getSessionId() + "-" + messageIdCounter.getAndIncrement();
			setNativeHeader(DED_MESSAGE_ID_HEADER, messageId);
		}
	}

	/**
	 * Return the DED command, or {@code null} if not yet set.
	 */
	public DEDCommand getCommand() {
		return (DEDCommand) getHeader(COMMAND_HEADER);
	}

	public boolean isHeartbeat() {
		return (SimpMessageType.HEARTBEAT == getMessageType());
	}

	public long[] getHeartbeat() {
		String rawValue = getFirstNativeHeader(DED_HEARTBEAT_HEADER);
		if (!StringUtils.hasText(rawValue)) {
			return Arrays.copyOf(DEFAULT_HEARTBEAT, 2);
		}
		String[] rawValues = StringUtils.commaDelimitedListToStringArray(rawValue);
		return new long[] {Long.valueOf(rawValues[0]), Long.valueOf(rawValues[1])};
	}

	public void setAcceptVersion(String acceptVersion) {
		setNativeHeader(DED_ACCEPT_VERSION_HEADER, acceptVersion);
	}

	public Set<String> getAcceptVersion() {
		String rawValue = getFirstNativeHeader(DED_ACCEPT_VERSION_HEADER);
		return (rawValue != null ? StringUtils.commaDelimitedListToSet(rawValue) : Collections.<String>emptySet());
	}

	public void setHost(String host) {
		setNativeHeader(DED_HOST_HEADER, host);
	}

	public String getHost() {
		return getFirstNativeHeader(DED_HOST_HEADER);
	}

	@Override
	public void setDestination(String destination) {
		super.setDestination(destination);
		setNativeHeader(DED_DESTINATION_HEADER, destination);
	}

	@Override
	public void setContentType(MimeType contentType) {
		super.setContentType(contentType);
		setNativeHeader(DED_CONTENT_TYPE_HEADER, contentType.toString());
	}

	@Override
	public void setSubscriptionId(String subscriptionId) {
		super.setSubscriptionId(subscriptionId);
		trySetDEDHeaderForSubscriptionId();
	}

	private void trySetDEDHeaderForSubscriptionId() {
		String subscriptionId = getSubscriptionId();
		if (subscriptionId != null) {
			if (getCommand() != null && DEDCommand.MESSAGE.equals(getCommand())) {
				setNativeHeader(DED_SUBSCRIPTION_HEADER, subscriptionId);
			}
			else {
				SimpMessageType messageType = getMessageType();
				if (SimpMessageType.SUBSCRIBE.equals(messageType) || SimpMessageType.UNSUBSCRIBE.equals(messageType)) {
					setNativeHeader(DED_ID_HEADER, subscriptionId);
				}
			}
		}
	}

	public Integer getContentLength() {
		if (containsNativeHeader(DED_CONTENT_LENGTH_HEADER)) {
			return Integer.valueOf(getFirstNativeHeader(DED_CONTENT_LENGTH_HEADER));
		}
		return null;
	}

	public void setContentLength(int contentLength) {
		setNativeHeader(DED_CONTENT_LENGTH_HEADER, String.valueOf(contentLength));
	}

	public void setHeartbeat(long cx, long cy) {
		setNativeHeader(DED_HEARTBEAT_HEADER, StringUtils.arrayToCommaDelimitedString(new Object[]{cx, cy}));
	}

	public void setAck(String ack) {
		setNativeHeader(DED_ACK_HEADER, ack);
	}

	public String getAck() {
		return getFirstNativeHeader(DED_ACK_HEADER);
	}

	public void setNack(String nack) {
		setNativeHeader(DED_NACK_HEADER, nack);
	}

	public String getNack() {
		return getFirstNativeHeader(DED_NACK_HEADER);
	}

	public void setLogin(String login) {
		setNativeHeader(DED_LOGIN_HEADER, login);
	}

	public String getLogin() {
		return getFirstNativeHeader(DED_LOGIN_HEADER);
	}

	public void setPasscode(String passcode) {
		setNativeHeader(DED_PASSCODE_HEADER, passcode);
		protectPasscode();
	}

	private void protectPasscode() {
		String value = getFirstNativeHeader(DED_PASSCODE_HEADER);
		if (value != null && !"PROTECTED".equals(value)) {
			setHeader(CREDENTIALS_HEADER, new DEDPasscode(value));
			setNativeHeader(DED_PASSCODE_HEADER, "PROTECTED");
		}
	}

	/**
	 * Return the passcode header value, or {@code null} if not set.
	 */
	public String getPasscode() {
		DEDPasscode credentials = (DEDPasscode) getHeader(CREDENTIALS_HEADER);
		return (credentials != null ? credentials.passcode : null);
	}

	public void setReceiptId(String receiptId) {
		setNativeHeader(DED_RECEIPT_ID_HEADER, receiptId);
	}

	public String getReceiptId() {
		return getFirstNativeHeader(DED_RECEIPT_ID_HEADER);
	}

	public void setReceipt(String receiptId) {
		setNativeHeader(DED_RECEIPT_HEADER, receiptId);
	}

	public String getReceipt() {
		return getFirstNativeHeader(DED_RECEIPT_HEADER);
	}

	public String getMessage() {
		return getFirstNativeHeader(DED_MESSAGE_HEADER);
	}

	public void setMessage(String content) {
		setNativeHeader(DED_MESSAGE_HEADER, content);
	}

	public String getMessageId() {
		return getFirstNativeHeader(DED_MESSAGE_ID_HEADER);
	}

	public void setMessageId(String id) {
		setNativeHeader(DED_MESSAGE_ID_HEADER, id);
	}

	public String getVersion() {
		return getFirstNativeHeader(DED_VERSION_HEADER);
	}

	public void setVersion(String version) {
		setNativeHeader(DED_VERSION_HEADER, version);
	}


	// Logging related

	@Override
	public String getShortLogMessage(Object payload) {
		if (DEDCommand.SUBSCRIBE.equals(getCommand())) {
			return "SUBSCRIBE " + getDestination() + " id=" + getSubscriptionId() + appendSession();
		}
		else if (DEDCommand.UNSUBSCRIBE.equals(getCommand())) {
			return "UNSUBSCRIBE id=" + getSubscriptionId() + appendSession();
		}
		else if (DEDCommand.SEND.equals(getCommand())) {
			return "SEND " + getDestination() + appendSession() + appendPayload(payload);
		}
		else if (DEDCommand.CONNECT.equals(getCommand())) {
			return "CONNECT" + (getUser() != null ? " user=" + getUser().getName() : "") + appendSession();
		}
		else if (DEDCommand.CONNECTED.equals(getCommand())) {
			return "CONNECTED heart-beat=" + Arrays.toString(getHeartbeat()) + appendSession();
		}
		else if (DEDCommand.DISCONNECT.equals(getCommand())) {
			return "DISCONNECT" + (getReceipt() != null ? " receipt=" + getReceipt() : "") + appendSession();
		}
		else {
			return getDetailedLogMessage(payload);
		}
	}

	@Override
	public String getDetailedLogMessage(Object payload) {
		if (isHeartbeat()) {
			return "heart-beat in session " + getSessionId();
		}
		DEDCommand command = getCommand();
		if (command == null) {
			return super.getDetailedLogMessage(payload);
		}
		StringBuilder sb = new StringBuilder();
		sb.append(command.name()).append(" ").append(getNativeHeaders()).append(appendSession());
		if (getUser() != null) {
			sb.append(", user=").append(getUser().getName());
		}
		if (command.isBodyAllowed()) {
			sb.append(appendPayload(payload));
		}
		return sb.toString();
	}

	private String appendSession() {
		return " session=" + getSessionId();
	}

	private String appendPayload(Object payload) {
		Assert.isInstanceOf(byte[].class, payload);
		byte[] bytes = (byte[]) payload;
		String contentType = (getContentType() != null ? " " + getContentType().toString() : "");
		if (bytes.length == 0 || getContentType() == null || !isReadableContentType()) {
			return contentType;
		}
		Charset charset = getContentType().getCharSet();
		charset = (charset != null ? charset : DEDDecoder.UTF8_CHARSET);
		return (bytes.length < 80) ?
				contentType + " payload=" + new String(bytes, charset) :
				contentType + " payload=" + new String(Arrays.copyOf(bytes, 80), charset) + "...(truncated)";
	}


	// Static factory methods and accessors

	/**
	 * Create an instance for the given DED command.
	 */
	public static DEDHeaderAccessor create(DEDCommand command) {
		return new DEDHeaderAccessor(command, null);
	}

	/**
	 * Create an instance for the given DED command and headers.
	 */
	public static DEDHeaderAccessor create(DEDCommand command, Map<String, List<String>> headers) {
		return new DEDHeaderAccessor(command, headers);
	}

	/**
	 * Create headers for a heartbeat. While a DED heartbeat frame does not
	 * have headers, a session id is needed for processing purposes at a minimum.
	 */
	public static DEDHeaderAccessor createForHeartbeat() {
		return new DEDHeaderAccessor();
	}

	/**
	 * Create an instance from the payload and headers of the given Message.
	 */
	public static DEDHeaderAccessor wrap(Message<?> message) {
		return new DEDHeaderAccessor(message);
	}

	/**
	 * Return the DED command from the given headers, or {@code null} if not set.
	 */
	public static DEDCommand getCommand(Map<String, Object> headers) {
		return (DEDCommand) headers.get(COMMAND_HEADER);
	}

	/**
	 * Return the passcode header value, or {@code null} if not set.
	 */
	public static String getPasscode(Map<String, Object> headers) {
		DEDPasscode credentials = (DEDPasscode) headers.get(CREDENTIALS_HEADER);
		return (credentials != null ? credentials.passcode : null);
	}

	public static Integer getContentLength(Map<String, List<String>> nativeHeaders) {
		if (nativeHeaders.containsKey(DED_CONTENT_LENGTH_HEADER)) {
			List<String> values = nativeHeaders.get(DED_CONTENT_LENGTH_HEADER);
			String value = (values != null ? values.get(0) : null);
			return Integer.valueOf(value);
		}
		return null;
	}


	private static class DEDPasscode {

		private final String passcode;

		public DEDPasscode(String passcode) {
			this.passcode = passcode;
		}

		@Override
		public String toString() {
			return "[PROTECTED]";
		}
	}

}
