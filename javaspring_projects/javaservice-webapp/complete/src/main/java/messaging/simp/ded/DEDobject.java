package messaging.simp.ded;

/**
 * Created by serup on 23-12-15.
 */
public class DEDobject
{
	byte[] uncompresseddata;
	int iLengthOfTotalData;
	byte[] pCompressedData;
	int sizeofCompressedData;

	public byte[] getUncompresseddata() {
		return uncompresseddata;
	}

	public void setUncompresseddata(byte[] uncompresseddata) {
		this.uncompresseddata = uncompresseddata;
	}

	public byte[] getpCompressedData() {
		return pCompressedData;
	}

	public void setpCompressedData(byte[] pCompressedData) {
		this.pCompressedData = pCompressedData;
	}

	public int getiLengthOfTotalData() {
		return iLengthOfTotalData;
	}

	public void setiLengthOfTotalData(int iLengthOfTotalData) {
		this.iLengthOfTotalData = iLengthOfTotalData;
	}

	public int getSizeofCompressedData() {
		return sizeofCompressedData;
	}

	public void setSizeofCompressedData(int sizeofCompressedData) {
		this.sizeofCompressedData = sizeofCompressedData;
	}
}
