package messaging.simp.ded.compression;
import messaging.simp.ded.DEDEncoder;
import org.junit.Test;

import static org.junit.Assert.*;

/**
 * Created by serup on 28-12-15.
 */
public class LZSSTest {

    DEDEncoder dedEncoder = new DEDEncoder();

    @Test
    public void testCompress() throws Exception {

	// Do compression - okumura style
	byte[] uncmpdata = "Any String you want".getBytes();

    	// now make room for case where compression yields lager size - when trying to compress an image for example.
    	byte[] tmpCompressedData;

    	// now compress
    	tmpCompressedData = dedEncoder.compress_lzss(uncmpdata, uncmpdata.length);
    	int compressedSize=-1;
    	if(tmpCompressedData != null)
	    compressedSize = tmpCompressedData.length;
	    assertNotEquals(-1,compressedSize);

    }

    @Test
    public void testUncompress() throws Exception {

    }
}
