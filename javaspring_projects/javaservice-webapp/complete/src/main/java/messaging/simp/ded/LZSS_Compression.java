package messaging.simp.ded;

// originate from :
// http://darwinsource.opendarwin.org/Current/kext_tools59/compression.c
// Converted to Java, by johnny.serup@scanva.com
//
/**************************************************************
 LZSS.C -- A Data Compression Program
 ***************************************************************
 4/6/1989 Haruhiko Okumura
 Use, distribute, and modify this program freely.
 Please send me your improved versions.
 PC-VAN      SCIENCE
 NIFTY-Serve PAF01022
 CompuServe  74050,1022

 **************************************************************/
// http://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Storer%E2%80%93Szymanski
// Lempel<96>Ziv<96>Storer<96>Szymanski (LZSS) is a lossless data compression algorithm, a derivative of LZ77, that was created in 1982 by James Storer and Thomas Szymanski. LZSS was described in article "Data compression via textual substitution" published in Journal of the ACM (pp. 928<96>951).
//
//
//

/**
 * Created by serup on 24-12-2015.
 */
public class LZSS_Compression {
    int cN =       4096;  /* size of ring buffer - must be power of 2 */
    int cF =       18;    /* upper limit for match_length */
    int THRESHOLD = 2;    /* encode string into position and length
                         if match_length is greater than this */
    int NIL       = cN;   /* index for root of binary search trees */



    // http://en.wikipedia.org/wiki/Adler-32
    int local_adler32(byte[] buffer, int length) {
        int result;
        int cnt;
        int lowHalf, highHalf;
        lowHalf = 1;
        highHalf = 0;

        for (cnt = 0; cnt < length; cnt++) {
            if ((cnt % 5000) == 0) {
                lowHalf = lowHalf  % 65521;
                highHalf = highHalf % 65521;
            }
            lowHalf += buffer[cnt];
            highHalf += lowHalf;
        }
        lowHalf = lowHalf  % 65521;
        highHalf = highHalf % 65521;
        result = (highHalf << 16) | lowHalf;
        return result;
    }

    /*
     * initialize state, mostly the trees
     *
     * For i = 0 to N - 1, rchild[i] and lchild[i] will be the right and left
     * children of node i.  These nodes need not be initialized.  Also,._parent[i]
     * is the._parent of node i.  These are initialized to NIL (= N), which stands
     * for 'not used.'  For i = 0 to 255, rchild[N + i + 1] is the root of the
     * tree for strings that begin with character i.  These are initialized to NIL.
     * Note there are 256 trees. */
//function init_state(struct encode_state *sp)
/*    int init_state(sp)
    {
        int  i;

        //bzero(sp, sizeof(*sp));
        for(i = 0; i < sp.text_buf.length; i++)
            sp.text_buf[i] = 0;
        for(i = 0; i < sp.rchild.length; i++)
            sp.rchild[i] = 0;
        for(i = 0; i < sp.lchild.length; i++)
            sp.lchild[i] = 0;
        for(i = 0; i < sp._parent.length; i++)
            sp._parent[i] = 0;

        for (i = 0; i < cN - cF; i++)
            sp.text_buf[i] = ' ';
        for (i = cN + 1; i <= cN + 256; i++)
            sp.rchild[i] = NIL;
        for (i = 0; i < cN; i++)
            sp._parent[i] = NIL;

        return i;
    }
*/
}
