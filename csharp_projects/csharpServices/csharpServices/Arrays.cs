using System;

namespace csharpServices
{
	public class Arrays
	{
		public Arrays ()
		{
		}

		/**
	     * Copies the specified array, truncating or padding with zeros (if necessary)
	     * so the copy has the specified length.  For all indices that are
	     * valid in both the original array and the copy, the two arrays will
	     * contain identical values.  For any indices that are valid in the
	     * copy but not the original, the copy will contain <tt>(byte)0</tt>.
	     * Such indices will exist if and only if the specified length
	     * is greater than that of the original array.
	     *
	     * @param original the array to be copied
	     * @param newLength the length of the copy to be returned
	     * @return a copy of the original array, truncated or padded with zeros
	     *     to obtain the specified length
	     * @throws NegativeArraySizeException if <tt>newLength</tt> is negative
	     * @throws NullPointerException if <tt>original</tt> is null
	     * @since 1.6
	     */
	    public static byte[] copyOf(byte[] original, int newLength) {
	        byte[] copy = new byte[newLength];
	        System.Array.Copy(original, copy, Math.min(original.Length, newLength));
	        return copy;
	    }

		/**
	     * Checks that {@code fromIndex} and {@code toIndex} are in
	     * the range and throws an exception if they aren't.
	     */
	    private static void rangeCheck(int arrayLength, int fromIndex, int toIndex) {
	        if (fromIndex > toIndex) {
	            throw new ArgumentException("fromIndex(" + fromIndex.ToString() + ") > toIndex(" + toIndex.ToString() + ")");
	        }
	        if (fromIndex < 0) {
	            throw new IndexOutOfRangeException(fromIndex.ToString());
	        }
	        if (toIndex > arrayLength) {
				throw new IndexOutOfRangeException(toIndex.ToString());
	        }
	    }

	 

		/**
	     * Assigns the specified short value to each element of the specified
	     * range of the specified array of shorts.  The range to be filled
	     * extends from index <tt>fromIndex</tt>, inclusive, to index
	     * <tt>toIndex</tt>, exclusive.  (If <tt>fromIndex==toIndex</tt>, the
	     * range to be filled is empty.)
	     *
	     * @param a the array to be filled
	     * @param fromIndex the index of the first element (inclusive) to be
	     *        filled with the specified value
	     * @param toIndex the index of the last element (exclusive) to be
	     *        filled with the specified value
	     * @param val the value to be stored in all elements of the array
	     * @throws IllegalArgumentException if <tt>fromIndex &gt; toIndex</tt>
	     * @throws ArrayIndexOutOfBoundsException if <tt>fromIndex &lt; 0</tt> or
	     *         <tt>toIndex &gt; a.length</tt>
	     */
	    public static void fill(short[] a, int fromIndex, int toIndex, short val) {
	        rangeCheck(a.Length, fromIndex, toIndex);
	        for (int i = fromIndex; i < toIndex; i++)
	            a[i] = val;
	    }

	}
}

