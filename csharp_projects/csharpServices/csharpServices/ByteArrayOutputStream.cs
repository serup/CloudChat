﻿using System;

namespace csharpServices
{
	public class ByteArrayOutputStream : OutputStream 
	{
		/**
	     * The buffer where data is stored.
	     */
	    protected byte[] buf;

	    /**
	     * The number of valid bytes in the buffer.
	     */
	    protected int count;

	    /**
	     * Creates a new byte array output stream. The buffer capacity is
	     * initially 32 bytes, though its size increases if necessary.
	     */
	    public ByteArrayOutputStream() {
			buf = new byte[32];
	    }

	    /**
	     * Creates a new byte array output stream, with a buffer capacity of
	     * the specified size, in bytes.
	     *
	     * @param   size   the initial size.
	     * @exception  IllegalArgumentException if size is negative.
	     */
	    public ByteArrayOutputStream(int size) {
	        if (size < 0) {
	            throw new ArgumentException("Negative initial size: " + size.ToString());
	        }
	        buf = new byte[size];
	    }

		/**
	     * Increases the capacity if necessary to ensure that it can hold
	     * at least the number of elements specified by the minimum
	     * capacity argument.
	     *
	     * @param minCapacity the desired minimum capacity
	     * @throws OutOfMemoryError if {@code minCapacity < 0}.  This is
	     * interpreted as a request for the unsatisfiably large capacity
	     * {@code (long) Integer.MAX_VALUE + (minCapacity - Integer.MAX_VALUE)}.
	     */
	    private void ensureCapacity(int minCapacity) {
	        // overflow-conscious code
	        if (minCapacity - buf.Length > 0)
	            grow(minCapacity);
	    }

		static int   IntegerMAX_VALUE = 0x7fffffff; // Integer

		/**
	     * The maximum size of array to allocate.
	     * Some VMs reserve some header words in an array.
	     * Attempts to allocate larger arrays may result in
	     * OutOfMemoryError: Requested array size exceeds VM limit
	     */
	    private static int MAX_ARRAY_SIZE = IntegerMAX_VALUE - 8;

	    /**
	     * Increases the capacity to ensure that it can hold at least the
	     * number of elements specified by the minimum capacity argument.
	     *
	     * @param minCapacity the desired minimum capacity
	     */
	    private void grow(int minCapacity) {
	        // overflow-conscious code
	        int oldCapacity = buf.Length;
	        int newCapacity = oldCapacity << 1;
	        if (newCapacity - minCapacity < 0)
	            newCapacity = minCapacity;
	        if (newCapacity - MAX_ARRAY_SIZE > 0)
	            newCapacity = hugeCapacity(minCapacity);
	        buf = Arrays.copyOf(buf, newCapacity);
	    }

		private static int hugeCapacity(int minCapacity) {
	        if (minCapacity < 0) // overflow
	            throw new OutOfMemoryException();
	        return (minCapacity > MAX_ARRAY_SIZE) ?
	            IntegerMAX_VALUE :
	            MAX_ARRAY_SIZE;
	    }

	    /**
	     * Writes the specified byte to this byte array output stream.
	     *
	     * @param   b   the byte to be written.
	     */
	    public override void write(int b) {
	        ensureCapacity(count + 1);
	        buf[count] = (byte) b;
	        count += 1;
	    }

		/**
	     * Writes <code>len</code> bytes from the specified byte array
	     * starting at offset <code>off</code> to this byte array output stream.
	     *
	     * @param   b     the data.
	     * @param   off   the start offset in the data.
	     * @param   len   the number of bytes to write.
	     */
	    public void write(byte[] b, int off, int len) {
	        if ((off < 0) || (off > b.Length) || (len < 0) ||
	            ((off + len) - b.Length > 0)) {
	            throw new  IndexOutOfRangeException();
	        }
	        ensureCapacity(count + len);
			System.Array.Copy(b, off, buf, count, len);
	        count += len;
	    }

		/**
	     * Writes the complete contents of this byte array output stream to
	     * the specified output stream argument, as if by calling the output
	     * stream's write method using <code>out.write(buf, 0, count)</code>.
	     *
	     * @param      out   the output stream to which to write the data.
	     */
	    public void writeTo(OutputStream __out) {
	        __out.write(buf, 0, count);
	    }

	    /**
	     * Resets the <code>count</code> field of this byte array output
	     * stream to zero, so that all currently accumulated output in the
	     * output stream is discarded. The output stream can be used again,
	     * reusing the already allocated buffer space.
	     *
	     */
	    public void reset() {
	        count = 0;
	    }

	    /**
	     * Creates a newly allocated byte array. Its size is the current
	     * size of this output stream and the valid contents of the buffer
	     * have been copied into it.
	     *
	     * @return  the current contents of this output stream, as a byte array.
	     */
	    public byte[] toByteArray() {
	        return Arrays.copyOf(buf, count);
	    }

	    /**
	     * Returns the current size of the buffer.
	     *
	     * @return  the value of the <code>count</code> field, which is the number
	     *          of valid bytes in this output stream.
	     */
	    public int size() {
	        return this.count;
	    }

	    /**
	     * Converts the buffer's contents into a string decoding bytes using the
	     * platform's default character set. The length of the new <tt>String</tt>
	     * is a function of the character set, and hence may not be equal to the
	     * size of the buffer.
	     *
	     * <p> This method always replaces malformed-input and unmappable-character
	     * sequences with the default replacement string for the platform's
	     * default character set. 
	     *
	     * @return String decoded from the buffer's contents.
	     */
	    public String toString() {
			byte[] array = new byte[this.count];
			System.Array.Copy(this.buf, array, this.count);
			String result = System.Text.Encoding.UTF8.GetString(array);
			//return this.buf.ToString(); // somehow fails
			return result;
	    }

	    /**
	     * Closing a <tt>ByteArrayOutputStream</tt> has no effect. The methods in
	     * this class can be called after the stream has been closed without
	     * generating an <tt>IOException</tt>.
	     */
	    public void close() {
	    }

	}
}

