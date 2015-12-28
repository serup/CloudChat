//TODO: change to fit DED ASN1 Okumura compress
/*
 * Copyright 2014-2016 SCANVA ApS
 *
 * This Encoder / Decoder follows same structure as the Encoder for STOMP, however it is NOT the same
 * - DED is a completely different protocol
 */

package messaging.simp.ded;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import org.springframework.messaging.simp.SimpMessageType;

/**
 * Represents a DED command.
 *
 * @author Johnny Serup
 */
public enum DEDCommand {

	// client
	CONNECT,
	DED,
	DISCONNECT,
	SUBSCRIBE,
	UNSUBSCRIBE,
	SEND,
	ACK,
	NACK,
	BEGIN,
	COMMIT,
	ABORT,

	// server
	CONNECTED,
	MESSAGE,
	RECEIPT,
	ERROR;


	private static Map<DEDCommand, SimpMessageType> messageTypes = new HashMap<DEDCommand, SimpMessageType>();
	static {
		messageTypes.put(DEDCommand.CONNECT, SimpMessageType.CONNECT);
		messageTypes.put(DEDCommand.DED, SimpMessageType.CONNECT);
		messageTypes.put(DEDCommand.SEND, SimpMessageType.MESSAGE);
		messageTypes.put(DEDCommand.MESSAGE, SimpMessageType.MESSAGE);
		messageTypes.put(DEDCommand.SUBSCRIBE, SimpMessageType.SUBSCRIBE);
		messageTypes.put(DEDCommand.UNSUBSCRIBE, SimpMessageType.UNSUBSCRIBE);
		messageTypes.put(DEDCommand.DISCONNECT, SimpMessageType.DISCONNECT);
	}

	private static Collection<DEDCommand> destinationRequired = Arrays.asList(SEND, SUBSCRIBE, MESSAGE);
	private static Collection<DEDCommand> subscriptionIdRequired = Arrays.asList(SUBSCRIBE, UNSUBSCRIBE, MESSAGE);
	private static Collection<DEDCommand> contentLengthRequired = Arrays.asList(SEND, MESSAGE, ERROR);
	private static Collection<DEDCommand> bodyAllowed = Arrays.asList(SEND, MESSAGE, ERROR);



	public SimpMessageType getMessageType() {
		SimpMessageType type = messageTypes.get(this);
		return (type != null) ? type : SimpMessageType.OTHER;
	}

	public boolean requiresDestination() {
		return destinationRequired.contains(this);
	}

	public boolean requiresSubscriptionId() {
		return subscriptionIdRequired.contains(this);
	}

	public boolean requiresContentLength() {
		return contentLengthRequired.contains(this);
	}

	public boolean isBodyAllowed() {
		return bodyAllowed.contains(this);
	}

}

