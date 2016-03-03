// originate from :
// http://darwinsource.opendarwin.org/Current/kext_tools59/compression.c
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
//#pragma comment( user, "Compiled on " __DATE__ " at " __TIME__ )
//#pragma message( "Compiling " __FILE__ )
//#pragma message( "Last modified on " __TIMESTAMP__ )

//#include "stdafx.h"
#include "compression.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#include <string.h>
#include <cstring>

using namespace std;

namespace HandleCompression
{
__u_int32_t kext_tools59::local_adler32(__u_int8_t *buffer, __int32_t length)
{
    try
    {
    register __int32_t cnt;
    __u_int32_t  result, lowHalf, highHalf;

    lowHalf = 1;
    highHalf = 0;

    for (cnt = 0; cnt < length; cnt++) {
        if ((cnt % 5000) == 0) {
            lowHalf  %= 65521L;
            highHalf %= 65521L;
        }

        lowHalf += buffer[cnt];
        highHalf += lowHalf;
    }

    lowHalf  %= 65521L;
    highHalf %= 65521L;

    result = (highHalf << 16) | lowHalf;

    return result;
    }catch(...){ return (__u_int32_t)0; }
}


//////////////////////////////////////////////////////
// DECOMPRESS                                       //
//////////////////////////////////////////////////////
int kext_tools59::decompress_lzss(__u_int8_t *dst, __u_int8_t *src, __u_int32_t srclen)
{
    try
    {
    /* ring buffer of size N, with extra F-1 bytes to aid string comparison
*/
    __u_int8_t text_buf[cN + cF - 1];
    __u_int8_t *dststart = dst;
    __u_int8_t *srcend = src + srclen;
    int  i, j, r, c;
    register int k;
    unsigned int flags;

    dst = dststart;
    srcend = src + srclen;
    for (i = 0; i < cN - cF; i++)
        text_buf[i] = 0;
    r = cN - cF;
    flags = 0;
    for ( ; ; ) {
        if (((flags >>= 1) & 0x100) == 0) {
            if (src < srcend) c = *src++; else break;
            flags = c | 0xFF00;  /* uses higher byte cleverly */
        }   /* to count eight */
        if (flags & 1) {
            if (src < srcend) c = *src++; else break;
            *dst++ = c;
            text_buf[r++] = c;
            r &= (cN - 1);
        } else {
            if (src < srcend) i = *src++; else break;
            if (src < srcend) j = *src++; else break;
            i |= ((j & 0xF0) << 4);
            j  =  (j & 0x0F) + THRESHOLD;
            for (k = 0; k <= j; k++) {
                c = text_buf[(i + k) & (cN - 1)];
                *dst++ = c;
                text_buf[r++] = c;
                r &= (cN - 1);
            }
        }
    }

    return (int)(dst - dststart);
    }catch(...){ return 0; }
}


/*
 * initialize state, mostly the trees
 *
 * For i = 0 to N - 1, rchild[i] and lchild[i] will be the right and left
 * children of node i.  These nodes need not be initialized.  Also,
parent[i]
 * is the parent of node i.  These are initialized to NIL (= N), which
stands
 * for 'not used.'  For i = 0 to 255, rchild[N + i + 1] is the root of the
 * tree for strings that begin with character i.  These are initialized to
NIL.
 * Note there are 256 trees. */
void kext_tools59::init_state(struct encode_state *sp)
{
    register int  i;

    bzero(sp, sizeof(*sp));

    for (i = 0; i < cN - cF; i++)
        sp->text_buf[i] = ' ';
    for (i = cN + 1; i <= cN + 256; i++)
        sp->rchild[i] = NIL;
    for (i = 0; i < cN; i++)
        sp->parent[i] = NIL;
}

/*
 * Inserts string of length F, text_buf[r..r+F-1], into one of the trees
 * (text_buf[r]'th tree) and returns the longest-match position and length
 * via the global variables match_position and match_length.
 * If match_length = F, then removes the old node in favor of the new one,
 * because the old one will be deleted sooner. Note r plays double role,
 * as tree node and position in buffer.
 */
void kext_tools59::insert_node(struct encode_state *sp, int r)
{
    int  p, cmp;
    register int i;
    __u_int8_t  *key;

    cmp = 1;
    key = &sp->text_buf[r];
    p = cN + 1 + key[0];
    sp->rchild[r] = sp->lchild[r] = NIL;
    sp->match_length = 0;
    for ( ; ; ) {
        if (cmp >= 0) {
            if (sp->rchild[p] != NIL)
                p = sp->rchild[p];
            else {
                sp->rchild[p] = r;
                sp->parent[r] = p;
                return;
            }
        } else {
            if (sp->lchild[p] != NIL)
                p = sp->lchild[p];
            else {
                sp->lchild[p] = r;
                sp->parent[r] = p;
                return;
            }
        }
        for (i = 1; i < cF; i++) {
            if ((cmp = key[i] - sp->text_buf[p + i]) != 0)
                break;
        }
        if (i > sp->match_length) {
            sp->match_position = p;
            if ((sp->match_length = i) >= cF)
                break;
        }
    }
    sp->parent[r] = sp->parent[p];
    sp->lchild[r] = sp->lchild[p];
    sp->rchild[r] = sp->rchild[p];
    sp->parent[sp->lchild[p]] = r;
    sp->parent[sp->rchild[p]] = r;
    if (sp->rchild[sp->parent[p]] == p)
        sp->rchild[sp->parent[p]] = r;
    else
        sp->lchild[sp->parent[p]] = r;
    sp->parent[p] = NIL;  /* remove p */
}

/* deletes node p from tree */
void kext_tools59::delete_node(struct encode_state *sp, int p)
{
    int  q;

    if (sp->parent[p] == NIL)
        return;  /* not in tree */
    if (sp->rchild[p] == NIL)
        q = sp->lchild[p];
    else if (sp->lchild[p] == NIL)
        q = sp->rchild[p];
    else {
        q = sp->lchild[p];
        if (sp->rchild[q] != NIL) {
            do {
                q = sp->rchild[q];
            } while (sp->rchild[q] != NIL);
            sp->rchild[sp->parent[q]] = sp->lchild[q];
            sp->parent[sp->lchild[q]] = sp->parent[q];
            sp->lchild[q] = sp->lchild[p];
            sp->parent[sp->lchild[p]] = q;
        }
        sp->rchild[q] = sp->rchild[p];
        sp->parent[sp->rchild[p]] = q;
    }
    sp->parent[q] = sp->parent[p];
    if (sp->rchild[sp->parent[p]] == p)
        sp->rchild[sp->parent[p]] = q;
    else
        sp->lchild[sp->parent[p]] = q;
    sp->parent[p] = NIL;
}


//////////////////////////////////////////////////////
// COMPRESS                                         //
//////////////////////////////////////////////////////
int  kext_tools59::compress_lzss(__u_int8_t *dst, __u_int32_t dstlen, __u_int8_t *src, __u_int32_t srcLen)
{
    int size=0;

    /* Encoding state, mostly tree but some current match stuff */
    struct encode_state fsp;
    struct encode_state *sp;

    int c, r, s, last_match_length, code_buf_ptr;
    register int i;
    register int len;
    __u_int8_t code_buf[17], mask;
    __u_int8_t *srcend = src + srcLen;
    __u_int8_t *dstend = dst + dstlen;
    //u_int8_t *begindst = dst;

    try
    {
    /* initialize trees */
    sp = (struct encode_state *)&fsp;
    init_state(sp);

    /*
     * code_buf[1..16] saves eight units of code, and code_buf[0] works
     * as eight flags, "1" representing that the unit is an unencoded
     * letter (1 byte), "0" a position-and-length pair (2 bytes).
     * Thus, eight units require at most 16 bytes of code.
     */
    code_buf[0] = 0;
    code_buf_ptr = mask = 1;

    /* Clear the buffer with any character that will appear often. */
    s = 0;  r = cN - cF;

    /* Read F bytes into the last F bytes of the buffer */
    for (len = 0; len < cF && src < srcend; len++)
        sp->text_buf[r + len] = *src++;
    if (!len) {
        return 0;  /* text of size zero */
    }
    /*
     * Insert the F strings, each of which begins with one or more
     * 'space' characters.  Note the order in which these strings are
     * inserted.  This way, degenerate trees will be less likely to occur.
     */
    for (i = 1; i <= cF; i++)
        insert_node(sp, r - i);

    /*
     * Finally, insert the whole string just read.
     * The global variables match_length and match_position are set.
     */
    insert_node(sp, r);
    do {
        /* match_length may be spuriously long near the end of text. */
        if (sp->match_length > len)
            sp->match_length = len;
        if (sp->match_length <= THRESHOLD) {
            sp->match_length = 1;  /* Not long enough match.  Send one byte.
*/
            code_buf[0] |= mask;  /* 'send one byte' flag */
            code_buf[code_buf_ptr++] = sp->text_buf[r];  /* Send uncoded. */
        } else {
            /* Send position and length pair. Note match_length > THRESHOLD.
*/
            code_buf[code_buf_ptr++] = (__u_int8_t) sp->match_position;
            code_buf[code_buf_ptr++] = (__u_int8_t)
                ( ((sp->match_position >> 4) & 0xF0)
                |  (sp->match_length - (THRESHOLD + 1)) );
        }
        if ((mask <<= 1) == 0) {  /* Shift mask left one bit. */
                /* Send at most 8 units of code together */
            for (i = 0; i < code_buf_ptr; i++)
            {
                if (dst < dstend){
                    *dst++ = code_buf[i];
                    size++;
                }
                else{
                    return  0;
                }
            }
            code_buf[0] = 0;
            code_buf_ptr = mask = 1;
        }
        last_match_length = sp->match_length;
        for (i = 0; i < last_match_length && src < srcend; i++) {
            delete_node(sp, s);    /* Delete old strings and */
            c = *src++;
            sp->text_buf[s] = c;    /* read new bytes */

            /*
             * If the position is near the end of buffer, extend the buffer
             * to make string comparison easier.
             */
            if (s < cF - 1)
                sp->text_buf[s + cN] = c;

            /* Since this is a ring buffer, increment the position modulo N.
*/
            s = (s + 1) & (cN - 1);
            r = (r + 1) & (cN - 1);

            /* Register the string in text_buf[r..r+F-1] */
            insert_node(sp, r);
        }
        while (i++ < last_match_length) {
        delete_node(sp, s);

            /* After the end of text, no need to read, */
            s = (s + 1) & (cN - 1);
            r = (r + 1) & (cN - 1);
            /* but buffer may not be empty. */
            if (--len)
                insert_node(sp, r);
        }
    } while (len > 0);   /* until length of string to be processed is zero
*/

    if (code_buf_ptr > 1) {    /* Send remaining code. */
        for (i = 0; i < code_buf_ptr; i++){
            if (dst < dstend){
                *dst++ = code_buf[i];
                    size++;
            }
            else {
                return  0;
                //return  size;//unable to compress, thus return orig size - this error bug found with boost unittest :-)
            }
        }
    }


    }catch(...){ size=0; }


    return size;
}

/*
BEGIN_MESSAGE_MAP(kext_tools59, CStatic)
END_MESSAGE_MAP()
IMPLEMENT_DYNCREATE(kext_tools59, CStatic)
*/

}// end of namespace
