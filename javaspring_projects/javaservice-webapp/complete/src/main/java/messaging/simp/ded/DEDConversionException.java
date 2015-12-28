//TODO: change to fit DED ASN1 Okumura compress
/*
 * Copyright 2014-2016 SCANVA ApS
 *
 * This Encoder / Decoder follows same structure as the Encoder for STOMP, however it is NOT the same
 * - DED is a completely different protocol
 */

package messaging.simp.ded;

import org.springframework.core.NestedRuntimeException;

/**
 * Raised after a failure to encode or decode a DED message.
 *
 */
@SuppressWarnings("serial")
public class DEDConversionException extends NestedRuntimeException {

	public DEDConversionException(String msg, Throwable cause) {
		super(msg, cause);
	}

	public DEDConversionException(String msg) {
		super(msg);
	}

}
