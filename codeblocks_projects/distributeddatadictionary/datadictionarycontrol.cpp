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
			boost::property_tree::ptree pt = createBFiBlockRecord(bfirst, ++aiid, ++seq, strTransGUID, "ddid", filename.c_str(),block.first, block.second);
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
boost::property_tree::ptree CDataDictionaryControl::createBFiBlockRecord(bool bfirst,long aiid,long seq, std::string transGuid,std::string ddid, std::string realmName, char* blob, int size)
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
			node.put("chunk_seq",seq); // sequence number of particular BlockRecord 
			node.put("chunk_data.TransGUID",transGuid);
			node.put("chunk_data.Protocol", "DED");
			node.put("chunk_data.ProtocolVersion", "1.0.0.0");
			node.put("chunk_data.chunk_record.chunk_ddid", ddid); 
			node.put("chunk_data.chunk_record.DataSize", nSizeOfHex); //TODO: consider dropping transfer to HEX, since it is making size bigger and only needed for Debug
			node.put("chunk_data.chunk_record.Data", data_in_hex_buf);
			node.put("chunk_data.chunk_record.DataMD5", strMD5);
			bfirst=false;
	}
	else {
			cout << "ADD node chunk_record" << endl;
			ptree &node = pt.add("chunk_record", "");
			node.put("chunk_ddid", ddid); 
			node.put("DataSize", nSizeOfHex); //TODO: consider dropping transfer to HEX, since it is making size bigger and only needed for Debug
			node.put("Data", data_in_hex_buf);
			node.put("DataMD5", strMD5);
	}
	free(data_in_hex_buf);
	return pt;
}

std::vector< pair<unsigned char*, int> > CDataDictionaryControl::splitAttributIntoDEDchunks(long &aiid, std::string attributName, std::vector<unsigned char>& attributValue, long maxDEDchunkSize)
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

	cout << "--- attributValue size : " << attributValue.size() << endl;

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
		cout << "--- chunkdata size : " << chunkdata.size() << endl;
		cout << "--- adding to DED attributName : " << attributName << endl;
		cout << "--- adding to DED aiid: " << aiid << endl;
		cout << "--- adding to DED seq:  " << entity_chunk_seq << endl;
		{ /// defined in DD_ATTRIBUT_TOAST.xml in datadictionary
			DED_START_ENCODER(encoder_ptr);
			DED_PUT_STRUCT_START( encoder_ptr, "chunk_record" );
				DED_PUT_STDSTRING	( encoder_ptr, "attribut_chunk_id", attributName ); // key of particular item
				DED_PUT_ULONG   	( encoder_ptr, "attribut_aiid", (unsigned long)aiid ); // this number is continuesly increasing all thruout the entries in this table
				DED_PUT_ULONG   	( encoder_ptr, "attribut_chunk_seq", (unsigned long)entity_chunk_seq ); // sequence number of particular item
				DED_PUT_STDVECTOR	( encoder_ptr, "attribut_chunk_data", chunkdata );
			DED_PUT_STRUCT_END( encoder_ptr, "chunk_record" );
			DED_GET_ENCODED_DATA(encoder_ptr,data_ptr,iLengthOfTotalData,pCompressedData,sizeofCompressedData);
			cout << "--- iLengthOfTotalData : " << iLengthOfTotalData << endl;
			cout << "--- sizeofCompressedData : " << sizeofCompressedData << endl;
			if(sizeofCompressedData==0) sizeofCompressedData = iLengthOfTotalData; // if sizeofcompresseddata is 0 then compression was not possible and size is the same as for uncompressed
				iBytesLeft = iTotalSize-iMaxChunkSize*n;
/*
//+DEBUG
//

        DED_PUT_DATA_IN_DECODER(decoder_ptr,pCompressedData,sizeofCompressedData);

struct EntityChunkDataInfo{
    std::string entity_chunk_id;
    unsigned long aiid;
    unsigned long entity_chunk_seq;
    std::vector<unsigned char> entity_chunk_data;
};
	
        EntityChunkDataInfo chunk;
 	// decode data ...
        DED_GET_STRUCT_START( decoder_ptr, "chunk_record" );
            DED_GET_STDSTRING	( decoder_ptr, "attribut_chunk_id", chunk.entity_chunk_id ); // key of particular item
            DED_GET_ULONG   	( decoder_ptr, "attribut_aiid", chunk.aiid ); // this number is continuesly increasing all thruout the entries in this table
            DED_GET_ULONG   	( decoder_ptr, "attribut_chunk_seq", chunk.entity_chunk_seq ); // sequence number of particular item
            DED_GET_STDVECTOR	( decoder_ptr, "attribut_chunk_data", chunk.entity_chunk_data ); //
        DED_GET_STRUCT_END( decoder_ptr, "chunk_record" );
	
	cout << "--- entity_chunk_id : " << chunk.entity_chunk_id << endl;
	cout << "--- entity_aiid : " << chunk.aiid << endl;
	cout << "--- entity_chunk_seq : " << chunk.entity_chunk_seq << endl;
	std::string strtmp(chunk.entity_chunk_data.begin(), chunk.entity_chunk_data.end());
	if(strtmp.size() < 30)
		cout << "--- entity_chunk_data : " << strtmp << endl;
	
//-DEBUG
*/


			listOfDEDchunks.push_back(make_pair(pCompressedData,sizeofCompressedData));
	

//+DEBUG
	struct EntityChunkDataInfo{
			std::string entity_chunk_id;
			unsigned long aiid;
			unsigned long entity_chunk_seq;
			std::vector<unsigned char> entity_chunk_data;
	};

	// now decode and verify
	pair <unsigned char*,int> chunk;
	BOOST_FOREACH( chunk, listOfDEDchunks )
	{
			cout << "decode inside function " << endl;
			cout << "--- chunk.second ; size of chunk " << chunk.second << endl;
			DED_PUT_DATA_IN_DECODER(decoder_ptr,chunk.first,chunk.second);

			EntityChunkDataInfo chunk;
			// decode data ...
			DED_GET_STRUCT_START( decoder_ptr, "chunk_record" );
			DED_GET_STDSTRING	( decoder_ptr, "attribut_chunk_id", chunk.entity_chunk_id ); // key of particular item
			DED_GET_ULONG   	( decoder_ptr, "attribut_aiid", chunk.aiid ); // this number is continuesly increasing all thruout the entries in this table
			DED_GET_ULONG   	( decoder_ptr, "attribut_chunk_seq", chunk.entity_chunk_seq ); // sequence number of particular item
			DED_GET_STDVECTOR	( decoder_ptr, "attribut_chunk_data", chunk.entity_chunk_data ); //
			DED_GET_STRUCT_END( decoder_ptr, "chunk_record" );

			cout << "--- entity_chunk_id : " << chunk.entity_chunk_id << endl;
			cout << "--- entity_aiid : " << chunk.aiid << endl;
			cout << "--- entity_chunk_seq : " << chunk.entity_chunk_seq << endl;
			std::string strtmp(chunk.entity_chunk_data.begin(), chunk.entity_chunk_data.end());
			if(strtmp.size() < 30)
					cout << "--- entity_chunk_data : " << strtmp << endl;
	}


//-DEBUG
	
		}
	}while(iBytesLeft>0 && !bError);

	return listOfDEDchunks;
}

boost::property_tree::ptree CDataDictionaryControl::addDEDchunksToBlockRecords(long &aiid, std::string realmName, std::string ddid, std::vector<pair<unsigned char*,int>>listOfDEDchunks, long maxBlockRecordSize)
{
	using boost::property_tree::ptree;
	std::string strTransGUID="";
	ptree pt;
	pt.clear();
	long bytesLeftInBlockRecord = maxBlockRecordSize;
	pair <unsigned char*,int> chunk;
	long seq=1; // Every BlockRecord have a sequence number 
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

		if(bytesLeftInBlockRecord<=0) {
			bfirst=true;
			bytesLeftInBlockRecord = maxBlockRecordSize;
		}
		bytesLeftInBlockRecord-= chunk.second;
		iBytesLeft = iBytesLeft - chunk.second;
		std::cout << "bytes left : " << iBytesLeft << '\n';
		if(iBytesLeft > 0) {
			boost::property_tree::ptree subpt = createBFiBlockRecord(bfirst, ++aiid, seq, strTransGUID, ddid, realmName, (char*)chunk.first, chunk.second);
			std::cout << "appending chunk of size : " << chunk.second << '\n';
			cout << "- search for last BlockRecord " << endl;
			if(bfirst) {
				pt.insert(pt.get_child("listOfBlockRecords").end(),subpt.front());
				seq++; // new BlockRecord, thus new sequence number
			}
			else {
				if (!appendChunkRecordToLastBlockRecordsChunkData(pt, subpt)) cout << "- FAIL: somehow there was no BlockRecords.chunk_data section to append chunk_record to - possible corrupt data" << endl;
			}
		}
		else
			cout << "- OK! no more chunks to insert " << endl;
		bfirst=false;	
	}

	//DEBUG	
	// write test xml file
	ofstream blockFile ("xmlresult.xml", ios::out | ios::binary);
	write_xml(blockFile, pt);

	return pt;		
}



bool CDataDictionaryControl::addDEDchunksToBlockRecords(boost::property_tree::ptree &pt, long &aiid, std::string realmName, std::string ddid, std::vector<pair<unsigned char*,int>>listOfDEDchunks, long maxBlockRecordSize)
{
	using boost::property_tree::ptree;
	using boost::optional;
	
	bool bResult=false;
	std::string strTransGUID="";
	long bytesLeftInBlockRecord = maxBlockRecordSize;
	pair <unsigned char*,int> chunk;
	long seq=1; // Every BlockRecord have a sequence number 
	int iTotalSize=totalSizeOf(listOfDEDchunks);
	int iBytesLeft=iTotalSize;
	int n=0;
	bool bfirst=true;

		// make sure a list is present or else create
	optional< ptree& > child = pt.get_child_optional( "listOfBlockRecords" );
	if( !child )
	{
		// child node is missing - now create initial basic node
		pt.add("listOfBlockRecords", "");		
	}

	ptree &node = pt.get_child("listOfBlockRecords");
	node.put("chunksInBlockRecords",listOfDEDchunks.size());

	std::cout << "total size of DED chunks: " << iTotalSize << '\n';

	BOOST_FOREACH( chunk, listOfDEDchunks )
	{
		if(aiid==0) 
			strTransGUID = CMD5((const char*)chunk.first).GetMD5s();

		if(bytesLeftInBlockRecord<=0) {
			bfirst=true;
			bytesLeftInBlockRecord = maxBlockRecordSize;
		}
		bytesLeftInBlockRecord-= chunk.second;
		iBytesLeft = iBytesLeft - chunk.second;
		std::cout << "bytes left : " << iBytesLeft << '\n';
		if(iBytesLeft >= 0) {
			boost::property_tree::ptree subpt = createBFiBlockRecord(bfirst, ++aiid, seq, strTransGUID, ddid, realmName, (char*)chunk.first, chunk.second);
			std::cout << "appending chunk of size : " << chunk.second << '\n';
			cout << "- search for last BlockRecord " << endl;
			if(bfirst) {
				pt.insert(pt.get_child("listOfBlockRecords").end(),subpt.front());
				seq++; // new BlockRecord, thus new sequence number
			}
			else {
				if (!appendChunkRecordToLastBlockRecordsChunkData(pt, subpt)) cout << "- FAIL: somehow there was no BlockRecords.chunk_data section to append chunk_record to - possible corrupt data" << endl;
			}
			bResult=true;
		}
		else
			cout << "- OK! no more chunks to insert " << endl;
		bfirst=false;	
	}

	return bResult;		
}




/**
 * Insert a sub ptree containing chunk_data.chunk_records into last BlockRecord.chunk_data
 *
 * parameters:
 *
 * pt     - pointer to ptree which has BlockRecords to append to
 * subpt  - pointer to ptree structure containing a BlockRecord with chunk_data.chunk_records
 */
bool CDataDictionaryControl::appendChunkRecordToLastBlockRecordsChunkData(boost::property_tree::ptree &pt, boost::property_tree::ptree &subpt) 
{
	bool bResult = false;
	try {
		boost::property_tree::ptree _empty_tree;
		BOOST_REVERSE_FOREACH(boost::property_tree::ptree::value_type &v2, pt.get_child("listOfBlockRecords", _empty_tree)) 
		{
			if(v2.first == "BlockRecord")
			{ 
				cout << "- OK: Found Last BlockRecord " << endl;
				BOOST_REVERSE_FOREACH(boost::property_tree::ptree::value_type &v3, v2.second)
				{
					if(v3.first == "chunk_data"){
						cout << "- append new chunk_record inside chunk_data " << endl;
						v3.second.add_child("chunk_record", subpt.get_child("chunk_record", _empty_tree));
						bResult=true;
						break;	
					}
				}
			}	 
		}
	} 
	catch(...)
	{
		cout << "- FAIL: somehow there was no BlockRecords.chunk_data section - possible corrupt data" << endl;
	}
	return bResult; 
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

boost::property_tree::ptree CDataDictionaryControl::addBlockRecordToBlockEntity(std::string transGuid, boost::property_tree::ptree &ptListOfBlockRecords, long maxBlockEntitySize)
{
	using boost::property_tree::ptree;
	ptree _empty_tree;
		
	cout << "ADD - adding blockrecords to blockentity " << endl;
		
	long blockRecordSize = 0;
	long iBytesLeftInBlockEntity=0;
	bool bFirst=true;

	ptree blkrecord;
	ptree &node = blkrecord.add("listOfBlockEntities", "");
	node.add("BlockEntity", "");

	iBytesLeftInBlockEntity=maxBlockEntitySize;
	BOOST_FOREACH(boost::property_tree::ptree::value_type &v2, ptListOfBlockRecords.get_child("listOfBlockRecords", _empty_tree)) 
	{
		if(v2.first == "BlockRecord")
		{
			blockRecordSize = fetchBlockRecordSize(v2);	
			cout << "BlockRecordSize : " << blockRecordSize << endl;	
			iBytesLeftInBlockEntity -= blockRecordSize;
			if(iBytesLeftInBlockEntity <= 0)
			{
				iBytesLeftInBlockEntity=maxBlockEntitySize;
				cout << "-- no more bytes left in this BlockEntity - " << endl;
				if( appendToLastBlockEntity(node, v2.second, transGuid) )
					node.add("BlockEntity",""); // ready for next BlockEntity
				else
					cout << "- FAIL: could not append to last blockentity" << endl;
			}
			else {
				if( !appendToLastBlockEntity(node, v2.second, transGuid) )
					cout << "- FAIL: could not append to last blockentity" << endl;
			}	
		}	 
	}

	return blkrecord;
}

bool CDataDictionaryControl::appendToLastBlockEntity(boost::property_tree::ptree &node, boost::property_tree::ptree &subpt, std::string transGuid)
{
	bool bResult=false;

	BOOST_REVERSE_FOREACH(boost::property_tree::ptree::value_type &v3, node) 
	{
		cout << "-- first : " << v3.first << endl;
		if(v3.first == "BlockEntity")
		{ 
			cout << "- OK: Found Last BlockEntity " << endl;
			v3.second.put("TransGUID", transGuid);
			v3.second.add_child("BlockRecord", subpt);
			bResult=true;
			break;
		}	 
	}

	return bResult;
}


long CDataDictionaryControl::fetchBlockRecordSize(boost::property_tree::ptree::value_type &vt)
{
	using boost::property_tree::ptree;
	ptree _empty_tree;
	long lResult = 0;
	BOOST_FOREACH(ptree::value_type &v2, vt.second)
	{
		lResult += v2.second.data().size();

		// iterate thru elements in current BlockRecord and calculate size of BlockRecord
		BOOST_FOREACH(ptree::value_type &v3, v2.second)
		{
			if(v3.first == "chunk_record") {
				cout << "-- chunk_record size : " << v3.second.get_child("DataSize", _empty_tree).data() << endl;
				lResult += v3.second.get_child("Data", _empty_tree).data().size();
			}
			lResult += v3.second.data().size();
		}
		
	}
	return	lResult;
}

std::vector< pair<std::string ,int> > CDataDictionaryControl::writeBlockEntityToBFiFile(boost::property_tree::ptree &ptBlockEntities)
{
	using boost::property_tree::ptree;
	ptree _empty_tree;
	std::vector< pair<std::string ,int> > listOfBlockEntityFiles;

	cout << "- writeBlockEntityToBFiFile " << endl;
	long filenumber=0;
	ptree blkEntity;
	BOOST_FOREACH(ptree::value_type &v2, ptBlockEntities.get_child("listOfBlockEntities", _empty_tree))
	{
		filenumber++;
		ptree &node = blkEntity.add("BFi", "");
		node.add_child("BlockEntity", v2.second);
		
		std::string filename =  v2.second.get_child("TransGUID").data();
		cout << "- BlockEntity, transGUID : " << filename << endl;

		std::string blockfilename = filename + "_" + std::to_string(filenumber) + ".BFi";
		ofstream blockFile (blockfilename.c_str(), ios::out | ios::binary);
		write_xml(blockFile, blkEntity);
		
		int filesize = (int)boost::filesystem::file_size(blockfilename);
		listOfBlockEntityFiles.push_back(make_pair(blockfilename,filesize));

		blkEntity.clear();
	}


	return listOfBlockEntityFiles;
}

bool CDataDictionaryControl::addAttributToBlockRecord(boost::property_tree::ptree &ptListOfBlockRecords, long maxBlockRecordSize, std::string realmName, std::string attributName, std::vector<unsigned char> attributValue)
{
	using boost::property_tree::ptree;
	using boost::optional;
	bool bResult=false;
	
	// make sure a list is present or else create
	optional< ptree& > child = ptListOfBlockRecords.get_child_optional( "listOfBlockRecords" );
	if( !child )
	{
		// child node is missing - now create initial basic node
		ptListOfBlockRecords.add("listOfBlockRecords", "");		
	}

//	ptree &node = ptListOfBlockRecords;

	long maxDEDchunkSize=300;
	static long aiid=0;
	std::vector< pair<unsigned char*,int> > listOfDEDchunks = splitAttributIntoDEDchunks(aiid, attributName, attributValue, maxDEDchunkSize);

	aiid=0;
	bool bAdded = addDEDchunksToBlockRecords(ptListOfBlockRecords, aiid, realmName, attributName, listOfDEDchunks, maxBlockRecordSize);

//TODO: add attribut ...
	


	bResult=bAdded;

	return bResult;
}


