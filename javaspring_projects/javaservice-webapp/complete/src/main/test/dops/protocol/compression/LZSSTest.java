package dops.protocol.compression;

import dops.protocol.ded.DEDDecoder;
import dops.protocol.ded.DEDEncoder;
import org.junit.Test;
import org.springframework.core.io.FileSystemResource;
import org.springframework.core.io.Resource;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.MissingResourceException;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.fail;

/**
 * Created by serup on 28-12-15.
 */
public class LZSSTest {

    DEDEncoder dedEncoder = new DEDEncoder();
    DEDDecoder dedDecoder = new DEDDecoder();

    public static byte[] getBytes(InputStream is) throws IOException {

        int len;
        int size = 1024;
        byte[] buf;

        if (is instanceof ByteArrayInputStream) {
            size = is.available();
            buf = new byte[size];
            len = is.read(buf, 0, size);
        } else {
            ByteArrayOutputStream bos = new ByteArrayOutputStream();
            buf = new byte[size];
            while ((len = is.read(buf, 0, size)) != -1)
                bos.write(buf, 0, len);
            buf = bos.toByteArray();
        }
        return buf;
    }


    @Test
    public void testCompression() {
        InputStream txtfile = null;
        byte[] originalTxtfileInBytes=null;
        try {
            Resource resource = null;
            resource = new FileSystemResource("src/main/resources/helloworld.txt");
            InputStream file = (InputStream) resource.getInputStream();
            originalTxtfileInBytes = getBytes(file);
        } catch (MissingResourceException e) {
            fail();
        } catch (Exception e) {
            fail();
        }


        //String originaltxt = "ATATAAAFFFF";
        //txtfile = new ByteArrayInputStream(originaltxt.getBytes());

        txtfile = new ByteArrayInputStream(originalTxtfileInBytes);

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
         //   result = new String(back, "UTF-8");
         //   assertEquals(originaltxt,result);

            assertArrayEquals(originalTxtfileInBytes,back);
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
