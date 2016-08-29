#ifndef CDATADICTIONARYCONTROL_H
#define CDATADICTIONARYCONTROL_H

#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))
#define ZeroMemory RtlZeroMemory


#include <string>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <stdexcept>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/hex.hpp>
#include "md5.h"
#include <boost/property_tree/xml_parser.hpp>
#include "../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h"
#include "../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h"

using namespace std;

struct BlockRecordEntry{
	std::string TransGUID;
	std::string chunk_id;
	unsigned long aiid;
	unsigned long chunk_seq;
	std::string chunk_data_in_hex;
	unsigned long chunk_size;
	std::string chunk_md5;
};
typedef std::vector<BlockRecordEntry> BlockRecord;

class CDataDictionaryControl
{
    public:
        /** Default constructor */
        CDataDictionaryControl();
        /** Default destructor */
        virtual ~CDataDictionaryControl();

	bool CreateBlockFile(std::string filename);
	int splitFileIntoBlocks(std::string filename);
	boost::property_tree::ptree createBFiBlockRecord(long aiid, long seq, std::string transGuid,std::string id, char* blob, int size);
	std::vector< pair<unsigned char*,int> > splitAttributIntoDEDchunks(long aiid, std::string attributName, std::vector<unsigned char>& attributValue, long maxDEDchunkSize);
	boost::property_tree::ptree addDEDchunksToBlockRecords(long aiid, std::string attributName, std::vector<pair<unsigned char*,int>>listOfDEDchunks, long maxDEDblockSize);

	
    protected:
	
    private:
	vector< pair<char*, int> > readFile(const char* fn);
	std::vector<unsigned char> readFile(std::string fn);
};

#endif // CDATADICTIONARYCONTROL_H
