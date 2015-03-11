/*
 *  Copyright 2011 Twitter, Inc.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

// http://docs.oseems.com/general/application/vim/auto-complete-javascript
// auto-complete JavaScript syntaxes by pressing Ctrl + x followed by Ctrl + o while partially typing JavaScript codes. - See more at: http://docs.oseems.com/general/application/vim/auto-complete-javascript#sthash.8iMXwNkB.dpuf
//
var Hogan = Hogan || require('./lib/hogan')
  , doc = this["document"]



function testlocal_adler32() {

  buffer = new Array();
  buffer['a1']='foo';
  buffer['a2']='bar';
  
  var result = local_adler32(buffer, buffer.length);
  if(result > 0)
	  result = 1;
  var expected = 1;
  is(result, expected, "local_adler32 return expected");
  delete buffer;
}

function testinit_state() {
// http://answers.unity3d.com/questions/198318/javascript-array-use-with-a-struct-.html
//
// // http://answers.unity3d.com/questions/23044/struct-in-javascript.html
//TODO: deside if node.js should be used in client javascript
// http://nodejs.org/api/buffer.html
// http://nodejs.org/api/buffer.html#buffer_buf_fill_value_offset_end
/* http://www.phpied.com/3-ways-to-define-a-javascript-class/
 * It's important to note that there are no classes in JavaScript. Functions can be used to somewhat simulate classes, but in general JavaScript is a class-less language. Everything is an object. And when it comes to inheritance, objects inherit from objects, not classes from classes as in the "class"-ical languages.
 */
//
// http://stackoverflow.com/questions/502366/structs-in-javascript
//
// http://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Storer%E2%80%93Szymanski
// Lempel–Ziv–Storer–Szymanski (LZSS) is a lossless data compression algorithm, a derivative of LZ77, that was created in 1982 by James Storer and Thomas Szymanski. LZSS was described in article "Data compression via textual substitution" published in Journal of the ACM (pp. 928–951).
//

/*	
    struct encode_state {
        //
        // left & right children & parent. These constitute binary search trees.
        //
        int lchild[cN + 1], rchild[cN + 257], parent[cN + 1];

        // ring buffer of size N, with extra F-1 bytes to aid string comparison
        //
        __u_int8_t text_buf[cN + cF - 1];

        //
        // match_length of longest match.
        // These are set by the insert_node() procedure.
        //
        int match_position, match_length;
    };
*/

    var cN =       4096;  /* size of ring buffer - must be power of 2 */
    var cF =       18;    /* upper limit for match_length */
    var THRESHOLD = 2;    /* encode string into position and length
	                     if match_length is greater than this */
    var NIL       = cN;   /* index for root of binary search trees */


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

    //+ test of struct in javascript
    var o = {
      'a': 3, 'b': 4,
      'doStuff': function() {
	alert(this.a + this.b); // displays: 7
	var Item = makeStruct("id speaker country");
	var row = new Item(1, 'john', 'au');
	alert(row.speaker); // displays: john
      }
    };
    //-

    //
    // left & right children & parent. These constitute binary search trees.
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
    var encode_state = new struct_encode_state(lchild, rchild, _parent, text_buf, match_position, match_length);
    
    /////////////////////////////////////////
    // Test function 
    /////////////////////////////////////////
    var result = init_state(encode_state);
    if (result > 0)
      result = 1;

    var expected = 1;
    is(result, expected, "init_state return expected");
}

function testdelete_node() {
// http://stackoverflow.com/questions/8318357/javascript-pointer-reference-craziness-can-someone-explain-this
    var cN =       4096;  /* size of ring buffer - must be power of 2 */
    var cF =       18;    /* upper limit for match_length */
    var THRESHOLD = 2;    /* encode string into position and length
	                     if match_length is greater than this */
    var NIL       = cN;   /* index for root of binary search trees */


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

    //
    // left & right children & parent. These constitute binary search trees.
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
    var encode_state = new struct_encode_state(lchild, rchild, _parent, text_buf, match_position, match_length);
   
    
    /* initialize trees */
    sp = encode_state;
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

    /* Clear the buffer with any character that will appear often. */
    s = 0;  r = cN - cF;

    //var src = new Array(200);
    var txt="I am Sam\nSam I am\n\nThat Sam-I-am!\nThat Sam-I-am!\nI do not like\nthat Sam-I-am!\n\nDo you like green eggs and ham?\n\nI do not like them, Sam-I-am.\nI do not like green eggs and ham."

   /* var src = new Array();
    src[0] = "h";
    src[1] = "e";
    src[2] = "l";
    src[3] = "l";
    src[4] = "o";*/
    var src = txt;	   
    nsrclen = src.length;
    nsrcend = nsrclen;

    /* Read F bytes into the last F bytes of the buffer */
    for (len = 0, nsrc = 0; len < cF && nsrc < nsrcend; len++, nsrc++)
        sp.text_buf[r + len] = src[nsrc]; 
   
    var result = -1;
    /*
     * Insert the F strings, each of which begins with one or more
     * 'space' characters.  Note the order in which these strings are
     * inserted.  This way, degenerate trees will be less likely to occur.
     */
    for (i = 1; i <= cF; i++)
        insert_node(sp, r - i);

    /////////////////////////////////////////
    // Test function 
    /////////////////////////////////////////
    s = 0;
    /* Since this is a ring buffer, increment the position modulo N. */
    //s = (s + 1) & (cN - 1);
    //s = 4060;
    s = r - 1;
    result = delete_node(sp, s);
    

    var expected = 0;
    is(result, expected, "delete_node return expected");
}


function testinsert_node() {
// http://stackoverflow.com/questions/8318357/javascript-pointer-reference-craziness-can-someone-explain-this
    var cN =       4096;  /* size of ring buffer - must be power of 2 */
    var cF =       18;    /* upper limit for match_length */
    var THRESHOLD = 2;    /* encode string into position and length
	                     if match_length is greater than this */
    var NIL       = cN;   /* index for root of binary search trees */


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

    //
    // left & right children & parent. These constitute binary search trees.
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
    var encode_state = new struct_encode_state(lchild, rchild, _parent, text_buf, match_position, match_length);
   
    
    /////////////////////////////////////////
    // Test function 
    /////////////////////////////////////////
    /* initialize trees */
    sp = encode_state;
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

    /* Clear the buffer with any character that will appear often. */
    s = 0;  r = cN - cF;

    nsrc = 0;
    var src = new Array(200);
   /* src[0] = "h";
    src[1] = "e";
    src[2] = "l";
    src[3] = "l";
    src[4] = "o";*/
    nsrclen = src.length;
    nsrcend = nsrclen;

    /* Read F bytes into the last F bytes of the buffer */
    for (len = 0; len < cF && nsrc < nsrcend; len++)
        sp.text_buf[r + len] = src[nsrc]; nsrc++;
   
    var result = -1;
    /*
     * Insert the F strings, each of which begins with one or more
     * 'space' characters.  Note the order in which these strings are
     * inserted.  This way, degenerate trees will be less likely to occur.
     */
    for (i = 1; i <= cF; i++)
        result = insert_node(sp, r - i);

    var expected = 2;
    is(result, expected, "insert_node return expected");
}

function testcompress_lzss() {
  var result = -1;
  var txt="I am Sam\nSam I am\n\nThat Sam-I-am!\nThat Sam-I-am!\nI do not like\nthat Sam-I-am!\n\nDo you like green eggs and ham?\n\nI do not like them, Sam-I-am.\nI do not like green eggs and ham."
  var src = txt;	  
  var srclen = src.length;  
  var expected = 1;
  var dstlen = txt.length;
  var dst = new Array(dstlen);

  result = compress_lzss(dst, dstlen, src, srclen);
  if(result > 0) result = 1;
  is(result, expected, "compress_lzss return expected");
}

function testdecompress_lzss() { // NB!  Danish characters is a problem !!!!!
 
  //var txt="I am Sam\nSam I am\n\nThat Sam-I-am!\nThat Sam-I-am!\nI do not like\nthat Sam-I-am!\n\nDo you like green eggs and ham?\n\nI do not like them, Sam-I-am.\nI do not like green eggs and ham."
  var txt="I am Sam\nSam I am\n\nThat Sam-I-am!\nThat Sam-I-am!\nI do not like\nthat Sam-I-am!\n\nDo you like green eggs and ham?\n\nI do not like them, Sam-I-am.\nI do not like green eggs and ham.aabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeffgggggggggggggggggggggggggggggggggHHHHhhhhhIIIIiiiiiiJJJJjjjjjkllmnopqqqqqrRRRRsssstuvWvvvvvWWWWWxxxXXXXyyyyyYYYZZZZzzzzz"
  //var txt="aaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbaaaaaaaabbbbbbbaaabbbbbbbbbbaaaacccccccccccccccccddddddccccddddeeeee";
  //var txt="am am am am am am am am am am"
  //var txt="aaaaaaaaaa"
  //var txt="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
  //var txt="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbb";
  //var txt="aabbccddeeffaabbccddeeffaabbccddeeffaabbccddeeff";
  var src = txt;	  
  var cmprsd = new Array(txt.length);
  var decmprsd = new Array(txt.length*2);
  //var dst = new Array(txt.length);


  // First compress
  var dcmprsdSize = compress_lzss(cmprsd, cmprsd.length, src, src.length);
  if(dcmprsdSize>0) expected = 1;
  var result = 1;
  is(result, expected, "compress_lzss return expected");

  // Then decompress 
  var dst = decompress_lzss(cmprsd, dcmprsdSize);
  result = dst;
  var src2 = "";
  for(i = 0; i < dcmprsdSize; i++) src2 += dst[i]; // convert to string 
  //if(result.length > 0) result = 1;
  //var expected = 1;
  //is(result, expected, "decompress_lzss return expected");
  is(src2, src, "decompress_lzss decompress success");

  delete dst;
  delete cmprsd;
}

function appendText(el, text) {
  var textNode = document.createTextNode(text);
  el.appendChild(textNode);
  el.appendChild(document.createElement('br'));
}

if (!this["output"]) {
  var output = function (s) {
    return doc ? appendText(doc.getElementById('console'), s) : console.log(s);
  };
}
var passed = 0;
var failed = 0;

function isInXML(description, got, expected) {
  if (got === expected) {
    output("<TestCase name=\"" + description + "\" result=\"passed\" assertions_passed=\"1\" assertions_failed=\"0\" expected_failures=\"0\"></TestCase>");
//    ++passed;
  } else {
    output("<TestCase name=\"" + description + "\" result=\"failed\" assertions_passed=\"1\" assertions_failed=\"0\" expected_failures=\"0\"></TestCase>");
//    ++failed;
  }
}

function is(got, expected, msg) {
  if (got === expected) {
    output("OK:   " + msg);
    ++passed;
  } else {
    output("FAIL: " + msg);
    output("Expected |" + expected + "|");
    output("     Got |" + got + "|");
    ++failed;
  }
  isInXML(msg, got, expected);
}



function complete() {
  output("--------------");
  output("\nTests Complete");
  output("--------------");
  output("Passed: " + passed);
  output("Failed: " + failed);
  output("--------------");
  output("\n");
}

// How to debug :
// https://developers.google.com/chrome-developer-tools/docs/javascript-debugging#breakpoints
function runTests() {
  output("--------------");
  output("Test of compression - testing okumura compression aka  LempelZivStorerSzymanski (LZSS) is a lossless data compression algorithm, hoffman coding");
  output("--------------");
  testlocal_adler32();
  testinit_state();
  testinsert_node();
  testdelete_node();
  testcompress_lzss();
  testdecompress_lzss();
/*  testScanTextNoTags();
  testScanOneTag();
  testScanMultipleTags();
  testScanSectionOpen();
  testScanSectionClose();
  testScanSection();
  testScanSectionInContent();
  testScanNegativeSection();
  testScanPartial();
  testScanBackwardPartial();
  testScanAmpersandNoEscapeTag();
  testScanTripleStache();
  testScanSectionWithTripleStacheInside();
  testScanSetDelimiter();
  testScanResetDelimiter();
  testSetDelimiterWithWhitespace();
  testSingleCharDelimiter();
  testParseBasic();
  testParseVariables();
  testParseSection();
  testParseIndexes();
  testParseNegativeSection();
  testParseNestedSections();
  testMissingClosingTag();
  testBadNesting();
  testBasicOutput();
  testOneVariable();
  testMultipleVariables();
  testNumberValues();
  testObjectRender();
  testObjectToStringRender();
  testArrayRender();
  testEscaping();
  testMustacheInjection();
  testTripleStache();
  testAmpNoEscaping();
  testPartial();
  testNestedPartials();
  testNegativeSection();
  testSectionElision();
  testSectionObjectContext();
  testSectionArrayContext();
  testRenderWithWhitespace();
  testRenderWithWhitespaceAroundTripleStache();
  testRenderWithWhitespaceAroundAmpersand();
  testFalsyVariableNoRender();
  testRenderOutput();
  testDefaultRenderImpl();
  testSectionExtensions();
  testMisnestedSectionExtensions();
  testNestedSection();
  testShootOutString();
  testShootOutReplace();
  testShootOutArray();
  testShootOutObject();
  testShootOutPartial();
  testShootOutRecurse();
  testShootOutFilter();
  testShootOutComplex();
  testDottedNames();
  testImplicitIterator();
  testPartialsAndDelimiters();
  testStringPartials();
  testMissingPartials();
  testIndentedStandaloneComment();
  testNewLineBetweenDelimiterChanges();
  testMustacheJSApostrophe();
  testMustacheJSArrayOfImplicitPartials();
  testMustacheJSArrayOfPartials();
  testMustacheJSArrayOfStrings();
  testMustacheJSUndefinedString();
  testMustacheJSTripleStacheAltDelimiter();
  */
  complete();
}

if (doc) {
  window.onload = runTests;
} else {
  runTests();
}
