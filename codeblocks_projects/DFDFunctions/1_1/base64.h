#ifndef _BASE64_H_
#define _BASE64_H_

#include <vector>
#include <string>
typedef unsigned char BYTE;

std::string base64_encode(BYTE const* buf, unsigned int bufLen);
std::vector<BYTE> base64_decode(std::string const&);
std::string extractBase64ToImageFile(std::string filenameWithoutSuffixType, std::string strBase64Image);
bool extractBase64TojpgImagefile(std::string filenamepath, std::string strBase64Image);

#endif
