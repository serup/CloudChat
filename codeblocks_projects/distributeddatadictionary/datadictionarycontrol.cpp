#include "datadictionarycontrol.hpp"


CDataDictionaryControl::CDataDictionaryControl()
{
    //ctor
	setMaxDEDchunkSize(300); // Default DED chunk size 
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
		//std::cout << "[readFile] Reading file: " << fn << " ; amount " << length << " characters... \n";
        FileDataBytesInVector.resize(length,0); // Make sure receipient has room
		
		//read content of infile
		is.read ((char*)&FileDataBytesInVector[0],length);

		//std::cout << "[readFile] size: " << (int) FileDataBytesInVector.size() << '\n';
		//std::cout << "[readFile] capacity: " << (int) FileDataBytesInVector.capacity() << '\n';
		//std::cout << "[readFile] max_size: " << (int) FileDataBytesInVector.max_size() << '\n';

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
			//cout << "ADD first BlockRecord entry" << endl;
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
//			cout << "ADD node chunk_record" << endl;
			ptree &node = pt.add("chunk_record", "");
			node.put("chunk_ddid", ddid); 
			node.put("DataSize", nSizeOfHex); //TODO: consider dropping transfer to HEX, since it is making size bigger and only needed for Debug
			node.put("Data", data_in_hex_buf);
			node.put("DataMD5", strMD5);
	}
	free(data_in_hex_buf);
	return pt;
}


std::vector< pair<std::vector<unsigned char>, int> > CDataDictionaryControl::splitAttributIntoDEDchunks(long &aiid, std::string attributName, std::vector<unsigned char>& attributValue, long maxDEDchunkSize)
{
    using boost::property_tree::ptree;
	ptree pt;

	int iMaxChunkSize=maxDEDchunkSize; 
	int iTotalSize=attributValue.size();
	int iBytesLeft=iTotalSize;
	int n=0;
	std::vector<unsigned char> chunkdata;
	unsigned long entity_chunk_seq= (unsigned long)0;
	bool bInternalFlush=false;
	std::vector< pair<std::vector<unsigned char>,int> > listOfDEDchunks;
	int strangeCount=0;
	bool bError=false;

	cout << "*{{{" << endl;
	cout << " DED entity_chunk_seq : ";

	do
	{
		chunkdata.clear();  

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
				BOOST_LOG_TRIVIAL(error) << "[splitAttributIntoDEDchunks] ERROR: NO data inserted in chunkdata, default <empty> added to field : " << attributName  << "\n";
				std::string strtmp="<empty>";
				std::copy(strtmp.begin(), strtmp.end(), std::back_inserter(chunkdata));
				strangeCount++;
				if(strangeCount>10) {
					bError=true; // avoid deadlock, due to errornous attribut
					BOOST_LOG_TRIVIAL(fatal) << "[splitAttributIntoDEDchunks] ERROR: Aborting function !!!!" << "\n";
				}
		}

		n++;
		aiid++;
		entity_chunk_seq++;
	
		cout << "," << entity_chunk_seq;
    

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

			std::vector<unsigned char> vdata;
			vdata.assign(pCompressedData, pCompressedData + sizeofCompressedData);  

			//+test
			cout << endl << "/*{{{*/" << endl;
			cout << "internal test of data - before DED : " << endl;
			for(int n=0;n<chunkdata.size(); n++)
			{ 
				fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "\r\n" : " " );
			}

			EntityChunkDataInfo _chunk;
			// decode data ...
			DED_PUT_DATA_IN_DECODER(decoder_ptr,pCompressedData,sizeofCompressedData);

			DED_GET_STRUCT_START( decoder_ptr, "chunk_record" );
			DED_GET_STDSTRING	( decoder_ptr, "attribut_chunk_id", _chunk.entity_chunk_id ); // key of particular item
			DED_GET_ULONG   	( decoder_ptr, "attribut_aiid", _chunk.aiid ); // this number is continuesly increasing all thruout the entries in this table
			DED_GET_ULONG   	( decoder_ptr, "attribut_chunk_seq", _chunk.entity_chunk_seq ); // sequence number of particular item
			DED_GET_STDVECTOR	( decoder_ptr, "attribut_chunk_data", _chunk.entity_chunk_data ); //
			DED_GET_STRUCT_END( decoder_ptr, "chunk_record" );

			cout << endl << "internal test of data - after DED : " << endl;
			cout << endl << "/*{{{*/" << endl;
			for(int n=0;n<_chunk.entity_chunk_data.size(); n++)
			{
				if( chunkdata[n] != _chunk.entity_chunk_data[n] )
					cout << "FAIL:";	
				fprintf(stdout, "%02X%s", _chunk.entity_chunk_data[n], ( n + 1 ) % 16 == 0 ? "\r\n" : " " );
			}
			cout << "/*}}}*/" << endl;

			cout << "/*}}}*/" << endl;


			//-test
			listOfDEDchunks.push_back(make_pair(vdata,sizeofCompressedData));
	

		}
	}while(iBytesLeft>0 && !bError);

	cout << "*}}}" << endl;


	return listOfDEDchunks;
}


boost::property_tree::ptree CDataDictionaryControl::addDEDchunksToBlockRecords(long &aiid, std::string realmName, std::string ddid, std::vector<pair<std::vector<unsigned char>,int>>listOfDEDchunks, long maxBlockRecordSize)
{
	using boost::property_tree::ptree;
	std::string strTransGUID="";
	ptree pt;
	pt.clear();
	long bytesLeftInBlockRecord = maxBlockRecordSize;
	pair <std::vector<unsigned char>,int> chunk;
	long seq=1; // Every BlockRecord have a sequence number 
	int iTotalSize=totalSizeOf(listOfDEDchunks);
	int iBytesLeft=iTotalSize;
	int n=0;
	bool bfirst=true;

	ptree &node = pt.add("listOfBlockRecords", "");
	node.put("chunksInBlockRecords",listOfDEDchunks.size());

	int c=0;
	cout << "handling DED chunk : ";
	BOOST_FOREACH( auto &chunk, listOfDEDchunks )
	{
		cout << ++c << ",";
		if(aiid==0) 
			strTransGUID = CMD5((const char*)chunk.first.data()).GetMD5s();

		if(bytesLeftInBlockRecord<=0) {
			bfirst=true;
			bytesLeftInBlockRecord = maxBlockRecordSize;
		}
		bytesLeftInBlockRecord-= chunk.second;
		iBytesLeft = iBytesLeft - chunk.second;
		if(iBytesLeft >= 0) {
			aiid++;
			boost::property_tree::ptree subpt = createBFiBlockRecord(bfirst, aiid, seq, strTransGUID, ddid, realmName, (char*)chunk.first.data(), chunk.second);
			if(bfirst) {
				pt.insert(pt.get_child("listOfBlockRecords").end(),subpt.front());
				seq++; // new BlockRecord, thus new sequence number
			}
			else {
				if (!appendChunkRecordToLastBlockRecordsChunkData(pt, subpt)) cout << "- FAIL: somehow there was no BlockRecords.chunk_data section to append chunk_record to - possible corrupt data" << endl;
			}
		}
		bfirst=false;	
	}
	cout << endl;

	//DEBUG	
	// write test xml file
	ofstream blockFile ("xmlresult.xml", ios::out | ios::binary);
	write_xml(blockFile, pt);

	return pt;		
}



bool CDataDictionaryControl::addDEDchunksToBlockRecords(std::string transGuid, boost::property_tree::ptree &pt, long &aiid, std::string realmName, std::string ddid, std::vector<pair<std::vector<unsigned char>,int>>listOfDEDchunks, long &maxBlockRecordSize)
{
	using boost::property_tree::ptree;
	using boost::optional;
	
	bool bResult=false;
	std::string strTransGUID="";
	long bytesLeftInBlockRecord = maxBlockRecordSize;
	pair <std::vector<unsigned char>,int> chunk;
	long seq=1; // Every BlockRecord have a sequence number 
	int iTotalSize=totalSizeOf(listOfDEDchunks);
	int iBytesLeft=iTotalSize;
	int n=0;
	bool bfirst=false;

	// make sure a list is present or else create
	optional< ptree& > child = pt.get_child_optional( "listOfBlockRecords" );
	if( !child )
	{
		// child node is missing - now create initial basic node
		pt.add("listOfBlockRecords", "");		
	}
	
	child = pt.get_child_optional( "BlockRecord.chunk_data" );
	if( !child )
	{
		// child node is missing - this must be first time 
		bfirst=true;
	}

	ptree &node = pt.get_child("listOfBlockRecords");

	int c=0;
	cout << "handling DED chunk : ";
	BOOST_FOREACH( auto &chunk, listOfDEDchunks )
	{
		cout << ++c << ",";
		strTransGUID = transGuid;

		if(bytesLeftInBlockRecord<=0) {
			bfirst=true;
			bytesLeftInBlockRecord = maxBlockRecordSize;
		}
		bytesLeftInBlockRecord-= chunk.second;
		iBytesLeft = iBytesLeft - chunk.second;
		if(iBytesLeft >= 0) {
			aiid++;
			boost::property_tree::ptree subpt = createBFiBlockRecord(bfirst, aiid, seq, strTransGUID, ddid, realmName, (char*)chunk.first.data(), chunk.second);
			if(bfirst) {
				pt.insert(pt.get_child("listOfBlockRecords").end(),subpt.front());
				seq++; // new BlockRecord, thus new sequence number
			}
			else {
				if (!appendChunkRecordToLastBlockRecordsChunkData(pt, subpt)) cout << "- FAIL: somehow there was no BlockRecords.chunk_data section to append chunk_record to - possible corrupt data" << endl;
			}
			bResult=true;
		}
		bfirst=false;	
	}

	cout << endl;
	maxBlockRecordSize = bytesLeftInBlockRecord;
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
	using boost::property_tree::ptree;
	using boost::optional;
	bool bResult = false;
	try {
		boost::property_tree::ptree _empty_tree;
		BOOST_REVERSE_FOREACH(boost::property_tree::ptree::value_type &v2, pt.get_child("listOfBlockRecords", _empty_tree)) 
		{

			if(v2.first == "BlockRecord")
			{ 
//				cout << "- OK: Found Last BlockRecord " << endl;
				BOOST_REVERSE_FOREACH(boost::property_tree::ptree::value_type &v3, v2.second)
				{
					if(v3.first == "chunk_data"){
//						cout << "- append new chunk_record inside chunk_data " << endl;
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

long CDataDictionaryControl::totalSizeOf(std::vector<pair<std::vector<unsigned char>, int>> vectorPairList)
{
	long totalCount=0;		
	pair <std::vector<unsigned char>,int> chunk;
	BOOST_FOREACH( auto &chunk, vectorPairList )
	{
		totalCount+=chunk.second;
	}	
	return totalCount;
}

boost::property_tree::ptree CDataDictionaryControl::addBlockRecordToBlockEntity(std::string transGuid, boost::property_tree::ptree &ptListOfBlockRecords, long maxBlockEntitySize)
{
	using boost::property_tree::ptree;
	ptree _empty_tree;
		
	long iBytesLeftInBlockEntity=maxBlockEntitySize;
	long amountOfBlockRecords=ptListOfBlockRecords.count("BlockRecord"); 

	if(amountOfBlockRecords<=0)
		BOOST_LOG_TRIVIAL(error) << "[addBlockRecordToBlockEntity] - FAIL: ERROR: There was no BlockRecords, hence BlockEntity will be empty " << endl;

	ptree blkrecord;
	ptree &node = blkrecord.add("listOfBlockEntities", "");
	node.add("BlockEntity", "");

	BOOST_FOREACH(ptree::value_type &v2, ptListOfBlockRecords.get_child("listOfBlockRecords", _empty_tree)) 
	{
		if(v2.first == "BlockRecord")
		{
			amountOfBlockRecords--;
			iBytesLeftInBlockEntity -= fetchBlockRecordSize(v2);
			if(iBytesLeftInBlockEntity <= 0)
			{
				if(iBytesLeftInBlockEntity < 0)
					BOOST_LOG_TRIVIAL(warning) << "- WARNING: current BlockRecord size exceeds BlockEntity size, hence this BlockRecord will span over multiple .BFi files; this is Normal " << endl;
				
				if( !appendToLastBlockEntity(node, v2.second, transGuid) ) 
					BOOST_LOG_TRIVIAL(error) << "[addBlockRecordToBlockEntity] - FAIL: could not append to last blockentity" << endl;
				
				if(amountOfBlockRecords>0) 
					node.add("BlockEntity",""); // ready for next BlockEntity
				
				iBytesLeftInBlockEntity=maxBlockEntitySize;
			}
			else {
				if( !appendToLastBlockEntity(node, v2.second, transGuid) )
					BOOST_LOG_TRIVIAL(error) << "[addBlockRecordToBlockEntity] - FAIL: could not append to last blockentity" << endl;
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
		if(v3.first == "BlockEntity")
		{ 
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

	long filenumber=0;
	ptree blkEntity;
	BOOST_FOREACH(ptree::value_type &v2, ptBlockEntities.get_child("listOfBlockEntities", _empty_tree))
	{
		filenumber++;
		ptree &node = blkEntity.add("BFi", "");
		node.add_child("BlockEntity", v2.second);
		
		std::string filename =  v2.second.get_child("TransGUID").data();

		std::string blockfilename = filename + "_" + std::to_string(filenumber) + ".BFi";
		{
		ofstream blockFile (blockfilename.c_str(), ios::out | ios::binary);
		write_xml(blockFile, blkEntity);
		}

		int filesize = (int)boost::filesystem::file_size(blockfilename);
		if(filesize<=0) {
			BOOST_LOG_TRIVIAL(warning) << "WARNING: file created, was of size 0 according to boost::filesystem::file_size( " << blockfilename << " ) - will try to get size again... " << "\n"; 
			// try again - perhaps flush was not finished
			filesize = (int)boost::filesystem::file_size(blockfilename);
			if(filesize<=0)
				BOOST_LOG_TRIVIAL(error) << "ERROR: file created, was of size 0 according to boost::filesystem::file_size( " << blockfilename << " ) " << "\n"; 
		}
		if(filesize > 0) 
			listOfBlockEntityFiles.push_back(make_pair(blockfilename,filesize));

		blkEntity.clear();
	}


	return listOfBlockEntityFiles;
}


bool CDataDictionaryControl::addAttributToBlockRecord(std::string transGuid, boost::property_tree::ptree &ptListOfBlockRecords, long &maxBlockRecordSize, std::string realmName, std::string attributName, std::vector<unsigned char> attributValue)
{
	using boost::property_tree::ptree;
	using boost::optional;
	bool bResult=false;
	static long aiid=0;


	long maxDEDchunkSize=getMaxDEDchunkSize();
	std::vector< pair<std::vector<unsigned char>,int> > listOfDEDchunks = splitAttributIntoDEDchunks(aiid, attributName, attributValue, maxDEDchunkSize);
	bResult = addDEDchunksToBlockRecords(transGuid, ptListOfBlockRecords, aiid, realmName, attributName, listOfDEDchunks, maxBlockRecordSize);

	
	return bResult;
}

void CDataDictionaryControl::setMaxDEDchunkSize(long maxSize)
{
	_maxDEDchunkSize = maxSize;
}

long CDataDictionaryControl::getMaxDEDchunkSize()
{
	return _maxDEDchunkSize;
}


std::list<std::string> CDataDictionaryControl::ls()
{
	using boost::optional;
	using boost::property_tree::ptree;
	std::list<std::string> listBFiAttributes;

	try
	{
	std::string transGuid="";
	std::string id="";
	std::string str;
	std::string attribut = "";
	std::string prev = "";
	std::string prevAtt = "";
	boost::property_tree::ptree _empty_tree;

	boost::filesystem::path targetDir( boost::filesystem::current_path() );
	boost::filesystem::recursive_directory_iterator iter(targetDir), eod;

	BOOST_FOREACH(boost::filesystem::path const& i, make_pair(iter, eod))
	{
		if (is_regular_file(i)){

			bool bExtBFi=false;
			bExtBFi = (boost::filesystem::extension(i.string()) == ".BFi");
			if(bExtBFi) {
				std::ifstream is (i.string());
				ptree pt;
				try{
						read_xml(is, pt);
				}catch(...) {}

				optional< ptree& > child = pt.get_child_optional("BFi");
				if(child) 
				{
					BOOST_FOREACH(const boost::property_tree::ptree::value_type & child, pt.get_child("BFi.BlockEntity.BlockRecord", _empty_tree)) 
					{
						str = child.first.data();
						if (str == "chunk_id") id = child.second.data();
						if (str == "TransGUID") transGuid = child.second.data();

						attribut = transGuid + "./" + id + "/";
						prevAtt="";
						BOOST_FOREACH(const boost::property_tree::ptree::value_type &vt2 , child.second)
						{
								if(vt2.first == "chunk_record")
								{
										prev=attribut;
										BOOST_FOREACH(const boost::property_tree::ptree::value_type &vt3, vt2.second)
										{
												if(vt3.first == "chunk_ddid") {
													if(prevAtt!=vt3.second.data()) {
														prevAtt=vt3.second.data();
														attribut += vt3.second.data();
														// find if any existing attribut
														auto i = listBFiAttributes.begin(), end = listBFiAttributes.end();
														i = std::find(i, end, attribut);
														if(i != end)
															cout << "INFO: span over .BFi : " << attribut << endl;
														else	
															listBFiAttributes.push_back(attribut); // disregard chunks of attribut and duplicates, only list unique attributs
														attribut=prev;
													}
												}
										}
								}
						}
					}
				}
			}
		}
	}
	}
	catch (const std::exception& e)  // catch any exceptions
	{ cerr << endl << "Exception: " << e.what() << endl; }


	return listBFiAttributes;
}

std::vector<assembledElements> CDataDictionaryControl::readBlockRecordElements(boost::property_tree::ptree::value_type &vt2)
{
	std::vector<assembledElements> records_elements; // contains assembled elements from tree
	bool pushed=false;
	std::string prevchunkid = (std::string)"nothing";
	assembledElements Element;

	cout << "searching for chunk_record ";
	bool bFound=false;
	BOOST_FOREACH(boost::property_tree::ptree::value_type &vt3, vt2.second)
	{
		if(vt3.first == "chunk_record")
		{
			if(!bFound) { cout << "+" << endl; bFound=true; }

			chunk_record_entries f;
			f.chunk_ddid = vt3.second.get<std::string>("chunk_ddid");
			f.DataSize  = vt3.second.get<unsigned long>("DataSize");
			f.Data      = vt3.second.get<std::string>("Data");
			f.DataMD5   = vt3.second.get<std::string>("DataMD5");

			//cout << "DataMD5 : "<< f.DataMD5 <<endl;
			std::string strMD5(CMD5(f.Data.c_str()).GetMD5s());
			if(f.DataMD5 != strMD5)
				cout << "FAIL: ERROR: data area for attribut have been corrupted " << endl;

			// convert Data 
			std::string hexdata = f.Data;
			unsigned char* data_in_unhexed_buf = (unsigned char*) malloc (hexdata.size());
			ZeroMemory(data_in_unhexed_buf,hexdata.size()); // make sure no garbage is inside the newly allocated space
			boost::algorithm::unhex(hexdata.begin(),hexdata.end(), data_in_unhexed_buf);// convert the hex array to an array containing byte values

			//cout << "hexdata : < " << hexdata << " > " << endl; 
			//cout << "hexdata size: " << hexdata.size() << endl;
			// initialize decoder with Data 
			DED_PUT_DATA_IN_DECODER(decoder_ptr,data_in_unhexed_buf,hexdata.size());
			if(decoder_ptr != 0) {
				EntityChunkDataInfo _chunk;
				// decode data ...
				DED_GET_STRUCT_START( decoder_ptr, "chunk_record" );
				DED_GET_STDSTRING	( decoder_ptr, "attribut_chunk_id", _chunk.entity_chunk_id ); // key of particular item
				DED_GET_ULONG   	( decoder_ptr, "attribut_aiid", _chunk.aiid ); // this number is continuesly increasing all thruout the entries in this table
				DED_GET_ULONG   	( decoder_ptr, "attribut_chunk_seq", _chunk.entity_chunk_seq ); // sequence number of particular item
				DED_GET_STDVECTOR	( decoder_ptr, "attribut_chunk_data", _chunk.entity_chunk_data ); //
				DED_GET_STRUCT_END( decoder_ptr, "chunk_record" );
			
				//cout << "entity_chunk_id : " << _chunk.entity_chunk_id << endl;
				//cout << "entity_aiid : " << _chunk.aiid << endl;
			
				if(prevchunkid=="nothing" || prevchunkid != _chunk.entity_chunk_id)
				{
					if(prevchunkid!="nothing" && prevchunkid != _chunk.entity_chunk_id){
						records_elements.push_back(Element);
						pushed=true;
						/// new Element
						Element.strElementID = _chunk.entity_chunk_id;
						Element.ElementData.clear();
						prevchunkid = _chunk.entity_chunk_id;
						cout << endl; 
					}
					else
					{
						pushed=false;
						/// new Element
						Element.strElementID = _chunk.entity_chunk_id;
						Element.ElementData.clear();
						prevchunkid = _chunk.entity_chunk_id;
					}
					cout << "- attribut : " << _chunk.entity_chunk_id << endl << "-- entity_chunk_seq : ";
				}
	
				cout << "," << _chunk.entity_chunk_seq;

				/// this will, chunk by chunk, assemble the data
				std::copy(_chunk.entity_chunk_data.begin(), _chunk.entity_chunk_data.end(), std::back_inserter(Element.ElementData));

				pushed=false;
			}
			free(data_in_unhexed_buf);
		}
		else
			cout << "-";
	}
	if(pushed==false){
		records_elements.push_back(Element);
	}
	cout << endl;

	return records_elements;
}

bool CDataDictionaryControl::findElement(std::vector<assembledElements>& _Elements, std::string strElementID)
{
	bool bResult=false;
	/// finding the element
	struct ElementIs {
		ElementIs( string s ) : toFind(s) { }
		bool operator() (const assembledElements &n)
		{
			return n.strElementID == toFind;
		}
		string toFind;
	};
	
	std::vector<assembledElements>::iterator it = std::find_if(_Elements.begin(), _Elements.end(), ElementIs(strElementID));
	if(it != _Elements.end())
	{
		// element was found 
		bResult=true;
	}
	return bResult;
}

std::vector<unsigned char> CDataDictionaryControl::fetchElement(std::vector<assembledElements>& _Elements, std::string strElementID)
{
	std::vector<unsigned char> resultValue;

	/// finding the element
	struct ElementIs {
		ElementIs( string s ) : toFind(s) { }
		bool operator() (const assembledElements &n)
		{
			return n.strElementID == toFind;
		}
		string toFind;
	};
	
	std::vector<assembledElements>::iterator it = std::find_if(_Elements.begin(), _Elements.end(), ElementIs(strElementID));
	if(it != _Elements.end())
	{
		/// now fetch value
		assembledElements e;
		e = (*it);
		std::vector<unsigned char> rValue(e.ElementData.begin(),e.ElementData.end());
		resultValue = rValue;
	}
	return resultValue;
}

/*
pair<std::string, std::vector<unsigned char>> CDataDictionaryControl::_ftgt(std::string attributpath)
{
	using boost::optional;
	using boost::property_tree::ptree;
	
	std::list<EntityChunkDataInfo> listOfattributChunks;

	pair<std::string, std::vector<unsigned char>> resultAttributPair;
	
	std::vector<unsigned char> ElementData;

	try
	{
	std::string transGuid="";
	std::string id="";
	std::string str;
	std::string attribut = "";
	std::string prev = "";
	std::string prevAtt = "";
	boost::property_tree::ptree _empty_tree;

	boost::filesystem::path targetDir( boost::filesystem::current_path() );
	boost::filesystem::recursive_directory_iterator iter(targetDir), eod;

	BOOST_FOREACH(boost::filesystem::path const& i, make_pair(iter, eod))
	{
		if (is_regular_file(i)){

			bool bExtBFi=false;
			bExtBFi = (boost::filesystem::extension(i.string()) == ".BFi");
			if(bExtBFi) {
				cout << "file : " << i << endl;
				cout << "*{{{" << endl;

				std::ifstream is (i.string());
				ptree pt;
				try{
						read_xml(is, pt);
				}catch(...) {}

				optional< ptree& > child = pt.get_child_optional("BFi");
				if(child) 
				{
					BOOST_FOREACH(const boost::property_tree::ptree::value_type & child, pt.get_child("BFi.BlockEntity.BlockRecord", _empty_tree)) 
					{
						str = child.first.data();
						if (str == "chunk_id") id = child.second.data();
						if (str == "TransGUID") transGuid = child.second.data();

						attribut = transGuid + "./" + id + "/";
						prevAtt="";
						BOOST_FOREACH(const boost::property_tree::ptree::value_type &vt2 , child.second)
						{
								if(vt2.first == "chunk_record")
								{
										prev=attribut;
										BOOST_FOREACH(const boost::property_tree::ptree::value_type &vt3, vt2.second)
										{
												if(vt3.first == "chunk_ddid") {
														attribut += vt3.second.data();

													  //cout << "attribut : " << attribut << " == attributpath : " << attributpath << endl;

														// find if any existing attribut
														if(attribut == attributpath){
															chunk_record_entries f;
															f.DataSize = vt2.second.get<unsigned long>("DataSize");
															f.Data = vt2.second.get<std::string>("Data");
															f.DataMD5 = vt2.second.get<std::string>("DataMD5");

															//cout << "DataMD5 : "<< f.DataMD5 <<endl;
															std::string strMD5(CMD5(f.Data.c_str()).GetMD5s());
															if(f.DataMD5 != strMD5)
																	cout << "FAIL: ERROR: data area for attribut have been corrupted " << endl;


															// convert Data 
															std::string hexdata = f.Data;
															unsigned char* data_in_unhexed_buf = (unsigned char*) malloc (hexdata.size());
															ZeroMemory(data_in_unhexed_buf,hexdata.size()); // make sure no garbage is inside the newly allocated space
															boost::algorithm::unhex(hexdata.begin(),hexdata.end(), data_in_unhexed_buf);// convert the hex array to an array containing byte values

															//cout << "hexdata : < " << hexdata << " > " << endl; 
															//cout << "hexdata size: " << hexdata.size() << endl;
															// initialize decoder with Data 
															DED_PUT_DATA_IN_DECODER(decoder_ptr,data_in_unhexed_buf,hexdata.size());
															if(decoder_ptr != 0) {

																	EntityChunkDataInfo _chunk;
																	// decode data ...
																	DED_GET_STRUCT_START( decoder_ptr, "chunk_record" );
																	DED_GET_STDSTRING	( decoder_ptr, "attribut_chunk_id", _chunk.entity_chunk_id ); // key of particular item
																	DED_GET_ULONG   	( decoder_ptr, "attribut_aiid", _chunk.aiid ); // this number is continuesly increasing all thruout the entries in this table
																	DED_GET_ULONG   	( decoder_ptr, "attribut_chunk_seq", _chunk.entity_chunk_seq ); // sequence number of particular item
																	DED_GET_STDVECTOR	( decoder_ptr, "attribut_chunk_data", _chunk.entity_chunk_data ); //
																	DED_GET_STRUCT_END( decoder_ptr, "chunk_record" );

																	//cout << "entity_chunk_id : " << _chunk.entity_chunk_id << endl;
																	//cout << "entity_aiid : " << _chunk.aiid << endl;
																	cout << "entity_chunk_seq : " << _chunk.entity_chunk_seq << endl;
													
																	// add to list of received chunks
																	listOfattributChunks.push_back(_chunk); 

																	//cout << "Elements in listOfattributChunks : " << listOfattributChunks.size() << endl;

																	//TODO: assemble after sort	of listOfattributChunks
																    /// this will, chunk by chunk, assemble the element data
																    std::copy(_chunk.entity_chunk_data.begin(), _chunk.entity_chunk_data.end(), std::back_inserter(ElementData));	
															}
															else
																	BOOST_LOG_TRIVIAL(error) << "[CDataDictionaryControl::ftgt] FAIL: ERROR:  when trying to put data into decoder " << endl;
															
														
														}
														attribut=prev;

												
												}
										}
								}
						}
					}
				}

				cout << "*}}}" << endl;
			}
			
		}
	}
	}
	catch (const std::exception& e)  // catch any exceptions
	{ cerr << endl << "Exception: " << e.what() << endl; }

	resultAttributPair = make_pair(attributpath,ElementData); // assemble data 

	return resultAttributPair;
}
*/

pair<std::string, std::vector<unsigned char>> CDataDictionaryControl::ftgt(std::string attributpath)
{
	using boost::optional;
	using boost::property_tree::ptree;
	
	std::vector<assembledElements> records_elements; // contains assembled elements from tree
	std::vector<unsigned char> ElementData;
	pair<std::string, std::vector<unsigned char>> resultAttributPair;


	try
	{
	std::string transGuid="";
	std::string id="";
	std::string str;
	std::string attribut = "";
	std::string prev = "";
	std::string prevAtt = "";
	boost::property_tree::ptree _empty_tree;

	boost::filesystem::path targetDir( boost::filesystem::current_path() );
	boost::filesystem::recursive_directory_iterator iter(targetDir), eod;

	BOOST_FOREACH(boost::filesystem::path const& i, make_pair(iter, eod))
	{
		if (is_regular_file(i)){

			bool bExtBFi=false;
			bExtBFi = (boost::filesystem::extension(i.string()) == ".BFi");
			if(bExtBFi) {
				cout << "file : " << i << endl;
				cout << "*{{{" << endl;

				std::ifstream is (i.string());
				ptree pt;
				try{
						read_xml(is, pt);
				}catch(...) {}

				optional< ptree& > child = pt.get_child_optional("BFi");
				if(child) 
				{
					cout << "searching for chunk_data ";
					BOOST_FOREACH(boost::property_tree::ptree::value_type &child, pt.get_child("BFi.BlockEntity.BlockRecord", _empty_tree)) 
					{
							if (child.first == "TransGUID") transGuid = child.second.data();
							if (child.first == "chunk_id") id = child.second.data();

							//cout << "attribut name : " << child.first << endl;
							cout << "-";
							if(child.first == "chunk_data")
							{
								cout << "+" << endl;
								records_elements = readBlockRecordElements(child);
								cout << "*{{{" << endl;
								BOOST_FOREACH(assembledElements &_element, records_elements)
								{
									cout << "element id : " << _element.strElementID << endl;
									// add padding for element
									_element.strElementID = transGuid + "./" + id + "/" + _element.strElementID;
									cout << "-element id : " << _element.strElementID << endl;
									
									cout << "element size of value : " << _element.ElementData.size() << endl;
									if(_element.ElementData.size() < 50) {
										std::string str(_element.ElementData.begin(), _element.ElementData.end());
										cout << "element value : " << str << endl;
									}
								}
								cout << "*}}}" << endl;
							}
					}
				}

				cout << "*}}}" << endl;
			}
			
		}
	}
	}
	catch (const std::exception& e)  // catch any exceptions
	{ cerr << endl << "Exception: " << e.what() << endl; }

	//find element in list
	ElementData = fetchElement(records_elements, attributpath);

	//return as a pair <name,value>
	resultAttributPair = make_pair(attributpath,ElementData); // assemble data 

	return resultAttributPair;
}

