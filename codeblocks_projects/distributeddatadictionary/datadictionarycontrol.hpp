#ifndef CDATADICTIONARYCONTROL_H
#define CDATADICTIONARYCONTROL_H

#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))
#define ZeroMemory RtlZeroMemory


#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <exception>
#include <stdexcept>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/detail/xml_parser_writer_settings.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/log/trivial.hpp>
#include "md5.h"
#ifndef DED
#define DED
#include "../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h"
#include "../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h"
#endif
#include "utils.hpp"

using namespace std;
using boost::optional;
using boost::property_tree::ptree;
	
const string BFI_FILE_EXTENSION = ".BFi";
const string BFI_BLOCK_ENTITY = "BFi";
const string DATADICTIONARY_ID = "ddid";
const string FILENAME_SEPERATOR = "_";

struct BlockRecordEntry{
	std::string TransGUID;
	std::string chunk_id;
	unsigned long aiid;
	unsigned long chunk_seq;
	std::string chunk_data_in_hex;
	unsigned long chunk_size;
	std::string chunk_md5;
	std::string chunk_ddid;
};
typedef std::vector<BlockRecordEntry> BlockRecord;

struct chunk_record_entries
{
	std::string chunk_ddid;
	unsigned int DataSize;
	std::string Data;
	std::string DataMD5;
};

struct EntityChunkDataInfo{
	std::string entity_chunk_id;
	unsigned long aiid;
	unsigned long entity_chunk_seq;
	std::vector<unsigned char> entity_chunk_data;
};

struct assembledElements {
	std::string strElementID;
	std::list<unsigned long> seqNumbers;
	std::vector<unsigned char> ElementData;
};

struct seqSpan {
	std::list<unsigned long> seqNumbers;
	std::string attributPath;
};

class CDataDictionaryControl
{
    public:
        CDataDictionaryControl();
        virtual ~CDataDictionaryControl();

		bool CreateBlockFile(std::string filename);
		int splitFileIntoBlocks(std::string filename);
		string generateBlockFileName(string filename, int filenumber);
	  	boost::property_tree::ptree createBFiBlockRecord(bool bfirst,long aiid, long seq, std::string transGuid,std::string ddid, std::string realmName, char* blob, int size);
	  	std::vector< pair<std::vector<unsigned char>,int> > splitAttributIntoDEDchunks(long &aiid, std::string attributName, std::vector<unsigned char>& attributValue, long maxDEDchunkSize);
	  	boost::property_tree::ptree addDEDchunksToBlockRecords(long &aiid, std::string realmName, std::string ddid, std::vector<pair<std::vector<unsigned char>,int>>listOfDEDchunks, long maxBlockRecordSize);
	  	bool addDEDchunksToBlockRecords(std::string transGuid, boost::property_tree::ptree &pt, long &aiid, std::string realmName, std::string ddid, std::vector<pair<std::vector<unsigned char>,int>>listOfDEDchunks, long &maxBlockRecordSize);
	  	boost::property_tree::ptree addBlockRecordToBlockEntity(std::string transGuid, boost::property_tree::ptree &pt, long maxBlockEntitySize);
	  	long fetchBlockRecordSize(boost::property_tree::ptree::value_type &vt);
	  	std::vector< pair<std::string ,int> > writeBlockEntityToBFiFile(boost::property_tree::ptree &ptBlockEntities);
	  	bool addAttributToBlockRecord(std::string transGuid, boost::property_tree::ptree &ptListOfBlockRecords, long &maxBlockRecordSize, std::string realmName, std::string attributName, std::vector<unsigned char> attributValue);
	  
	  	std::list<std::string> ls();
		std::vector<assembledElements> readBlockRecordElements(boost::property_tree::ptree::value_type &vt2); 
		bool findElement(std::vector<assembledElements>& _Elements, std::string strElementID);
		std::vector<unsigned char> fetchElement(std::vector<assembledElements>& _Elements, std::string strElementID);
		pair<std::string, std::vector<unsigned char>> ftgt(std::string attributpath);
	
    protected:
		long _maxDEDchunkSize;

    private:

		vector< pair<char*, int> > readFile(const char* fn);
		std::vector<unsigned char> readFile(std::string fn);
		long totalSizeOf(std::vector<pair<std::vector<unsigned char>, int>> vectorPairList);
		bool appendChunkRecordToLastBlockRecordsChunkData(boost::property_tree::ptree &pt, boost::property_tree::ptree &subpt);
		bool appendToLastBlockEntity(boost::property_tree::ptree &node, boost::property_tree::ptree &subpt, std::string transGuid);
		void setMaxDEDchunkSize(long maxSize);
		long getMaxDEDchunkSize();
		vector<pair<unsigned long, std::vector<unsigned char> >> filterAndSortAssembledRecords(std::string attributpath, std::list< pair<seqSpan, std::vector<assembledElements>> > listOfRecords);
		bool mergeRecords(vector<pair<unsigned long, std::vector<unsigned char> >> list, std::vector<unsigned char> &ElementData);
		bool assembleBlockRecords(std::string transGuid, std::string id, std::vector<assembledElements> &recElements,  std::list< pair<seqSpan, std::vector<assembledElements>> > &listOfAssembledAttributes);
		pair<std::string, std::vector<unsigned char>> findAndAssembleAttributFromBFiFiles( std::string attributpath, boost::filesystem::path _targetDir );
		std::list< pair<seqSpan, std::vector<assembledElements>> > fetchAttributBlocksFromBFiFiles(std::string attributpath, boost::filesystem::path _targetDir);
		bool addAttributFromBFiToList(ptree pt, std::list< pair<seqSpan, std::vector<assembledElements>> > &listOfAssembledAttributes);

};


#endif // CDATADICTIONARYCONTROL_H
