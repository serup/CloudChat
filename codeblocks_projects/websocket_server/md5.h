// MD5.h  http://www.faqs.org/rfcs/rfc1321.html

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef MD5h
#define MD5h

/* CMD5 Forms the Message Digest 5 (MD5) hash for given data.
CString S=CMD5("The Text").GetMD5s();

For some random string:
CString S=CMD5(itoa((long)GetTickCount())).GetMD5s();

CFileMD5 Finds the Message Digest of a whole File.

The Automatic Tester at the end of MD5.h uses standard Test Vectors.
*/
class CMD5 {
  BYTE Digest[16];
  char   sMD5[33];
public:
  CMD5() {*sMD5=0;}
  CMD5(const char* String) {Set(String);}
  virtual ~CMD5() {}

  struct MD5Context{
    DWORD state[4];          // state (ABCD)
    DWORD count[2];          // number of bits, modulo 2^64 (lsb first)
    unsigned char buffer[64];// input buffer
  };

  void Set(const char* String) {
    MD5Context Context;
    Init(&Context);
    Update(&Context, (BYTE*)String, strlen(String));
    Final(&Context);
    CreateString();
  }
         bool IsValid() const {return *sMD5!=0;}
  const char* GetMD5s() const {return  sMD5;}
  const BYTE* GetMD5n() const {return Digest;}

public:
//protected:
  void Init(MD5Context *Context) { // MD5 initialization. Begins an MD5 operation, writing a new Context.
    *sMD5=0;
    memset(Digest, 0, sizeof(Digest));
    Context->count[0]=Context->count[1]=0; // Load magic initialization constants.
    Context->state[0]=0x67452301;
    Context->state[1]=0xEFCDAB89;
    Context->state[2]=0x98BADCFE;
    Context->state[3]=0x10325476;
  }

  // MD5 block update operation.
// Continues an MD5 message-digest operation, processing another message block, and updating the Context.
  void Update(MD5Context *Context, unsigned char *input, DWORD inputLen) {
    unsigned int i, index, partLen;
    index=(unsigned int)((Context->count[0] >> 3) & 0x3F); // Compute number of bytes mod 64
    // Update number of bits:
    if((Context->count[0] += (inputLen << 3)) < (inputLen << 3)) Context->count[1]++;
    Context->count[1] += (inputLen >> 29);
    partLen=64-index;
    if(inputLen>=partLen) { // Transform as many times as possible:
      memcpy((unsigned char*)&Context->buffer[index], (unsigned char*)input, partLen);
      Transform(Context->state, Context->buffer);
      for(i=partLen; (i+63)<inputLen; i+=64) Transform(Context->state, &input[i]);
      index=0;
    }else i=0;
    // Buffer remaining input:
    if(inputLen>i) memcpy((unsigned char *)&Context->buffer[index], (unsigned char *)&input[i], inputLen-i);
  }

  void CreateString() {
    char* ptr=sMD5;
    BYTE* src=Digest;
    for(int i=0; i<16; i++) {
      BYTE b=*src++;
      BYTE B=b>>4;
      b&=0x0F;
      B=(B>9 ? B-10+'A' : B+'0');
      b=(b>9 ? b-10+'A' : b+'0');
      *ptr++=B;
      *ptr++=b;
    }
    *ptr=0;
  }

// MD5 finalization.
// Ends an MD5 message-digest operation, writing the the message digest and zeroing the Context.
  void Final(MD5Context *Context) {
    unsigned char bits[8];
    unsigned int index, padLen;
    static unsigned char Padding[64]={0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    Encode(bits, Context->count, 8); //Save number of bits
    index=(unsigned int)((Context->count[0] >> 3) & 0x3F); //Pad out to 56 mod 64.
    padLen=(index<56) ? (56-index) : (120-index);
    Update(Context, Padding, padLen);
    Update(Context, bits, 8); //Append length (before padding)
    Encode(Digest, Context->state, 16); // Store state in digest
    memset((unsigned char *)Context, 0, sizeof(*Context)); // Zero sensitive information.
  }

private:
  void Transform(DWORD state[4], unsigned char block[64]) {
    DWORD a=state[0], b=state[1], c=state[2], d=state[3], x[16];

    Decode(x, block, 64);

    const DWORD S11= 7;
    const DWORD S12=12;
    const DWORD S13=17;
    const DWORD S14=22;
    const DWORD S21= 5;
    const DWORD S22= 9;
    const DWORD S23=14;
    const DWORD S24=20;
    const DWORD S31= 4;
    const DWORD S32=11;
    const DWORD S33=16;
    const DWORD S34=23;
    const DWORD S41= 6;
    const DWORD S42=10;
    const DWORD S43=15;
    const DWORD S44=21;

  // Round 1
    FF(a,b,c,d,x[ 0],S11,0xD76AA478); // 1
    FF(d,a,b,c,x[ 1],S12,0xE8C7B756); // 2
    FF(c,d,a,b,x[ 2],S13,0x242070DB); // 3
    FF(b,c,d,a,x[ 3],S14,0xC1BDCEEE); // 4
    FF(a,b,c,d,x[ 4],S11,0xF57C0FAF); // 5
    FF(d,a,b,c,x[ 5],S12,0x4787C62A); // 6
    FF(c,d,a,b,x[ 6],S13,0xA8304613); // 7
    FF(b,c,d,a,x[ 7],S14,0xFD469501); // 8
    FF(a,b,c,d,x[ 8],S11,0x698098D8); // 9
    FF(d,a,b,c,x[ 9],S12,0x8B44F7AF); // 10
    FF(c,d,a,b,x[10],S13,0xFFFF5BB1); // 11
    FF(b,c,d,a,x[11],S14,0x895CD7BE); // 12
    FF(a,b,c,d,x[12],S11,0x6B901122); // 13
    FF(d,a,b,c,x[13],S12,0xFD987193); // 14
    FF(c,d,a,b,x[14],S13,0xA679438E); // 15
    FF(b,c,d,a,x[15],S14,0x49B40821); // 16

  // Round 2
    GG(a,b,c,d,x[ 1],S21,0xF61E2562); // 17
    GG(d,a,b,c,x[ 6],S22,0xC040B340); // 18
    GG(c,d,a,b,x[11],S23,0x265E5A51); // 19
    GG(b,c,d,a,x[ 0],S24,0xE9B6C7AA); // 20
    GG(a,b,c,d,x[ 5],S21,0xD62F105D); // 21
    GG(d,a,b,c,x[10],S22,0x02441453); // 22
    GG(c,d,a,b,x[15],S23,0xD8A1E681); // 23
    GG(b,c,d,a,x[ 4],S24,0xE7D3FBC8); // 24
    GG(a,b,c,d,x[ 9],S21,0x21E1CDE6); // 25
    GG(d,a,b,c,x[14],S22,0xC33707D6); // 26
    GG(c,d,a,b,x[ 3],S23,0xF4D50D87); // 27

    GG(b,c,d,a,x[ 8],S24,0x455A14ED); // 28
    GG(a,b,c,d,x[13],S21,0xA9E3E905); // 29
    GG(d,a,b,c,x[ 2],S22,0xFCEFA3F8); // 30
    GG(c,d,a,b,x[ 7],S23,0x676F02D9); // 31
    GG(b,c,d,a,x[12],S24,0x8D2A4C8A); // 32

  // Round 3
    HH(a,b,c,d,x[ 5],S31,0xFFFA3942); // 33
    HH(d,a,b,c,x[ 8],S32,0x8771F681); // 34
    HH(c,d,a,b,x[11],S33,0x6D9D6122); // 35
    HH(b,c,d,a,x[14],S34,0xFDE5380C); // 36
    HH(a,b,c,d,x[ 1],S31,0xA4BEEA44); // 37
    HH(d,a,b,c,x[ 4],S32,0x4BDECFA9); // 38
    HH(c,d,a,b,x[ 7],S33,0xF6BB4B60); // 39
    HH(b,c,d,a,x[10],S34,0xBEBFBC70); // 40
    HH(a,b,c,d,x[13],S31,0x289B7EC6); // 41
    HH(d,a,b,c,x[ 0],S32,0xEAA127FA); // 42
    HH(c,d,a,b,x[ 3],S33,0xD4EF3085); // 43
    HH(b,c,d,a,x[ 6],S34,0x04881D05); // 44
    HH(a,b,c,d,x[ 9],S31,0xD9D4D039); // 45
    HH(d,a,b,c,x[12],S32,0xE6DB99E5); // 46
    HH(c,d,a,b,x[15],S33,0x1FA27CF8); // 47
    HH(b,c,d,a,x[ 2],S34,0xC4AC5665); // 48

  // Round 4
    II(a,b,c,d,x[ 0],S41,0xF4292244); // 49
    II(d,a,b,c,x[ 7],S42,0x432AFF97); // 50
    II(c,d,a,b,x[14],S43,0xAB9423A7); // 51
    II(b,c,d,a,x[ 5],S44,0xFC93A039); // 52
    II(a,b,c,d,x[12],S41,0x655B59C3); // 53
    II(d,a,b,c,x[ 3],S42,0x8F0CCC92); // 54
    II(c,d,a,b,x[10],S43,0xFFEFF47D); // 55
    II(b,c,d,a,x[ 1],S44,0x85845DD1); // 56
    II(a,b,c,d,x[ 8],S41,0x6FA87E4F); // 57
    II(d,a,b,c,x[15],S42,0xFE2CE6E0); // 58
    II(c,d,a,b,x[ 6],S43,0xA3014314); // 59
    II(b,c,d,a,x[13],S44,0x4E0811A1); // 60
    II(a,b,c,d,x[ 4],S41,0xF7537E82); // 61
    II(d,a,b,c,x[11],S42,0xBD3AF235); // 62
    II(c,d,a,b,x[ 2],S43,0x2AD7D2BB); // 63
    II(b,c,d,a,x[ 9],S44,0xEB86D391); // 64

    state[0]+=a;
    state[1]+=b;
    state[2]+=c;
    state[3]+=d;

    memset ((unsigned char*)x, 0, sizeof (x)); // Zero sensitive information.
  }

// Encodes input (DWORD) into output (unsigned char). Assumes len is a multiple of 4.
  void Encode(unsigned char *output, DWORD *input, unsigned int len) {
    unsigned i,j;
    for (i=j=0; j<len; ++i, j+=4) {
      output[j]=  (unsigned char)( input[i]      & 0xFF);
      output[j+1]=(unsigned char)((input[i]>> 8) & 0xFF);
      output[j+2]=(unsigned char)((input[i]>>16) & 0xFF);
      output[j+3]=(unsigned char)((input[i]>>24) & 0xFF);
  } }

// Decodes input (unsigned char) into output (DWORD). Assumes len is a multiple of 4.
  void Decode(DWORD *output, unsigned char *input, unsigned int len) {
    unsigned int i, j;
    for(i=j=0; j<len; ++i, j+=4) {
      output[i]=((DWORD)input[j  ])
             | (((DWORD)input[j+1])<< 8)
             | (((DWORD)input[j+2])<<16)
             | (((DWORD)input[j+3])<<24);
  } }

// F, G, H and I are basic MD5 functions.
  DWORD F(DWORD x, DWORD y, DWORD z) {return (x & y) | (~x &  z);}
  DWORD G(DWORD x, DWORD y, DWORD z) {return (x & z) | ( y & ~z);}
  DWORD H(DWORD x, DWORD y, DWORD z) {return x^y^z;}
  DWORD I(DWORD x, DWORD y, DWORD z) {return y^(x | ~z);}

// RotateLeft rotates x left n bits.
  void RotateLeft(DWORD& x, DWORD n) {x=(x<<n)|(x>>(32-n));}

// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
// Rotation is separate from addition to prevent re-computation.
  void FF(DWORD& a, DWORD b, DWORD c, DWORD d, DWORD x, DWORD s, DWORD ac) {a+=F(b,c,d)+x+ac; RotateLeft(a,s); a+=b;}
  void GG(DWORD& a, DWORD b, DWORD c, DWORD d, DWORD x, DWORD s, DWORD ac) {a+=G(b,c,d)+x+ac; RotateLeft(a,s); a+=b;}
  void HH(DWORD& a, DWORD b, DWORD c, DWORD d, DWORD x, DWORD s, DWORD ac) {a+=H(b,c,d)+x+ac; RotateLeft(a,s); a+=b;}
  void II(DWORD& a, DWORD b, DWORD c, DWORD d, DWORD x, DWORD s, DWORD ac) {a+=I(b,c,d)+x+ac; RotateLeft(a,s); a+=b;}
};

struct CFileMD5 : public CMD5 {
  CFileMD5() {}
  CFileMD5(const HANDLE File) {Load(File);}
  CFileMD5(const char*  Path) {Load(Path);}
  virtual ~CFileMD5() {}
  bool Load(const char* Path) {
    HANDLE File=CreateFile(Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(File==INVALID_HANDLE_VALUE) return false;
    bool OK=Load(File);
    CloseHandle(File);
    return OK;
  }
  bool Load(const HANDLE File) {
    if(File==INVALID_HANDLE_VALUE) return false;
#if _MSC_VER > 1200
    LARGE_INTEGER Size,Pos={0};
    SetFilePointerEx(File,Pos,&Pos,FILE_CURRENT); // find out how far from the End of the File the File Pointer is.
    GetFileSizeEx(File,&Size);
#else
    DWORD PosHi=0;
    DWORD PosLo=SetFilePointer(File,0,(PLONG)&PosHi,FILE_CURRENT); // find out how far from the End of the File the File Pointer is.
    DWORD SizeHi;
    DWORD SizeLo=GetFileSize(File,&SizeHi);
#endif
    DWORD ByteCount;
    unsigned char Buffer[64];
    MD5Context Context;
    Init(&Context);
#if _MSC_VER > 1200
    if((Size.HighPart> Pos.HighPart)
    || ((Size.HighPart==Pos.HighPart) && (Size.LowPart>Pos.LowPart))) {
#else
    if((SizeHi> PosHi)
    ||((SizeHi==PosHi) && (SizeLo>PosLo))) {
#endif
      do {
        ReadFile(File, Buffer, 64, &ByteCount, NULL);
        Update(&Context, Buffer, ByteCount);
      } while(ByteCount==64);
    }
    Final(&Context);
    CreateString();
    return true;
  }
};

#ifdef Assert // Run tests if Tester.h is included in stdafx.h
namespace { // namespace prevents multiple definitions if instantiating in the header like this:
  struct MD5Tester : Tester {
    MD5Tester() {
      Assert(strcmp(CMD5("").GetMD5s(), "D41D8CD98F00B204E9800998ECF8427E")==0);
      Assert(strcmp(CMD5("a").GetMD5s(), "0CC175B9C0F1B6A831C399E269772661")==0);
      Assert(strcmp(CMD5("abc").GetMD5s(), "900150983CD24FB0D6963F7D28E17F72")==0);
      Assert(strcmp(CMD5("message digest").GetMD5s(), "F96B697D7CB7938D525A2F31AAF161D0")==0);
      Assert(strcmp(CMD5("abcdefghijklmnopqrstuvwxyz").GetMD5s(), "C3FCD3D76192E4007DFB496CCA67E13B")==0);
      Assert(strcmp(CMD5("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789").GetMD5s(), "D174AB98D277D9F5A5611C2C9F419D9F")==0);
      Assert(strcmp(CMD5("12345678901234567890123456789012345678901234567890123456789012345678901234567890").GetMD5s(), "57EDF4A22BE3C955AC49DA2E2107B67A")==0);
    }
  } _MD5Tester;
}
#endif // def Assert

#endif //ndef MD5h
