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
// http://darwinsource.opendarwin.org/Current/kext_tools59/compression.c
//#pragma comment( user, "Compiled on " __DATE__ " at " __TIME__ )
//#pragma message( "Compiling " __FILE__ )
//#pragma message( "Last modified on " __TIMESTAMP__ )
//#pragma once

//#define bzero RtlZeroMemory
//#define u_int8_t unsigned _int8
//#define int32_t _int32
//#define u_int32_t unsigned _int32

#include <cstring>

#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))
#define bzero RtlZeroMemory
#define ZeroMemory RtlZeroMemory
#ifndef __u_int8_t
#define __u_int8_t unsigned char
#endif
#ifndef __int32_t
#define __int32_t int
#endif
#ifndef __u_int32_t
#define __u_int32_t unsigned int
#endif

namespace HandleCompression
{

#define cN         4096  /* size of ring buffer - must be power of 2 */
#define cF         18    /* upper limit for match_length */
#define THRESHOLD 2     /* encode string into position and length
                           if match_length is greater than this */
#define NIL       cN     /* index for root of binary search trees */

    struct encode_state {
        /*
        * left & right children & parent. These constitute binary search trees.
        */
        int lchild[cN + 1], rchild[cN + 257], parent[cN + 1];

        /* ring buffer of size N, with extra F-1 bytes to aid string comparison
        */
        __u_int8_t text_buf[cN + cF - 1];

        /*
        * match_length of longest match.
        * These are set by the insert_node() procedure.
        */
        int match_position, match_length;
    };


    class kext_tools59 //: public CStatic
    {

        public:
            __u_int32_t   local_adler32(__u_int8_t *buffer, __int32_t length);
            int         decompress_lzss(__u_int8_t *dst, __u_int8_t *src, __u_int32_t srclen);
            void        init_state(struct encode_state *sp);
            void        insert_node(struct encode_state *sp, int r);
            void        delete_node(struct encode_state *sp, int p);
            int  compress_lzss(__u_int8_t *dst, __u_int32_t dstlen, __u_int8_t *src, __u_int32_t srcLen);

        protected:
    };

}
using namespace HandleCompression;
