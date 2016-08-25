#include "datadictionarycontrol.hpp"

CDataDictionaryControl::CDataDictionaryControl()
{
    //ctor
}

CDataDictionaryControl::~CDataDictionaryControl()
{
    //dtor
}

bool CDataDictionaryControl::CreateBlockFile(std::string filename)
{
	bool bResult=false;
	std::ofstream blockfile;
	if(!boost::filesystem::exists(filename))
	{
       	blockfile.open(filename.c_str());
		bResult = blockfile.is_open();
	}
	return bResult;
}

/**
 * splits the file into blocks resulting in <filename>_<number>.BFi files
 * 
 * returns amount of blocks or -1 if error
 */
int CDataDictionaryControl::splitFileIntoBlocks(std::string filename)
{
	int amountOfBlocks=-1;
	long aiid=0; // automatic id, increased for every record thru out the .BFi files
	long seq=0;
	std::string strTransGUID;

	vector< pair<char*, int> > vp = readFile(filename.c_str());
	if(vp.size()>0) {
		amountOfBlocks = vp.size();
		int filenumber=0;
		pair <char*,int> block;
		BOOST_FOREACH( block, vp )
		{
			if(aiid==0)
				strTransGUID = CMD5((const char*)block.first).GetMD5s();

			filenumber++;
			boost::property_tree::ptree pt = createBFiBlockRecord(++aiid, ++seq, strTransGUID, filename.c_str(),block.first, block.second);

			std::string blockfilename = filename + "_" + std::to_string(filenumber) + ".BFi";
		    ofstream blockFile (blockfilename.c_str(), ios::out | ios::binary);

			write_xml(blockFile, pt);
		}
		assert(filenumber == amountOfBlocks);
	}
	return amountOfBlocks;
}

vector< pair<char*, int> > CDataDictionaryControl::readFile(const char* fn)
{
	vector< pair<char*,int> > vpair;
	ifstream::pos_type size;
	char * memblock;
	ifstream file;
	file.open(fn,ios::in|ios::binary|ios::ate);
	if (file.is_open()) {
			size = boost::filesystem::file_size(fn);
			file.seekg (0, ios::beg);
			int bs = size/3; //TODO: use maxBlockSize to determine how many blocks a file should be split into 
			int ws = 0;
			int i = 0;
			cout<<"size:"<<size<<" bs:"<<bs<<endl;
			for(i = 0; i < size; i+=bs){
					if(i+bs > size)
							ws = size%bs;
					else
							ws = bs;
					cout<<"read:"<<ws<<endl;
					memblock = new char [ws];
					file.read (memblock, ws);
					vpair.push_back(make_pair(memblock,ws));
			}
	}
	else{
			std::string filename(fn);
			throw std::runtime_error("Error: Could NOT read file: " + filename );
	}
	return vpair;
}

/**
 * create xml structure for .BFi file and add blob to its payload
 *
 *  <BlockRecord>
 *  	<TransGUID></TransGUID> 			-- each record has a unique transaction id to be used with security, when detecting inconsistencies
 *     	<chunk_id></chunk_id>				-- name of realm file
 *     	<aiid></aiid>						-- automatic increased id - a continues number for all of these records
 *    	<chunk_seq></chunk_seq>				-- sequence number for this chunk of the database realm file
 *  	<chunk_data_in_hex>payload</chunk_data_in_hex> 	-- chunk data aka payload -- here is where blob is put, this blob should in turn also be a xml structure for DED toast element
 *  	<chunk_size></chunk_size>
 *  	<chunk_md5></chunk_md5>
 *  </BlockRecord>
 *  	   	   	   		  
 * return ptree - containing xml structure ready to be used by fx. write_xml
 */
boost::property_tree::ptree CDataDictionaryControl::createBFiBlockRecord(long aiid,long seq, std::string transGuid,std::string id, char* blob, int size)
{
	pair<char*,int> p;
	p = make_pair(blob, size);
    using boost::property_tree::ptree;
	ptree pt;
	pt.clear();

	long maxSize=4*size+1;
	int nSizeOfHex=0;
	char* data_in_hex_buf = (char*) malloc (maxSize);
	ZeroMemory(data_in_hex_buf,maxSize); // make sure no garbage is inside the newly allocated space
	const std::vector<unsigned char> iterator_data_in_hex_buf(&blob[0],&blob[size]);
	boost::algorithm::hex(iterator_data_in_hex_buf.begin(),iterator_data_in_hex_buf.end(), data_in_hex_buf);// convert the byte array to an array containing hex values in ascii
	std::string strMD5(CMD5((const char*)data_in_hex_buf).GetMD5s());
	std::string strtmp((const char*)data_in_hex_buf);
	nSizeOfHex = strtmp.size();

	ptree &node = pt.add("BlockRecord", 1);
	node.put("TransGUID",transGuid);
	node.put("chunk_id",id);
	node.put("aiid",aiid);
	node.put("chunk_seq",seq); // sequence number of particular item
	node.put("chunk_data_in_hex",data_in_hex_buf);
	node.put("chunk_size",nSizeOfHex);
	node.put("chunk_md5",strMD5);

	free(data_in_hex_buf);
	return pt;
}

