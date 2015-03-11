// originate from :
// http://darwinsource.opendarwin.org/Current/kext_tools59/compression.c
// Converted to JavaScript, by johnny.serup@scanva.com
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
// Lempel–Ziv–Storer–Szymanski (LZSS) is a lossless data compression algorithm, a derivative of LZ77, that was created in 1982 by James Storer and Thomas Szymanski. LZSS was described in article "Data compression via textual substitution" published in Journal of the ACM (pp. 928–951).
//
//
//


var doc = this["document"];

if (!this["output"]) {
  var output = function (string) {
    return doc ? doc.write(string + '<br/>') : console.log(string);
  };
}

function Init() {
	output("\nHello World\n");
	return 1;
}

var cN =       4096;  /* size of ring buffer - must be power of 2 */
var cF =       18;    /* upper limit for match_length */
var THRESHOLD = 2;    /* encode string into position and length
                         if match_length is greater than this */
var NIL       = cN;   /* index for root of binary search trees */


function local_adler32(buffer, length) {
    var result=-1;

// http://en.wikipedia.org/wiki/Adler-32
//
// http://dreaminginjavascript.wordpress.com/2009/02/09/bitwise-byte-foolish/
//
    var cnt;
    var result, lowHalf, highHalf;

    lowHalf = 1;
    highHalf = 0;

    for (cnt = 0; cnt < length; cnt++) {
        if ((cnt % 5000) == 0) {
            lowHalf  % 65521;
            highHalf % 65521;
        }

        lowHalf += buffer[cnt];
        highHalf += lowHalf;
    }

    lowHalf  % 65521;
    highHalf % 65521;

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
function init_state(sp)
{
    var  i;

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


/*
 * Inserts string of length F, text_buf[r..r+F-1], into one of the trees
 * (text_buf[r]'th tree) and returns the longest-match position and length
 * via the global variables match_position and match_length.
 * If match_length = F, then removes the old node in favor of the new one,
 * because the old one will be deleted sooner. Note r plays double role,
 * as tree node and position in buffer.
 */
function insert_node(sp, r)
{
    var  p, cmp;
    var i;
    var key;
    var safetycount = 0;

  try
  {

    cmp = 1;
    //key = sp.text_buf[r]; // http://stackoverflow.com/questions/10923949/weirdness-in-equated-java-arrays-references-vs-pointers
    key = sp.text_buf; 
    try
    {
    //p = cN + 1 + key[0]; // not possible in javascript
	if (typeof key[0 + r] == 'string') 
    		p = cN + 1 + key[0 + r].charCodeAt(0);
	else
    		p = cN + 1 + key[0 + r];
    }
    catch(err)
    {
    output("---");
    output("[insert_node] Exception [ p = cN + 1 + key[0 + r].charCodeAt(0); ] : " + err );
    output("---");
    output("\n");
    return -3;
    }
    sp.rchild[r] = sp.lchild[r] = NIL;
    sp.match_length = 0;
    for ( ; ; ) {
	safetycount++;
	
	if (cmp >= 0) {
            if (sp.rchild[p] != NIL)
                p = sp.rchild[p];
            else {
                sp.rchild[p] = r;
                sp._parent[r] = p;
                return 1;
            }
        } else {
            if (sp.lchild[p] != NIL)
                p = sp.lchild[p];
            else {
                sp.lchild[p] = r;
                sp._parent[r] = p;
                return 2;
            }
        }
        for (i = 1; i < cF; i++) {
            var a = "";
	    var b = "";
	    var a = key[i + r];
	    var b = sp.text_buf[p + i];
	    var c = a + b;
	    try
	    {
	    	if(typeof c == 'string')
		    cmp =  c.charCodeAt(0) - c.charCodeAt(1);
		else
		    cmp =  c;
	    }
	    catch(err)
	    {
	      output("---");
	      output("[insert_node] Exception [ cmp =  c.charCodeAt(0) - c.charCodeAt(1); ] : " + err );
	      output("---");
	      output("\n");
	      return -4;
	    }
 	    
            if (cmp != 0)
                break;
        }

        if (i > sp.match_length) {
            sp.match_position = p;
            if ((sp.match_length = i) >= cF)
                break;
        }
	if(safetycount > 10000)
		//throw "[insert_node] exception: infinite loop in insert_node";
                return -5;
    }
    sp._parent[r] = sp._parent[p];
    sp.lchild[r] = sp.lchild[p];
    sp.rchild[r] = sp.rchild[p];
    sp._parent[sp.lchild[p]] = r;
    sp._parent[sp.rchild[p]] = r;
    try
    {
    if (sp.rchild[sp._parent[p]] == p)
        sp.rchild[sp._parent[p]] = r;
    else
        sp.lchild[sp._parent[p]] = r;
    sp._parent[p] = NIL;  // remove p 
    }
    catch(err)
    {
      output("---");
      output("[insert_node] Exception [ if (sp.rchild[sp._parent[p]] == p) ] : " + err );
      output("---");
      output("\n");
      return -5;
    }
 
  }
  catch(err)
  {
    output("---");
    output("[insert_node] Exception: " + err );
    output("---");
    output("\n");
    return -2; // error - exit to avoid infinite loop
  }

  return 0;
}

/* deletes node p from tree */
function delete_node(sp, p)
{
    var q;

    if (sp._parent[p] == NIL)
        return -1;  /* not in tree */
    if (sp.rchild[p] == NIL)
        q = sp.lchild[p];
    else if (sp.lchild[p] == NIL)
        q = sp.rchild[p];
    else {
        q = sp.lchild[p];
        if (sp.rchild[q] != NIL) {
            do {
                q = sp.rchild[q];
            } while (sp.rchild[q] != NIL);
            sp.rchild[sp._parent[q]] = sp.lchild[q];
            sp._parent[sp.lchild[q]] = sp._parent[q];
            sp.lchild[q] = sp.lchild[p];
            sp._parent[sp.lchild[p]] = q;
        }
        sp.rchild[q] = sp.rchild[p];
        sp._parent[sp.rchild[p]] = q;
    }
    sp._parent[q] = sp._parent[p];
    if (sp.rchild[sp._parent[p]] == p)
        sp.rchild[sp._parent[p]] = q;
    else
        sp.lchild[sp._parent[p]] = q;
    sp._parent[p] = NIL;

    return 0;
}

function makeStruct(names) {
  var names = names.split(' ');
  var count = names.length;
  function constructor() {
    for (var i = 0; i < count; i++) {
	  this[names[i]] = arguments[i];
    }
  }
  return constructor;
}


//////////////////////////////////////////////////////
// COMPRESS                                         //
//////////////////////////////////////////////////////
function compress_lzss(dst, dstlen, src, srcLen)
{
    var size=0;

    /* Encoding state, mostly tree but some current match stuff */
    //
    // left & right children & parent. These constituteComponents.utils.import("resource://gre/modules/ctypes.jsm"); binary search trees.
    //
    var lchild = new Array(cN + 1);
    var rchild = new Array(cN + 257);
    var _parent = new Array(cN + 1);

    // ring buffer of size N, with extra F-1 bytes to aid string comparison
    //
    var text_buf = new Array(cN + cF - 1);

    //
    // match_length of longest match.
    // These are set by the insert_node() procedure.
    //
    var match_position, match_length;

    var struct_encode_state = makeStruct("lchild rchild _parent text_buf match_position match_length");
    var fsp = new struct_encode_state(lchild, rchild, _parent, text_buf, match_position, match_length);
    // struct encode_state fsp;
    //struct encode_state *sp;

    var c, r, s, last_match_length, code_buf_ptr;
    var i;
    var len;

    try
    {
    /* initialize trees */
    sp = fsp;
    init_state(sp);

    /*
     * code_buf[1..16] saves eight units of code, and code_buf[0] works
     * as eight flags, "1" representing that the unit is an unencoded
     * letter (1 byte), "0" a position-and-length pair (2 bytes).
     * Thus, eight units require at most 16 bytes of code.
     */
    var code_buf = new Array(17);
    var mask;
    code_buf[0] = 0;
    code_buf_ptr = mask = 1;

    nsrclen = src.length;
    nsrcend = nsrclen;


    /* Clear the buffer with any character that will appear often. */
    s = 0;  r = cN - cF;

    /* Read F bytes into the last F bytes of the buffer */
    for (len = 0, nsrc = 0; len < cF && nsrc < nsrcend; len++, nsrc++)
        sp.text_buf[r + len] = src[nsrc]; 
    if (!len) {
        return 0;  /* text of size zero */
    }
    /*
     * Insert the F strings, each of which begins with one or more
     * 'space' characters.  Note the order in which these strings are
     * inserted.  This way, degenerate trees will be less likely to occur.
     */
    for (i = 1; i <= cF; i++)
        if(insert_node(sp, r - i) < 0) return -1;

    /*
     * Finally, insert the whole string just read.
     * The global variables match_length and match_position are set.
     */
    ndst = 0;
    ndstend = dstlen;
    if(insert_node(sp, r) < 0) return -2;
    do {

	/* match_length may be spuriously long near the end of text. */
        if (sp.match_length > len)
            sp.match_length = len;
        if (sp.match_length <= THRESHOLD) {
            sp.match_length = 1;  /* Not long enough match.  Send one byte.  */
            code_buf[0] |= mask;  /* 'send one byte' flag */
            code_buf[code_buf_ptr++] = sp.text_buf[r];  /* Send uncoded. */
        } else {
            /* Send position and length pair. Note match_length > THRESHOLD.  */
            code_buf[code_buf_ptr++] = sp.match_position;
            code_buf[code_buf_ptr++] = ( ((sp.match_position >> 4) & 0x000000F0) 
			               |  (sp.match_length - (THRESHOLD + 1)) )&0x000000FF;
        }
   
	mask=(mask<<=1)&0x000000FF;   // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Bitwise_Operators	
    	if (mask === 0) {  /* Shift mask left one bit. */
                /* Send at most 8 units of code together */
            for (i = 0; i < code_buf_ptr; i++,ndst++)
            {
                if (ndst < ndstend){
                    dst[ndst] = code_buf[i];
                    size++;
                }
                else{
                    return  0;
                }
            }
            code_buf[0] = 0;
            code_buf_ptr = mask = 1;
        }
        last_match_length = sp.match_length;
        for (i = 0; i < last_match_length && nsrc < nsrcend; i++,nsrc++) {
            delete_node(sp, s);    /* Delete old strings and */
            c = src[nsrc];
            sp.text_buf[s] = c;    /* read new bytes */

            /*
             * If the position is near the end of buffer, extend the buffer
             * to make string comparison easier.
             */
            if (s < cF - 1)
                sp.text_buf[s + cN] = c;

            /* Since this is a ring buffer, increment the position modulo N.  */
            s = (s + 1) & (cN - 1);
            r = (r + 1) & (cN - 1);

            /* Register the string in text_buf[r..r+F-1] */
            if(insert_node(sp, r) < 0) return -3;
        }
        while (i++ < last_match_length) {
        delete_node(sp, s);

            /* After the end of text, no need to read, */
            s = (s + 1) & (cN - 1);
            r = (r + 1) & (cN - 1);
            /* but buffer may not be empty. */
            if (--len)
                if(insert_node(sp, r) < 0) return -4;
        }
    } while (len > 0);   /* until length of string to be processed is zero */

    if (code_buf_ptr > 1) {    /* Send remaining code. */
        for (i = 0; i < code_buf_ptr; i++,ndst++){
            if (ndst < ndstend){
                dst[ndst] = code_buf[i];
                size++;
            }
            else {
                return  0;
                //return  size;//unable to compress, thus return orig size - 
            }
        }
    }


    }
    catch(err)
    { 
      output("---");
      output("[compress_lzss] Exception: " + err );
      output("---");
      output("\n");
      
      size=0;
    }


    return size;
}

//////////////////////////////////////////////////////
// DECOMPRESS                                       //
//////////////////////////////////////////////////////
// return: dst -- decompressed src
// http://stackoverflow.com/questions/1822350/what-is-the-javascript-operator-and-how-do-you-use-it
function decompress_lzss(src, srclen) 
{
    try
    {
    /* ring buffer of size N, with extra F-1 bytes to aid string comparison */
    var text_buf = new Array(cN + cF - 1);
    var dststart = 0;
    var srcend = srclen;
    var  i, j, r, c;
    var k;
    var flags;
    var dst = ""; // used when debugging to see values in string
    var decompressed_buf = new Array();
    var ttt=0;

    for (i = 0; i < cN - cF; i++)
        text_buf[i] = ' ';
    r = cN - cF;
    flags = 0;
    
    var nsrc = 0;
    var ndst = 0;
    for ( ; ; ) {
	flags=(flags >>= 1)&0x0000FFFF;   // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Bitwise_Operators	
        //if (((flags >>= 1) & 0x100) == 0) {
        if ((flags & 0x100) == 0) {
            if (nsrc < srcend) c = src[nsrc++]; else break;
            flags = c | 0xFF00;  /* uses higher byte cleverly */
        }   /* to count eight */
        if ((flags & 1)) {
            if (nsrc < srcend) c = src[nsrc++]; else break;
            //dst[ndst++] = c;
	    dst += c;
	    decompressed_buf[ttt++] = c;
            text_buf[r++] = c;
            r &= (cN - 1); // bitwise-and-assign; http://www.hermetic.ch/cfunlib/ast_amp2.htm
        } else {
            if (nsrc < srcend) i = src[nsrc++]; else break;
            if (nsrc < srcend) j = src[nsrc++]; else break;
            i |= ((j & 0xF0) << 4);
            j  = ((j & 0x0F) + THRESHOLD);
            for (k = 0; k <= j; k++) {
                c = text_buf[(i + k) & (cN - 1)];
                //dst[ndst++] = c;
		dst += c;
		decompressed_buf[ttt++] = c;
                text_buf[r++] = c;
                r &= (cN - 1);
            }
        }
    }
    
    //return ndst;
    //return dst;
    return decompressed_buf;
    }
    catch(err) 
    {
      output("---");
      output("[decompress_lzss] Exception: " + err );
      output("[decompress_lzss] Exception: result of decompress, txt : " + dst );
      output("---");
      output("\n");
      return 0; 
    }
}


