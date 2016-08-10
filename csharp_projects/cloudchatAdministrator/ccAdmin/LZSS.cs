﻿using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace csharpServices
{
	// working LZSS however it is not aligned with LZSS on DOPs Server - somehow compress/decompress on DOPs server is NOT a true LZSS or this is NOT a true LZSS
	class oldLZSS/*<TDataType> where TDataType : IComparable<TDataType>*/
	{
		public static float Procent;

		/// <summary>
		/// Высвобождает память
		/// </summary>
		public void Clear()
		{
			Procent = 0f;
		}

		/// <summary>
		/// Выполняет поиск указанной последовательности в словаре
		/// </summary>
		/// <param name="dictionary">Входной массив/список в котором выполняем поиск подпоследовательности</param>
		/// <param name="input">Подпоследовательность для поиска </param>
		/// <returns>Если последовательность была найдена возвращает её индекс, иначе  -1</returns>
		private static Int32 SearchInDict(IList<Byte> dictionary, IList<Byte> input)
		{
			//if (input.Count > dictionary.Count) return -1;
			for (var i = 0; i < dictionary.Count; i++)
			{
				for (var j = 0; j < input.Count && i + j < dictionary.Count; j++)
				{
					if (dictionary[i + j] == input[j])
					{
						if (j + 1 == input.Count)
							return i;
					}
					else break;
				}
			}
			return -1;
		}

		private static IEnumerable<Boolean> getBitList(Byte input)
		{
			return new BitArray(new[] { input }).Cast<Boolean>().ToList();
		}

		public Byte BitArrayToByte(BitArray ba)
		{
			Byte result = 0;
			for (Byte index = 0, m = 1; index < 8; index++, m *= 2)
				result += ba.Get(index) ? m : (Byte)0;
			return result;
		}

		public byte[] BitArrayToByteArray(BitArray bits)
		{
			byte[] ret = new byte[(bits.Length - 1) / 8 + 1];
			bits.CopyTo(ret, 0);
			return ret;
		}

		/// <summary>
		/// Сжимает входную последовательность с помощью алгоритма LZSS
		/// </summary>
		/// <param name="source">Исходный поток данных для сжатия</param>
		/// <returns></returns>
		public BitArray Compress(IList<Byte> source)
		{
			//Словарь
			var dictionary = new List<Byte>();
			//Выходной поток
			var output = new List<Boolean>();
			//Буферное окошко
			var buffer = new List<Byte>();

			for (var i = 0; i < source.Count; i++)
			{
				buffer.Add(source[i]);
				while ((SearchInDict(dictionary, buffer) != -1 && i + 1 < source.Count))
				{
					buffer.Add(source[++i]);
				}
				if (buffer.Count > 1)
				{
					buffer.RemoveAt(buffer.Count - 1);
					--i;
				}
				if (buffer.Count > 1)
				{
					output.Add(true);
					output.AddRange(getBitList((Byte)((dictionary.Count) - SearchInDict(dictionary, buffer))));
					output.AddRange(getBitList((Byte)buffer.Count));
					dictionary.AddRange(buffer);
					while (dictionary.Count > 255)
					{
						dictionary.RemoveAt(0);
					}
					buffer.Clear();
				}
				else
				{
					output.Add(false);
					output.AddRange(new BitArray(buffer.ToArray()).Cast<Boolean>().ToList());
					dictionary.AddRange(buffer);
					while (dictionary.Count > 255)
					{
						dictionary.RemoveAt(0);
					}
					buffer.Clear();
				}
				Procent = (100f / source.Count) * i;
			}
			Procent = 100;
			var countBits = new BitArray(BitConverter.GetBytes(output.Count)).Cast<Boolean>().ToList();
			output.InsertRange(0, countBits);
			return new BitArray(output.ToArray());
		}

		/// <summary>
		/// Расжимает входную последовательность
		/// </summary>
		/// <param name="source"></param>
		/// <param name="bitsCount">Колличество бит в исходном файле (за исключением мусора)</param>
		/// <returns></returns>
		public Byte[] UnCompress(BitArray source, Int32 bitsCount)
		{
			//Выходной поток
			var output = new List<Byte>();
			var tempByte = new BitArray(8);
			var bitOffset = new BitArray(8);
			var bitCount = new BitArray(8);
			for (var i = 32; i < bitsCount + 24 && i < source.Length; )
			{

				if (source[i] == false)
				{
					for (var j = 0; j < 8 && j + i + 1 < source.Length; j++)
					{
						tempByte[j] = source[++i];
					}
					output.Add(BitArrayToByte(tempByte));
					i++;
				}
				else
				{
					for (var j = 0; j < 8 && j + i + 1 < source.Length; j++)
					{
						bitOffset[j] = source[++i];
					}
					for (var j = 0; j < 8 && j + i + 1 < source.Length; j++)
					{
						bitCount[j] = source[++i];
					}
					var offset = BitArrayToByte(new BitArray(bitOffset));
					var count = BitArrayToByte(new BitArray(bitCount));
					var dicCount = output.Count;
					for (var c = 0; c < count; c++)
					{
						output.Add(output[dicCount - offset + c]);
					}
					i++;
				}
				Procent = (100f / source.Length) * i;
			}
			Procent = 100;
			return output.ToArray();
		}
	}


	// this version is a conversion from java implementation
	public class LZSS
	{

		/**
	     * This is the size of the ring buffer. It is set to 4K. It is important to
	     * note that a position within the ring buffer requires 12 bits.
	     */
	    static short RING_SIZE = 4096;

	    /**
	     * This is used to determine the next position in the ring buffer, from 0 to
	     * RING_SIZE - 1. The idiom s = (s + 1) &amp; RING_WRAP; will ensure this. This
	     * only works if RING_SIZE is a power of 2. Note this is slightly faster
	     * than the equivalent: s = (s + 1) % RING_SIZE;
	     */
	    short RING_WRAP = (short) (RING_SIZE - 1);

	    /**
	     * This is the maximum length of a character sequence that can be taken from
	     * the ring buffer. It is set to 18. Note that a length must be 3 before it
	     * is worthwhile to store a position/length pair, so the length can be
	     * encoded in only 4 bits. Or, put yet another way, it is not necessary to
	     * encode a length of 0-18, it is necessary to encode a length of 3-18,
	     * which requires 4 bits.
	     * <p>
	     * Note that the 12 bits used to store the position and the 4 bits used to
	     * store the length equal a total of 16 bits, or 2 bytes.
	     * </p>
	     */
	    int MAX_STORE_LENGTH = 18;

	    /**
	     * It takes 2 bytes to store an offset and a length. If a character sequence
	     * only requires 1 or 2 characters to store uncompressed, then it is better
	     * to store it uncompressed than as an offset into the ring buffer.
	     */
	    int THRESHOLD = 3;

	    /**
	     * Used to mark nodes as not used.
	     */
	    short NOT_USED = RING_SIZE;

	    /**
	     * A text buffer. It contains "nodes" of uncompressed text that can be
	     * indexed by position. That is, a substring of the ring buffer can be
	     * indexed by a position and a length. When decoding, the compressed text
	     * may contain a position in the ring buffer and a count of the number of
	     * bytes from the ring buffer that are to be moved into the uncompressed
	     * buffer.
	     *
	     * <p>
	     * This ring buffer is not maintained as part of the compressed text.
	     * Instead, it is reconstructed dynamically. That is, it starts out empty
	     * and gets built as the text is decompressed.
	     * </p>
	     *
	     * <p>
	     * The ring buffer contain RING_SIZE bytes, with an additional
	     * MAX_STORE_LENGTH - 1 bytes to facilitate string comparison.
	     * </p>
	     */
	    private byte[] ringBuffer;

	    /**
	     * The position in the ring buffer. Used by insertNode.
	     */
	    private short matchPosition;

	    /**
	     * The number of characters in the ring buffer at matchPosition that match a
	     * given string. Used by insertNode.
	     */
	    private short matchLength;

	    /**
	     * leftSon, rightSon, and dad are the Japanese way of referring to a tree
	     * structure. The dad is the parent and it has a right and left son (child).
	     *
	     * <p>
	     * For i = 0 to RING_SIZE-1, rightSon[i] and leftSon[i] will be the right
	     * and left children of node i.
	     * </p>
	     *
	     * <p>
	     * For i = 0 to RING_SIZE-1, dad[i] is the parent of node i.
	     * </p>
	     *
	     * <p>
	     * For i = 0 to 255, rightSon[RING_SIZE + i + 1] is the root of the tree for
	     * strings that begin with the character i. Note that this requires one byte
	     * characters.
	     * </p>
	     *
	     * <p>
	     * These nodes store values of 0...(RING_SIZE-1). Memory requirements can be
	     * reduces by using 2-byte integers instead of full 4-byte integers (for
	     * 32-bit applications). Therefore, these are defined as "shorts."
	     * </p>
	     */
	    private short[] dad;
	    private short[] leftSon;
	    private short[] rightSon;

		/**
	     * Initializes the tree nodes to "empty" states.
	     */
	    private void initTree() {
	        // For i = 0 to RING_SIZE - 1, rightSon[i] and leftSon[i] will be the
	        // right
	        // and left children of node i. These nodes need not be
	        // initialized. However, for debugging purposes, it is nice to
	        // have them initialized. Since this is only used for compression
	        // (not decompression), I don't mind spending the time to do it.
	        //
	        // For the same range of i, dad[i] is the parent of node i.
	        // These are initialized to a known value that can represent
	        // a "not used" state.
	        // For i = 0 to 255, rightSon[RING_SIZE + i + 1] is the root of the tree
	        // for strings that begin with the character i. This is why
	        // the right child array is larger than the left child array.
	        // These are also initialized to a "not used" state.
	        //
	        // Note that there are 256 of these, one for each of the possible
	        // 256 characters.
	        Arrays.fill(dad, 0, dad.Length, NOT_USED);
	        Arrays.fill(leftSon, 0, leftSon.Length, NOT_USED);
	        Arrays.fill(rightSon, 0, rightSon.Length, NOT_USED);
	    }

	 
		/**
	     * Inserts a string from the ring buffer into one of the trees. It loads the
	     * match position and length member variables for the longest match.
	     *
	     * <p>
	     * The string to be inserted is identified by the parameter pos, A full
	     * MAX_STORE_LENGTH bytes are inserted. So, ringBuffer[pos ...
	     * pos+MAX_STORE_LENGTH-1] are inserted.
	     * </p>
	     *
	     * <p>
	     * If the matched length is exactly MAX_STORE_LENGTH, then an old node is
	     * removed in favor of the new one (because the old one will be deleted
	     * sooner).
	     * </p>
	     *
	     * @param pos
	     *            plays a dual role. It is used as both a position in the ring
	     *            buffer and also as a tree node. ringBuffer[pos] defines a
	     *            character that is used to identify a tree node.
	     */
	    private void insertNode(short pos) 
	    {
	       // assert pos >= 0 && pos < RING_SIZE;

	        int cmp = 1;
	        short key = pos;

	        // The last 256 entries in rightSon contain the root nodes for
	        // strings that begin with a letter. Get an index for the
	        // first letter in this string.
	        short p = (short) (RING_SIZE + 1 + (ringBuffer[key] & 0xFF));
	        // assert p > RING_SIZE;

	        // Set the left and right tree nodes for this position to "not used."
	        leftSon[pos] = NOT_USED;
	        rightSon[pos] = NOT_USED;

	        // Haven't matched anything yet.
	        matchLength = 0;

	        while (true) {
	            if (cmp >= 0) {
	                if (rightSon[p] != NOT_USED) {
	                    p = rightSon[p];
	                } else {
	                    rightSon[p] = pos;
	                    dad[pos] = p;
	                    return;
	                }
	            } else {
	                if (leftSon[p] != NOT_USED) {
	                    p = leftSon[p];
	                } else {
	                    leftSon[p] = pos;
	                    dad[pos] = p;
	                    return;
	                }
	            }

	            // Should we go to the right or the left to look for the
	            // next match?
	            short i = 0;
	            for (i = 1; i < MAX_STORE_LENGTH; i++) {
	                cmp = (ringBuffer[key + i] & 0xFF) - (ringBuffer[p + i] & 0xFF);
	                if (cmp != 0) {
	                    break;
	                }
	            }

	            if (i > matchLength) {
	                matchPosition = p;
	                matchLength = i;

	                if (i >= MAX_STORE_LENGTH) {
	                    break;
	                }
	            }
	        }

	        dad[pos] = dad[p];
	        leftSon[pos] = leftSon[p];
	        rightSon[pos] = rightSon[p];

	        dad[leftSon[p]] = pos;
	        dad[rightSon[p]] = pos;

	        if (rightSon[dad[p]] == p) {
	            rightSon[dad[p]] = pos;
	        } else {
	            leftSon[dad[p]] = pos;
	        }

	        // Remove "p"
	        dad[p] = NOT_USED;
	    }

	    /**
	     * Remove a node from the tree.
	     *
	     * @param node
	     *            the node to remove
	     */
	    private void deleteNode(short node) {
	        // assert node >= 0 && node < (RING_SIZE + 1);

	        short q;

	        if (dad[node] == NOT_USED) {
	            // not in tree, nothing to do
	            return;
	        }

	        if (rightSon[node] == NOT_USED) {
	            q = leftSon[node];
	        } else if (leftSon[node] == NOT_USED) {
	            q = rightSon[node];
	        } else {
	            q = leftSon[node];
	            if (rightSon[q] != NOT_USED) {
	                do {
	                    q = rightSon[q];
	                } while (rightSon[q] != NOT_USED);

	                rightSon[dad[q]] = leftSon[q];
	                dad[leftSon[q]] = dad[q];
	                leftSon[q] = leftSon[node];
	                dad[leftSon[node]] = q;
	            }

	            rightSon[q] = rightSon[node];
	            dad[rightSon[node]] = q;
	        }

	        dad[q] = dad[node];

	        if (rightSon[dad[node]] == node) {
	            rightSon[dad[node]] = q;
	        } else {
	            leftSon[dad[node]] = q;
	        }

	        dad[node] = NOT_USED;
	    }

	    /**
	     * The output stream containing the result.
	     */
	    private ByteArrayOutputStream _out;

	    /**
	     * The input stream containing data to be compressed
	     */
		protected InputStream input;

		/**
     	 * The size to read/write when unzipping a compressed byte array of unknown size.
     	 */
    	int BUF_SIZE = 2048;

    	public LZSS ()
		{
		}

	    public LZSS(InputStream input)
	    {
	    	this.input = input;
			this.ringBuffer = new byte[RING_SIZE + MAX_STORE_LENGTH - 1];
	        this.dad = new short[RING_SIZE + 1];
	        this.leftSon = new short[RING_SIZE + 1];
	        this.rightSon = new short[RING_SIZE + 257];
	    }

		/*
	     *
	     */
	    public ByteArrayOutputStream compress() {
	        _out = new ByteArrayOutputStream(BUF_SIZE);

	        short i; // an iterator
	        short r; // node number in the binary tree
	        short s; // position in the ring buffer
	        short len; // length of initial string
	        short lastMatchLength; // length of last match
	        short codeBufPos; // position in the output buffer
	        byte[] codeBuff = new byte[17]; // the output buffer
	        byte mask; // bit mask for byte 0 of out input
	        byte c; // character read from string

	        // Start with a clean tree.
	        initTree();

	        // codeBuff[0] works as eight flags. A "1" represents that the
	        // unit is an unencoded letter (1 byte), and a "0" represents
	        // that the next unit is a <position,length> pair (2 bytes).
	        //
	        // codeBuff[1..16] stores eight units of code. Since the best
	        // we can do is store eight <position,length> pairs, at most 16
	        // bytes are needed to store this.
	        //
	        // This is why the maximum size of the code buffer is 17 bytes.
	        codeBuff[0] = 0;
	        codeBufPos = 1;

	        // Mask iterates over the 8 bits in the code buffer. The first
	        // character ends up being stored in the low bit.
	        //
	        // bit 8 7 6 5 4 3 2 1
	        // | |
	        // | first sequence in code buffer
	        // |
	        // last sequence in code buffer
	        mask = 1;

	        s = 0;
	        r = (short)(RING_SIZE - MAX_STORE_LENGTH);

	        // Initialize the ring buffer with spaces...

	        // Note that the last MAX_STORE_LENGTH bytes of the ring buffer are not
	        // filled.
	        // This is because those MAX_STORE_LENGTH bytes will be filled in
	        // immediately
	        // with bytes from the input stream.
			Arrays.fill(ringBuffer, 0, r, (byte) ' ');

	        // Read MAX_STORE_LENGTH bytes into the last MAX_STORE_LENGTH bytes of
	        // the ring buffer.
	        //
	        // This function loads the buffer with up to MAX_STORE_LENGTH characters
	        // and returns
	        // the actual amount loaded.
	        int readResult = input.read(ringBuffer, r, MAX_STORE_LENGTH);

	        // Make sure there is something to be compressed.
	        if (readResult <= 0) {
	            return _out;
	        }

	        len = (short) readResult;

	        // Insert the MAX_STORE_LENGTH strings, each of which begins with one or
	        // more
	        // 'space' characters. Note the order in which these strings
	        // are inserted. This way, degenerate trees will be less likely
	        // to occur.
	        for (i = 1; i <= MAX_STORE_LENGTH; i++) {
	            insertNode((short) (r - i));
	        }

	        // Finally, insert the whole string just read. The
	        // member variables matchLength and matchPosition are set.
	        insertNode(r);

	        // Now that we're preloaded, continue till done.
	        do {

	            // matchLength may be spuriously long near the end of text.
	            if (matchLength > len) {
	                matchLength = len;
	            }

	            // Is it cheaper to store this as a single character? If so, make it
	            // so.
	            if (matchLength < THRESHOLD) {
	                // Send one character. Remember that codeBuff[0] is the
	                // set of flags for the next eight items.
	                matchLength = 1;
	                codeBuff[0] |= mask;
	                codeBuff[codeBufPos++] = ringBuffer[r];
	            } else {
	                // Otherwise, we do indeed have a string that can be stored
	                // compressed to save space.

	                // The next 16 bits need to contain the position (12 bits)
	                // and the length (4 bits).
	                codeBuff[codeBufPos++] = (byte) matchPosition;
	                codeBuff[codeBufPos++] = (byte) (((matchPosition >> 4) & 0xF0) | (matchLength - THRESHOLD));
	            }

	            // Shift the mask one bit to the left so that it will be ready
	            // to store the new bit.
	            mask <<= 1;

	            // If the mask is now 0, then we know that we have a full set
	            // of flags and items in the code buffer. These need to be
	            // output.
	            if (mask == 0) {
	                // codeBuff is the buffer of characters to be output.
	                // codeBufPos is the number of characters it contains.
	                _out.write(codeBuff, 0, codeBufPos);

	                // Reset for next buffer...
	                codeBuff[0] = 0;
	                codeBufPos = 1;
	                mask = 1;
	            }

	            lastMatchLength = matchLength;

	            // Delete old strings and read new bytes...
	            for (i = 0; i < lastMatchLength; i++) {

	                // Get next character...
	                readResult = input.read();
	                if (readResult == -1) {
	                    break;
	                }
	                c = (byte) readResult;

	                // Delete "old strings"
	                deleteNode(s);

	                // Put this character into the ring buffer.
	                //
	                // The original comment here says "If the position is near
	                // the end of the buffer, extend the buffer to make
	                // string comparison easier."
	                //
	                // That's a little misleading, because the "end" of the
	                // buffer is really what we consider to be the "beginning"
	                // of the buffer, that is, positions 0 through MAX_STORE_LENGTH.
	                //
	                // The idea is that the front end of the buffer is duplicated
	                // into the back end so that when you're looking at characters
	                // at the back end of the buffer, you can index ahead (beyond
	                // the normal end of the buffer) and see the characters
	                // that are at the front end of the buffer without having
	                // to adjust the index.
	                //
	                // That is...
	                //
	                // 1234xxxxxxxxxxxxxxxxxxxxxxxxxxxxx1234
	                // | | |
	                // position 0 end of buffer |
	                // |
	                // duplicate of front of buffer
	                ringBuffer[s] = c;

	                if (s < MAX_STORE_LENGTH - 1) {
	                    ringBuffer[s + RING_SIZE] = c;
	                }

	                // Increment the position, and wrap around when we're at
	                // the end. Note that this relies on RING_SIZE being a power of
	                // 2.
	                s = (short) ((s + 1) & RING_WRAP);
	                r = (short) ((r + 1) & RING_WRAP);

	                // Register the string that is found in
	                // ringBuffer[r..r + MAX_STORE_LENGTH - 1].
	                insertNode(r);
	            }

	            // If we didn't quit because we hit the lastMatchLength,
	            // then we must have quit because we ran out of characters
	            // to process.
	            while (i++ < lastMatchLength) {
	                deleteNode(s);

	                s = (short) ((s + 1) & RING_WRAP);
	                r = (short) ((r + 1) & RING_WRAP);

	                // Note that len hitting 0 is the key that causes the
	                // do...while() to terminate. This is the only place
	                // within the loop that len is modified.
	                //
	                // Its original value is MAX_STORE_LENGTH (or a number less than
	                // MAX_STORE_LENGTH for
	                // short strings).
	                if (--len != 0) {
	                    insertNode(r); /* buffer may not be empty. */
	                }
	            }

	            // End of do...while() loop. Continue processing until there
	            // are no more characters to be compressed. The variable
	            // "len" is used to signal this condition.
	        } while (len > 0);

	        // There could still be something in the output buffer. Send it now.
	        if (codeBufPos > 1) {
	            // codeBuff is the encoded string to send.
	            // codeBufPos is the number of characters.
	            _out.write(codeBuff, 0, codeBufPos);
	        }

	        return _out;
	    }

		/*
	     *
	     */
	    public ByteArrayOutputStream uncompress() {
	        return uncompress(BUF_SIZE);
	    }

	    /*
	     *
	     */
	    public ByteArrayOutputStream uncompress(int expectedSize) {
	        _out = new ByteArrayOutputStream(expectedSize);

	        byte[] c = new byte[MAX_STORE_LENGTH]; // an array of chars
	        byte flags; // 8 bits of flags

	        // Initialize the ring buffer with a common string.
	        //
	        // Note that the last MAX_STORE_LENGTH bytes of the ring buffer are not
	        // filled.
	        // r is a nodeNumber
	        int r = RING_SIZE - MAX_STORE_LENGTH;
	        Arrays.fill(ringBuffer, 0, r, (byte) ' ');

	        flags = 0;
	        int flagCount = 0; // which flag we're on

	        while (true) {
	            // If there are more bits of interest in this flag, then
	            // shift that next interesting bit into the 1's position.
	            //
	            // If this flag has been exhausted, the next byte must be a flag.
	            if (flagCount > 0) {
	                flags = (byte) (flags >> 1);
	                flagCount--;
	            } else {
	                // Next byte must be a flag.
	                int readResult = input.read();
	                if (readResult == -1) {
	                    break;
	                }

	                flags = (byte) (readResult & 0xFF);

	                // Set the flag counter. While at first it might appear
	                // that this should be an 8 since there are 8 bits in the
	                // flag, it should really be a 7 because the shift must
	                // be performed 7 times in order to see all 8 bits.
	                flagCount = 7;
	            }

	            // If the low order bit of the flag is now set, then we know
	            // that the next byte is a single, unencoded character.
	            if ((flags & 1) != 0) {
	                if (input.read(c, 0, 1) != 1) {
	                    break;
	                }

	                _out.write(c[0]);

	                // Add to buffer, and increment to next spot. Wrap at end.
	                ringBuffer[r] = c[0];
	                r = (short) ((r + 1) & RING_WRAP);
	            } else {
	                // Otherwise, we know that the next two bytes are a
	                // <position,length> pair. The position is in 12 bits and
	                // the length is in 4 bits.
	                if (input.read(c, 0, 2) != 2) {
	                    break;
	                }

	                // Convert these two characters into the position and
	                // length in the ringBuffer. Note that the length is always at
	                // least
	                // THRESHOLD, which is why we're able to get a length
	                // of 18 out of only 4 bits.
	                short pos = (short) ((c[0] & 0xFF) | ((c[1] & 0xF0) << 4));
	                short len = (short) ((c[1] & 0x0F) + THRESHOLD);

	                // There are now "len" characters at position "pos" in
	                // the ring buffer that can be pulled out. Note that
	                // len is never more than MAX_STORE_LENGTH.
	                for (int k = 0; k < len; k++) {
	                    c[k] = ringBuffer[(pos + k) & RING_WRAP];

	                    // Add to buffer, and increment to next spot. Wrap at end.
	                    ringBuffer[r] = c[k];
	                    r = (r + 1) & RING_WRAP;
	                }

	                // Add the "len" characters to the output stream.
	                _out.write(c, 0, len);
	            }
	        }
	        return _out;
	    }


		public byte[] BitArrayToByteArray(BitArray bits)
		{
			byte[] ret = new byte[(bits.Length - 1) / 8 + 1];
			bits.CopyTo(ret, 0);
			return ret;
		}

		public BitArray Compress (IList<Byte> source)
		{
			InputStream _is = new ByteArrayInputStream(source.ToArray());
			LZSS lzss = new LZSS(_is);
			ByteArrayOutputStream _os = lzss.compress();
			return new BitArray(_os.toByteArray());
		}

		public Byte[] UnCompress (BitArray source, Int32 bitsCount)
		{
			byte[] barray = BitArrayToByteArray(source);
			InputStream _is = new ByteArrayInputStream(barray);
			LZSS lzss = new LZSS(_is);
			ByteArrayOutputStream _os = lzss.uncompress();
			return _os.toByteArray();
		}

	}
}
