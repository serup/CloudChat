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
		bool bfirst=true;
		BOOST_FOREACH( block, vp )
		{
			if(aiid==0)
				strTransGUID = CMD5((const char*)block.first).GetMD5s();

			filenumber++;
			boost::property_tree::ptree pt = createBFiBlockRecord(bfirst, ++aiid, ++seq, strTransGUID, filename.c_str(),block.first, block.second);
			bfirst=false;
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

std::vector<unsigned char> readFile(std::string fn)
{
	std::vector<unsigned char> FileDataBytesInVector;
	std::ifstream is (fn, ios::binary);
	if (is)
	{
		long length = boost::filesystem::file_size(fn);
		std::cout << "[readFile] Reading file: " << fn << " ; amount " << length << " characters... \n";
        FileDataBytesInVector.resize(length,0); // Make sure receipient has room
		
		//read content of infile
		is.read ((char*)&FileDataBytesInVector[0],length);

		std::cout << "[readFile] size: " << (int) FileDataBytesInVector.size() << '\n';
		std::cout << "[readFile] capacity: " << (int) FileDataBytesInVector.capacity() << '\n';
		std::cout << "[readFile] max_size: " << (int) FileDataBytesInVector.max_size() << '\n';

		is.close();
	}
	return FileDataBytesInVector;

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
boost::property_tree::ptree CDataDictionaryControl::createBFiBlockRecord(bool bfirst,long aiid,long seq, std::string transGuid,std::string realmName, char* blob, int size)
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

	if(bfirst){
			cout << "ADD first BlockRecord entry" << endl;
			ptree &node = pt.add("BlockRecord", "");
			node.put("TransGUID",transGuid);
			node.put("chunk_id",realmName);
			node.put("aiid",aiid);
			node.put("chunk_seq",seq); // sequence number of particular item
			node.put("chunk_data.TransGUID",transGuid);
			node.put("chunk_data.Protocol", "DED");
			node.put("chunk_data.ProtocolVersion", "1.0.0.0");
			node.put("chunk_data.chunk_record.DataSize", nSizeOfHex); //TODO: consider dropping transfer to HEX, since it is making size bigger and only needed for Debug
			node.put("chunk_data.chunk_record.Data", data_in_hex_buf);
			node.put("chunk_data.chunk_record.DataMD5", strMD5);
			bfirst=false;
	}
	else {
			cout << "ADD node chunk_record" << endl;
			ptree &node = pt.add("chunk_record", "");
			node.put("DataSize", nSizeOfHex); //TODO: consider dropping transfer to HEX, since it is making size bigger and only needed for Debug
			node.put("Data", data_in_hex_buf);
			node.put("DataMD5", strMD5);
	}
	free(data_in_hex_buf);
	return pt;
}

std::vector< pair<unsigned char*, int> > CDataDictionaryControl::splitAttributIntoDEDchunks(long aiid, std::string attributName, std::vector<unsigned char>& attributValue, long maxDEDchunkSize)
{
    using boost::property_tree::ptree;
	ptree pt;

	//int iMaxChunkSize=300; /// TEST WITH SMALL SIZE
	int iMaxChunkSize=maxDEDchunkSize; 
	int iTotalSize=attributValue.size();
	int iBytesLeft=iTotalSize;
	int n=0;
	std::vector<unsigned char> chunkdata;
	unsigned long entity_chunk_seq= (unsigned long)0;
	bool bInternalFlush=false;
	vector< pair<unsigned char*,int> > listOfDEDchunks;
	int strangeCount=0;
	bool bError=false;

	do
	{
		chunkdata.clear();  // clean previous use

		if(iTotalSize>iMaxChunkSize){
				if(iMaxChunkSize>iBytesLeft){
						std::copy(attributValue.begin()+(n*iMaxChunkSize), attributValue.begin()+(n*iMaxChunkSize)+iBytesLeft, std::back_inserter(chunkdata));
				}
				else {
						std::copy(attributValue.begin()+(n*iMaxChunkSize), attributValue.begin()+(n*iMaxChunkSize)+iMaxChunkSize, std::back_inserter(chunkdata));
				}
		}
		else {
			std::copy(attributValue.begin(), attributValue.end(), std::back_inserter(chunkdata));
		}

		if(chunkdata.size()<=0){
				std::cout << "[splitAttributIntoDEDchunks] ERROR: NO data inserted in chunkdata, default <empty> added to field : " << attributName  << "\n";
				std::string strtmp="<empty>";
				std::copy(strtmp.begin(), strtmp.end(), std::back_inserter(chunkdata));
				strangeCount++;
				if(strangeCount>10) {
					bError=true; // avoid deadlock, due to errornous attribut
					std::cout << "[splitAttributIntoDEDchunks] ERROR: Aborting function !!!!" << "\n";
				}
		}

		n++;
		aiid++;
		entity_chunk_seq++;
		{ /// defined in DD_ATTRIBUT_TOAST.xml in datadictionary
			DED_START_ENCODER(encoder_ptr);
			DED_PUT_STRUCT_START( encoder_ptr, "chunk_record" );
				DED_PUT_STDSTRING	( encoder_ptr, "attribut_chunk_id", attributName ); // key of particular item
				DED_PUT_ULONG   	( encoder_ptr, "attribut_aiid", (unsigned long)aiid ); // this number is continuesly increasing all thruout the entries in this table
				DED_PUT_ULONG   	( encoder_ptr, "attribut_chunk_seq", (unsigned long)entity_chunk_seq ); // sequence number of particular item
				DED_PUT_STDVECTOR	( encoder_ptr, "attribut_chunk_data", chunkdata );
			DED_PUT_STRUCT_END( encoder_ptr, "chunk_record" );
			DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);
			if(sizeofCompressedData==0) sizeofCompressedData = iLengthOfTotalData; // if sizeofcompresseddata is 0 then compression was not possible and size is the same as for uncompressed
				iBytesLeft = iTotalSize-iMaxChunkSize*n;
			listOfDEDchunks.push_back(make_pair(pCompressedData,sizeofCompressedData));
	
		}
	}while(iBytesLeft>0 && !bError);

	return listOfDEDchunks;
}

//TODO: refactor chunks should go into BlockRecord and there should only be created more BlockRecords if chunks can not fit standard set size of a BlockRecord
boost::property_tree::ptree CDataDictionaryControl::addDEDchunksToBlockRecords(long aiid, std::string realmName, std::vector<pair<unsigned char*,int>>listOfDEDchunks, long maxBlockRecordSize)
{
    using boost::property_tree::ptree;
	std::string strTransGUID="";
	ptree pt;
	pt.clear();
	long bytesLeftInBlockRecord = maxBlockRecordSize;
	pair <unsigned char*,int> chunk;
	long seq=0; // every attribut starts with seq=1 and increses per chunk
	
	int iTotalSize=totalSizeOf(listOfDEDchunks);
	int iBytesLeft=iTotalSize;
	int n=0;
	bool bfirst=true;

	ptree &node = pt.add("listOfBlockRecords", "");
	node.put("chunksInBlockRecords",listOfDEDchunks.size());


	std::cout << "total size of DED chunks: " << iTotalSize << '\n';

	
	BOOST_FOREACH( chunk, listOfDEDchunks )
	{
		if(aiid==0) 
			strTransGUID = CMD5((const char*)chunk.first).GetMD5s();

		boost::property_tree::ptree subpt = createBFiBlockRecord(bfirst, ++aiid, ++seq, strTransGUID, realmName, (char*)chunk.first, chunk.second);

		iBytesLeft = iBytesLeft - chunk.second;
		std::cout << "bytes left : " << iBytesLeft << '\n';
		if(iBytesLeft > 0) {
			/// there is room for attribut chunk - add it to tree
		    // insert subpt at the end of the list
			std::cout << "appending chunk of size : " << chunk.second << '\n';
			if(bfirst)
				pt.insert(pt.get_child("listOfBlockRecords").end(),subpt.front());
			else
				pt.insert(pt.get_child("BlockRecord.chunk_data").end(),subpt.front());
		}
		bfirst=false;	
	}

//DEBUG	
// write test xml file
	ofstream blockFile ("xmlresult.xml", ios::out | ios::binary);
	write_xml(blockFile, pt);
	
	return pt;		
}

long CDataDictionaryControl::totalSizeOf(std::vector<pair<unsigned char*, int>> vectorPairList)
{
	long totalCount=0;		
	pair <unsigned char*,int> chunk;
	BOOST_FOREACH( chunk, vectorPairList )
	{
		totalCount+=chunk.second;
	}	
	return totalCount;
}

