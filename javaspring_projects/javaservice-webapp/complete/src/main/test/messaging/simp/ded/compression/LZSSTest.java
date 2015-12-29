package messaging.simp.ded.compression;
import messaging.simp.ded.DEDDecoder;
import messaging.simp.ded.DEDEncoder;
import org.apache.tomcat.util.http.fileupload.IOUtils;
import org.junit.Test;

import java.io.*;
import java.util.MissingResourceException;

import static org.junit.Assert.*;

/**
 * Created by serup on 28-12-15.
 */
public class LZSSTest {

    DEDEncoder dedEncoder = new DEDEncoder();
    DEDDecoder dedDecoder = new DEDDecoder();

    @Test
    public void testCompression() {
        InputStream txtfile = null;
/*        try {
            txtfile =  getClass().getResourceAsStream("helloworld.txt");
        } catch (MissingResourceException e) {
            fail();
        } catch (Exception e) {
            fail();
        }
        */
        String originaltxt = "ATATAAAFFFF";
        txtfile = new ByteArrayInputStream(originaltxt.getBytes());

        Compressor compressor = new LZSS(txtfile);
        ByteArrayOutputStream bosCompressed = null;
        try {
            bosCompressed = compressor.compress();
        } catch (IOException e) {
            fail();
            return;
        }
        Compressor uncompressor = new LZSS(new ByteArrayInputStream(bosCompressed.toByteArray()));
        ByteArrayOutputStream bosUncompressed = null;
        try {
            bosUncompressed = uncompressor.uncompress();
        } catch (IOException e) {
            fail();
            return;
        };
        String result;
        try {
            byte[] back = bosUncompressed.toByteArray();
            result = new String(back, "UTF-8");
            assertEquals(originaltxt,result);
        } catch (Exception e) {
            fail();
            return;
        }

    }

    @Test
    public void testDEDEncode_DEDDecode() throws Exception
    {
        // Do compression - okumura style
        byte[] uncmpdata = "Any String you want...........AAAAABBBBBBCCCCCCDDDDDEEEEEFFFFFF................".getBytes();

        // now make room for case where compression yields lager size - when trying to compress an image for example.
        byte[] tmpCompressedData;


        // now compress
        tmpCompressedData = dedEncoder.compress_lzss(uncmpdata, uncmpdata.length);
        int compressedSize=-1;
        if(tmpCompressedData != null) {
            compressedSize = tmpCompressedData.length;
            byte[] decompressedData = dedDecoder.decompress_lzss(tmpCompressedData,compressedSize);
            assertArrayEquals(uncmpdata,decompressedData);// test if decompressed data is same as the data before compression
        }
    }
}
