
#define BOOST_TEST_MODULE datadictionarycontroltest
#include <boost/test/included/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <iostream>
#include <fstream>      // std::ifstream
#include "../datadictionarycontrol.hpp"
#include "../md5.h"
#include "../utils.hpp"

using namespace std;
using namespace boost::unit_test;

//////////////////////////////////////////
// TESTCASE
//////////////////////////////////////////
// howto compile :
// use makefile.mak from linux prompt
// command line in linux:
// g++ -g -o bin/Debug/DDDAdmin_test DDDAdmin_test.cpp ../datadictionarycontrol.hpp ../datadictionarycontrol.cpp -L"/usr/local/lib/" -D__DEBUG__ -D__MSABI_LONG=long -lboost_system -lboost_signals -lboost_thread -lboost_filesystem -lpthread -lrt -std=gnu++11
//
// ex.
// make -f makefile.mak total
// NB! makefile will not only build and link, it will also run the testcase
//     and convert the generated result output into the file test_result.html
//////////////////////////////////////////
// how to test:
//  make -f makefile_cygwin.mak test
//  This will convert test_results.xml file to test_results.html, based on test_results.xslt file
//
//  if you are running vim then do following
//  :vsp output
//  :0,$d|:read !make -f makefile_ubuntu test
//
//  first line will create a split window - use only one time
//  second line will delete output buffer and run makefile for tests - output will end up in the
//  split window
//    the second line can be run after every build
//    NB! make sure you have focus on output window before running command !!!  press ctrl-W and then press left or right arrow to change split window
//////////////////////////////////////////

#define BOOST_AUTO_TEST_MAIN
#ifndef NOTESTRESULTFILE
#ifdef BOOST_AUTO_TEST_MAIN
std::ofstream out;

struct ReportRedirector
{
    ReportRedirector()
    {
        out.open("test_results.xml");
        assert( out.is_open() );
        boost::unit_test::results_reporter::set_stream(out);
    }
};


struct EntityChunkDataInfo{
	std::string entity_chunk_id;
	unsigned long aiid;
	unsigned long entity_chunk_seq;
	std::vector<unsigned char> entity_chunk_data;
};


BOOST_GLOBAL_FIXTURE(ReportRedirector)
#endif
#endif

boost::shared_ptr< std::vector<pair<std::vector<unsigned char>,int>> > getV(){
	boost::shared_ptr< std::vector<pair<std::vector<unsigned char>,int>> > v( new std::vector<pair<std::vector<unsigned char>,int>>() );
	std::string name1 = "Johnny Serup";	
	std::vector<unsigned char> attributValue1(name1.begin(), name1.end());
	std::string name2 = "Lee Xiao Long";	
	std::vector<unsigned char> attributValue2(name2.begin(), name2.end());
	v->push_back(make_pair(attributValue1,attributValue1.size()));
	v->push_back(make_pair(attributValue2,attributValue2.size()));
	return v;
}

boost::shared_ptr< std::vector< pair<std::unique_ptr<unsigned char>,int>> > getVblob()
{
//Note the 'new' on the next line, as it is what actually creates the vector
	boost::shared_ptr< std::vector< pair<std::unique_ptr<unsigned char>,int>> > v( new std::vector< pair<std::unique_ptr<unsigned char>,int> >() );


    std::unique_ptr<unsigned char> testData;
	int sizeofdata=30;
	testData.reset(new unsigned char[sizeofdata]);
	//ZeroMemory(testData.get(),sizeofdata);
	memset(testData.get(),'M',sizeofdata);

	v->push_back(make_pair(std::move(testData),sizeofdata)); // the std::move() is a cast that produces an rvalue-reference to an object, to enable moving from it.
	return v;
}

std::vector< pair<std::unique_ptr<unsigned char>,int>> getVVblob()
{
	std::vector< pair<std::unique_ptr<unsigned char>,int>> v;

    std::unique_ptr<unsigned char> testData;
	int sizeofdata=30;
	testData.reset(new unsigned char[sizeofdata]);
	memset(testData.get(),'M',sizeofdata);

	v.push_back(make_pair(std::move(testData),sizeofdata)); // the std::move() is a cast that produces an rvalue-reference to an object, to enable moving from it.
	return v;
}

BOOST_AUTO_TEST_SUITE ( datadictionarycontrol ) // name of the test suite
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(returnstdvectorpair)
{
	cout<<"BOOST_AUTO_TEST(returnstdvector)\n{"<<endl;
	boost::shared_ptr< std::vector<pair<std::vector<unsigned char>,int>> > v = getV();
	cout << " returning std::vector from function " << endl;
	std::string strtmp(v->at(0).first.begin(),v->at(0).first.end()); 
	std::string strtmp2(v->at(1).first.begin(),v->at(1).first.end()); 
	std::cout << strtmp << " : " << v->at(0).second << endl;
	std::cout << strtmp2 <<" : " << v->at(1).second << endl;
	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(return_shared_ptr_stdvectorpairWithUnique_ptrToBlob)
{
	cout<<"BOOST_AUTO_TEST(return_shared_ptr_stdvectorpairWithUnique_ptrToBlob)\n{"<<endl;
	boost::shared_ptr< std::vector< pair<std::unique_ptr<unsigned char>,int>> > v = getVblob();
	cout << " returning shared_ptr std::vector from function " << endl;

	std::vector<unsigned char> test;
	test.assign(v->at(0).first.get(), v->at(0).first.get() + v->at(0).second);

	std::string strResult(test.begin(),test.end()); 
	std::cout << " value : " << strResult << endl << " size of value : " << v->at(0).second << endl;

	std::string expected = "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMM";
	BOOST_CHECK(expected == strResult);

	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(returnstdvectorpairWithUnique_ptrToBlob)
{
	cout<<"BOOST_AUTO_TEST(returnstdvectorpairWithUnique_ptrToBlob)\n{"<<endl;
	std::vector< pair<std::unique_ptr<unsigned char>,int>> v = getVVblob();
	cout << " returning std::vector from function " << endl;

	std::vector<unsigned char> test;
	test.assign(v.at(0).first.get(), v.at(0).first.get() + v.at(0).second);

	std::string strResult(test.begin(),test.end()); 
	std::cout << " value : " << strResult << endl << " size of value : " << v.at(0).second << endl;

	std::string expected = "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMM";
	BOOST_CHECK(expected == strResult);

	cout << " - using BOOST_FOREACH " << endl;
	BOOST_FOREACH( auto& chunk, v )
	{
		cout << " - " << chunk.second << endl;	
		BOOST_CHECK(chunk.second == 30);
		
		std::vector<unsigned char> test;
		test.assign(chunk.first.get(), chunk.first.get() + chunk.second);
		std::string strResult(test.begin(),test.end()); 
		 
		cout << " - " << strResult << endl;
		BOOST_CHECK(expected == strResult);

	}

	cout<<"}"<<endl;
}



BOOST_AUTO_TEST_CASE(datadictionarycontrol_instantiated)
{
	cout<<"BOOST_AUTO_TEST(datadictionarycontrol_instantiated)\n{"<<endl;
    CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
    BOOST_CHECK(ptestDataDictionaryControl != 0);
    delete ptestDataDictionaryControl;
	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(create_BFi_blockfile)
{
	cout<<"BOOST_AUTO_TEST(create_BFi_blockfile)\n{"<<endl;

    CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
    bool bFileCreated = ptestDataDictionaryControl->CreateBlockFile("test.BFi");

    BOOST_CHECK(bFileCreated == true);

    //Cleanup
    boost::filesystem::wpath file(L"test.BFi");
    if(boost::filesystem::exists(L"test.BFi"))
         boost::filesystem::remove(file);

	cout<<"}"<<endl;
}

//DEPRECATED - a file should be an attribut in a realm
BOOST_AUTO_TEST_CASE(create3Blockfiles)
{
	cout<<"BOOST_AUTO_TEST(create3Blockfiles)\n{"<<endl;
	// Create testfile to be put into .BFi file
    std::ofstream filestr;
	std::string testfilename("testfile.txt");

	filestr.open (testfilename.c_str());
	BOOST_CHECK(filestr.is_open());

	if (filestr.is_open())
	{
			filestr << " 0 This is a test file .";
			filestr << " 1 This is a test file .";
			filestr << " 2 This is a test file .";
			filestr << " 3 This is a test file .";
			filestr << " 4 This is a test file .";
			filestr << " 5 This is a test file .";
			filestr << " 6 This is a test file .";
			filestr << " 7 This is a test file .";
			filestr << " 8 This is a test file .";
			filestr << " 9 This is a test file .";
			filestr << "10 This is a test file .";
			filestr << "11 This is a test file .";
			filestr << "12 This is a test file .";
			filestr << "13 This is a test file .";
			filestr << "14 This is a test file .";
			filestr << "15 This is a test file .";
			filestr << "16 This is a test file .";
			filestr << "17 This is a test file .";
			filestr << "18 This is a test file .";
			filestr << "19 This is a test file .";
			filestr << "20 This is a test file .";
			filestr << "21 This is a test file .";
			filestr << "22 This is a test file .";
			filestr.close();
	}

	// Put testfile into .BFi file
   	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
	int amountOfBlocks = ptestDataDictionaryControl->splitFileIntoBlocks(testfilename);

	// file should be split in several blocks, pt. it is setup to make 3 blocks per file
	BOOST_CHECK(amountOfBlocks == 3);

	// verify that block files have been created
	BOOST_CHECK(boost::filesystem::exists("testfile.txt_1.BFi"));
	BOOST_CHECK(boost::filesystem::exists("testfile.txt_2.BFi"));
	BOOST_CHECK(boost::filesystem::exists("testfile.txt_3.BFi"));


    //Cleanup
    boost::filesystem::wpath file(L"testfile.txt");
    if(boost::filesystem::exists(L"testfile.txt"))
         boost::filesystem::remove(file);
    boost::filesystem::remove("testfile.txt_1.BFi");
    boost::filesystem::remove("testfile.txt_2.BFi");
    boost::filesystem::remove("testfile.txt_3.BFi");
	cout<<"}"<<endl;
}


BOOST_AUTO_TEST_CASE(writeBlockIntoBFiStructure)
{
	cout<<"BOOST_AUTO_TEST(writeBlockIntoBFiStructure)\n{"<<endl;
	std::string strTransGUID = "<empty>";

	// Create testfile to be put into .BFi file
	std::ofstream filestr;
	std::string testfilename("testfile.txt");

	filestr.open (testfilename.c_str());
	BOOST_CHECK(filestr.is_open());


	if (filestr.is_open())
	{
		filestr << " 0 This is a test file .";
		filestr << " 1 This is a test file .";
		filestr << " 2 This is a test file .";
		filestr << " 3 This is a test file .";
		filestr << " 4 This is a test file .";
		filestr << " 5 This is a test file .";
		filestr << " 6 This is a test file .";
		filestr << " 7 This is a test file .";
		filestr << " 8 This is a test file .";
		filestr << " 9 This is a test file .";
		filestr << "10 This is a test file .";
		filestr << "11 This is a test file .";
		filestr << "12 This is a test file .";
		filestr << "13 This is a test file .";
		filestr << "14 This is a test file .";
		filestr << "15 This is a test file .";
		filestr << "16 This is a test file .";
		filestr << "17 This is a test file .";
		filestr << "18 This is a test file .";
		filestr << "19 This is a test file .";
		filestr << "20 This is a test file .";
		filestr << "21 This is a test file .";
		filestr << "22 This is a test file .";
		filestr.close();
	}

	// put testfile into .BFi file
	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();

	int amountOfBlocks=-1;

	// read test file
	const char* fn = testfilename.c_str();
	vector< pair<char*,int> > vp;
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
			vp.push_back(make_pair(memblock,ws));
		}
		std::string _strMD5(CMD5((const char*)memblock).GetMD5s());
		strTransGUID = _strMD5;

	}
	else{
		std::string filename(fn);
		throw std::runtime_error("Error: Could NOT read file: " + filename );
	}

	if(vp.size()>0) {
		amountOfBlocks = vp.size();
		pair <char*,int> block;
		BlockRecord ans;
		int count=0;
		long aiid=0;
		long seq=0;
		bool bfirst=true;
		std::string attributName = "testfile";
		BOOST_FOREACH( block, vp )
		{
			boost::property_tree::ptree pt = ptestDataDictionaryControl->createBFiBlockRecord(bfirst,++aiid, ++seq, strTransGUID, attributName, testfilename.c_str(),block.first, block.second);

			BlockRecordEntry f;
			if(bfirst) {
				f.TransGUID = pt.get<std::string>("BlockRecord.TransGUID");
				f.chunk_id  = pt.get<std::string>("BlockRecord.chunk_id");
				f.aiid 		= pt.get<unsigned long>("BlockRecord.aiid");
				f.chunk_seq = pt.get<unsigned long>("BlockRecord.chunk_seq");
				cout << "aiid : " << f.aiid <<endl;
				BOOST_CHECK(f.aiid == ++count);
				cout << "chunk_id : " << f.chunk_id <<endl;
				// check if chunk_id is correct - it should be name of realm file, inorder to follow specs
				BOOST_CHECK(f.chunk_id == testfilename);
				cout << "chunk_seq : " << f.chunk_seq <<endl;
				cout << "TransGUID : " << f.TransGUID <<endl;
				f.chunk_ddid = pt.get<std::string>("BlockRecord.chunk_data.chunk_record.chunk_ddid");
				f.chunk_data_in_hex = pt.get<std::string>("BlockRecord.chunk_data.chunk_record.Data");
				f.chunk_size = pt.get<unsigned long>("BlockRecord.chunk_data.chunk_record.DataSize");
				f.chunk_md5 = pt.get<std::string>("BlockRecord.chunk_data.chunk_record.DataMD5");
			}
			else {
				f.chunk_ddid = pt.get<std::string>("chunk_record.chunk_ddid");
				f.chunk_data_in_hex = pt.get<std::string>("chunk_record.Data");
				f.chunk_size = pt.get<unsigned long>("chunk_record.DataSize");
				f.chunk_md5 = pt.get<std::string>("chunk_record.DataMD5");
			}
			bfirst=false;

			// check if data is in structure, and of correct size
			BOOST_CHECK(f.chunk_size == f.chunk_data_in_hex.size());
			// check if data has correct md5 sum
			std::string strMD5(CMD5((const char*)f.chunk_data_in_hex.c_str()).GetMD5s());
			BOOST_CHECK(f.chunk_md5 == strMD5);

			//TODO: check that data can be decoded !!!!
		}
	}

	//Cleanup
	boost::filesystem::remove(testfilename);

	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(DecodeAftersplitAttributIntoDEDchunks_small)
{
	cout<<"BOOST_AUTO_TEST(DecodeAftersplitAttributIntoDEDchunks_small)\n{"<<endl;

	std::string attributName = "name";
	std::string name = "Johnny Serup";	
	std::vector<unsigned char> attributValue(name.begin(), name.end());

	long maxDEDblockSize=65000; // should yield only one BlockRecord, since foto can be in one
	long maxDEDchunkSize=300; // should yield 1 chunk for this attribut

	// split data into several chunks of DED if size is bigger than DEDchunkSize
	long aiid=0;
	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
	std::vector< pair<std::vector<unsigned char>,int> > listOfDEDchunks = ptestDataDictionaryControl->splitAttributIntoDEDchunks(aiid, attributName, attributValue, maxDEDchunkSize);

	std::cout << "listOfDEDchunks : " << listOfDEDchunks.size() << '\n';
	// verify that data is in DED blocks
	BOOST_CHECK(listOfDEDchunks.size() == 1);

	// now decode and verify
	BOOST_FOREACH( auto &chunk, listOfDEDchunks )
	{
			cout << "decode outside function " << endl;
			cout << "--- chunk.second ; size of chunk " << chunk.second << endl;
			DED_PUT_DATA_IN_DECODER(decoder_ptr,chunk.first.data(),chunk.second);

			EntityChunkDataInfo _chunk;
			// decode data ...
			DED_GET_STRUCT_START( decoder_ptr, "chunk_record" );
			DED_GET_STDSTRING	( decoder_ptr, "attribut_chunk_id", _chunk.entity_chunk_id ); // key of particular item
			DED_GET_ULONG   	( decoder_ptr, "attribut_aiid", _chunk.aiid ); // this number is continuesly increasing all thruout the entries in this table
			DED_GET_ULONG   	( decoder_ptr, "attribut_chunk_seq", _chunk.entity_chunk_seq ); // sequence number of particular item
			DED_GET_STDVECTOR	( decoder_ptr, "attribut_chunk_data", _chunk.entity_chunk_data ); //
			DED_GET_STRUCT_END( decoder_ptr, "chunk_record" );

			cout << "--- entity_chunk_id : " << _chunk.entity_chunk_id << endl;
			cout << "--- entity_aiid : " << _chunk.aiid << endl;
			cout << "--- entity_chunk_seq : " << _chunk.entity_chunk_seq << endl;
			std::string strtmp(_chunk.entity_chunk_data.begin(), _chunk.entity_chunk_data.end());
			if(strtmp.size() < 30)
					cout << "--- entity_chunk_data : " << strtmp << endl;
			BOOST_CHECK(strtmp == name);
	}

	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(splitAttributIntoDEDchunks)
{
	cout<<"BOOST_AUTO_TEST(splitAttributIntoDEDchunks)\n{"<<endl;

	// use an image file as attribut value
	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
	std::string attributName = "foto";
	std::vector<unsigned char> FileDataBytesInVector;
	std::string fn = "testImage.png"; // should be of size 10.5 Kb
	std::ifstream is (fn, ios::binary);
	if (is)
	{
		long length = boost::filesystem::file_size(fn);
		std::cout << "[readFile] Reading file: " << fn << " ; amount " << length << " characters... \n";
		// Make sure receipient has room
        	FileDataBytesInVector.resize(length,0);
		//read content of infile
		is.read ((char*)&FileDataBytesInVector[0],length);
		std::cout << "[readFile] size: " << (int) FileDataBytesInVector.size() << '\n';
		std::cout << "[readFile] capacity: " << (int) FileDataBytesInVector.capacity() << '\n';
		std::cout << "[readFile] max_size: " << (int) FileDataBytesInVector.max_size() << '\n';
		is.close();
	}
	BOOST_CHECK(FileDataBytesInVector.size() > 0);

	long maxDEDblockSize=65000; // should yield only one BlockRecord, since foto can be in one
	long maxDEDchunkSize=300; // should yield several chunks for this attribut

	// split image data into several chunks of DED
	long aiid=0;
	std::vector< pair<std::vector<unsigned char>,int> > listOfDEDchunks = ptestDataDictionaryControl->splitAttributIntoDEDchunks(aiid, attributName, FileDataBytesInVector, maxDEDchunkSize);

	std::cout << "listOfDEDchunks : " << listOfDEDchunks.size() << '\n';
	// verify that image is in DED blocks
	BOOST_CHECK(listOfDEDchunks.size() == 35);


	BOOST_FOREACH( auto &chunk, listOfDEDchunks )
	{
			cout << "decode outside function " << endl;
			cout << "--- chunk.second ; size of chunk " << chunk.second << endl;
			DED_PUT_DATA_IN_DECODER(decoder_ptr,chunk.first.data(),chunk.second);

			EntityChunkDataInfo _chunk;
			// decode data ...
			DED_GET_STRUCT_START( decoder_ptr, "chunk_record" );
			DED_GET_STDSTRING	( decoder_ptr, "attribut_chunk_id", _chunk.entity_chunk_id ); // key of particular item
			DED_GET_ULONG   	( decoder_ptr, "attribut_aiid", _chunk.aiid ); // this number is continuesly increasing all thruout the entries in this table
			DED_GET_ULONG   	( decoder_ptr, "attribut_chunk_seq", _chunk.entity_chunk_seq ); // sequence number of particular item
			DED_GET_STDVECTOR	( decoder_ptr, "attribut_chunk_data", _chunk.entity_chunk_data ); //
			DED_GET_STRUCT_END( decoder_ptr, "chunk_record" );

			cout << "--- entity_chunk_id : " << _chunk.entity_chunk_id << endl;
			cout << "--- entity_aiid : " << _chunk.aiid << endl;
			cout << "--- entity_chunk_seq : " << _chunk.entity_chunk_seq << endl;
	}


	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(addChunkDataToBlockRecord)
{
	cout<<"BOOST_AUTO_TEST(addChunkDataToBlockRecord)\n{"<<endl;

	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
	std::string attributName = "foto";
	std::vector<unsigned char> FileDataBytesInVector;
	std::string fn = "testImage.png"; // should be of size 10.5 Kb
	std::ifstream is (fn, ios::binary);
	if (is)
	{
		long length = boost::filesystem::file_size(fn);
		std::cout << "[readFile] Reading file: " << fn << " ; amount " << length << " characters... \n";
		// Make sure receipient has room
		FileDataBytesInVector.resize(length,0);
		//read content of infile
		is.read ((char*)&FileDataBytesInVector[0],length);
		std::cout << "[readFile] size: " << (int) FileDataBytesInVector.size() << '\n';
		std::cout << "[readFile] capacity: " << (int) FileDataBytesInVector.capacity() << '\n';
		std::cout << "[readFile] max_size: " << (int) FileDataBytesInVector.max_size() << '\n';
		is.close();
	}

	BOOST_CHECK(FileDataBytesInVector.size() > 0);

	long maxBlockRecordSize=65000; // should yield only one BlockRecord, since foto can be in one
	long maxDEDchunkSize=300; // should yield several chunks for this attribut

	long aiid=0;
	std::vector< pair<std::vector<unsigned char>,int> > listOfDEDchunks = ptestDataDictionaryControl->splitAttributIntoDEDchunks(aiid, attributName, FileDataBytesInVector, maxDEDchunkSize);
	std::cout << "listOfDEDchunks : " << listOfDEDchunks.size() << '\n';
	BOOST_CHECK(listOfDEDchunks.size() == 35);


	aiid=0;
	std::string realmName = "profile";
	boost::property_tree::ptree pt = ptestDataDictionaryControl->addDEDchunksToBlockRecords(aiid, realmName, attributName, listOfDEDchunks, maxBlockRecordSize);

	BOOST_CHECK(pt.size() > 0);
	
	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(addBlockRecordToBlockEntity)
{
	cout<<"BOOST_AUTO_TEST(addBlockRecordToBlockEntity)\n{"<<endl;

	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
	std::string attributName = "foto"; // it should be ddid -- datadictionary id which refers to attribut description
	std::vector<unsigned char> FileDataBytesInVector;
	std::string fn = "testImage.png"; // should be of size 10.5 Kb
	std::ifstream is (fn, ios::binary);
	if (is)
	{
		long length = boost::filesystem::file_size(fn);
		std::cout << "[readFile] Reading file: " << fn << " ; amount " << length << " characters... \n";
		// Make sure receipient has room
		FileDataBytesInVector.resize(length,0);
		//read content of infile
		is.read ((char*)&FileDataBytesInVector[0],length);
		std::cout << "[readFile] size: " << (int) FileDataBytesInVector.size() << '\n';
		std::cout << "[readFile] capacity: " << (int) FileDataBytesInVector.capacity() << '\n';
		std::cout << "[readFile] max_size: " << (int) FileDataBytesInVector.max_size() << '\n';
		is.close();
	}

	BOOST_CHECK(FileDataBytesInVector.size() > 0);

	long maxBlockRecordSize=5250; // should yield in 3 BlockRecord, since foto cant be in one
	long maxDEDchunkSize=300; // should yield several chunks for this attribut

	long aiid=0;
	std::vector< pair<std::vector<unsigned char>,int> > listOfDEDchunks = ptestDataDictionaryControl->splitAttributIntoDEDchunks(aiid, attributName, FileDataBytesInVector, maxDEDchunkSize);
	std::cout << "listOfDEDchunks : " << listOfDEDchunks.size() << '\n';
	BOOST_CHECK(listOfDEDchunks.size() == 35);


	aiid=0;
	std::string realmName = "profile";
	boost::property_tree::ptree ptListOfBlockRecords = ptestDataDictionaryControl->addDEDchunksToBlockRecords(aiid, realmName, attributName, listOfDEDchunks, maxBlockRecordSize);

	BOOST_CHECK(ptListOfBlockRecords.size() > 0);

	//long maxBlockEntitySize=64000; // should result in 1 BlockEntity 
	long maxBlockEntitySize=27000; // should result in 2 BlockEntity 
	std::string transGuid = "E4C23762ED2823A27E62A64B95C024E7";
	boost::property_tree::ptree ptBlockEntity = ptestDataDictionaryControl->addBlockRecordToBlockEntity(transGuid, ptListOfBlockRecords, maxBlockEntitySize);

	BOOST_CHECK(ptBlockEntity.size()>0);

	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(writeBlockEntitiesToBFiFiles)
{
	cout<<"BOOST_AUTO_TEST(writeBlockEntitiesToBFiFiles)\n{"<<endl;

	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
	std::string attributName = "foto"; // it should be ddid -- datadictionary id which refers to attribut description
	std::vector<unsigned char> FileDataBytesInVector;
	std::string fn = "testImage.png"; // should be of size 10.5 Kb
	std::ifstream is (fn, ios::binary);
	if (is)
	{
		long length = boost::filesystem::file_size(fn);
		std::cout << "[readFile] Reading file: " << fn << " ; amount " << length << " characters... \n";
		// Make sure receipient has room
		FileDataBytesInVector.resize(length,0);
		//read content of infile
		is.read ((char*)&FileDataBytesInVector[0],length);
		std::cout << "[readFile] size: " << (int) FileDataBytesInVector.size() << '\n';
		std::cout << "[readFile] capacity: " << (int) FileDataBytesInVector.capacity() << '\n';
		std::cout << "[readFile] max_size: " << (int) FileDataBytesInVector.max_size() << '\n';
		is.close();
	}

	BOOST_CHECK(FileDataBytesInVector.size() > 0);

	long maxBlockRecordSize=5250; // should yield in 3 BlockRecord, since foto cant be in one
	long maxDEDchunkSize=300; // should yield several chunks for this attribut

	long aiid=0;
	std::vector< pair<std::vector<unsigned char>,int> > listOfDEDchunks = ptestDataDictionaryControl->splitAttributIntoDEDchunks(aiid, attributName, FileDataBytesInVector, maxDEDchunkSize);
	std::cout << "listOfDEDchunks : " << listOfDEDchunks.size() << '\n';
	BOOST_CHECK(listOfDEDchunks.size() == 35);


	aiid=0;
	std::string realmName = "profile";
	boost::property_tree::ptree ptListOfBlockRecords = ptestDataDictionaryControl->addDEDchunksToBlockRecords(aiid, realmName, attributName, listOfDEDchunks, maxBlockRecordSize);

	BOOST_CHECK(ptListOfBlockRecords.size() > 0);

	long maxBlockEntitySize=27000; // should result in 2 BlockEntity 
	std::string transGuid = "E4C23762ED2823A27E62A64B95C024E7";
	boost::property_tree::ptree ptBlockEntity = ptestDataDictionaryControl->addBlockRecordToBlockEntity(transGuid, ptListOfBlockRecords, maxBlockEntitySize);

	BOOST_CHECK(ptBlockEntity.size()>0);

	// write test xml file
	ofstream blockFile1 ("xmlresult2.xml", ios::out | ios::binary);
	write_xml(blockFile1, ptBlockEntity);

	std::vector< pair<std::string ,int> > listOfBlockEntityFiles = ptestDataDictionaryControl->writeBlockEntityToBFiFile(ptBlockEntity);

	cout << "amount of BlockEntityFiles created : " << listOfBlockEntityFiles.size() << endl; 
	BOOST_CHECK(listOfBlockEntityFiles.size()==2);

	pair <std::string,int> block;
	BOOST_FOREACH(block, listOfBlockEntityFiles)
	{
			cout << "- OK Created file : " << block.first << " size : " << block.second << endl;
			cout << "cleanup file " << endl;
			boost::filesystem::remove(block.first);
	}

	// test with only 1 blockentity file
	//
	maxBlockEntitySize=64000; // should result in 1 BlockEntity 
	transGuid = "F4C23762ED2823A27E62A64B95C024EF";
	boost::property_tree::ptree ptBlockEntity2 = ptestDataDictionaryControl->addBlockRecordToBlockEntity(transGuid, ptListOfBlockRecords, maxBlockEntitySize);

	// write test xml file
	ofstream blockFile2 ("xmlresult3.xml", ios::out | ios::binary);
	write_xml(blockFile2, ptBlockEntity2);


	BOOST_CHECK(ptBlockEntity2.size()>0);

	listOfBlockEntityFiles = ptestDataDictionaryControl->writeBlockEntityToBFiFile(ptBlockEntity2);

	cout << "amount of BlockEntityFiles created : " << listOfBlockEntityFiles.size() << endl; 
	BOOST_CHECK(listOfBlockEntityFiles.size()==1);

	BOOST_FOREACH(block, listOfBlockEntityFiles)
	{
			cout << "- OK Created file : " << block.first << " size : " << block.second << endl;
			cout << "cleanup file " << endl;
			boost::filesystem::remove(block.first);
	}

	cout<<"}"<<endl;
}


BOOST_AUTO_TEST_CASE(addAttributToBlockRecord)
{
	using boost::optional;
	using boost::property_tree::ptree;

	cout<<"BOOST_AUTO_TEST(addAttributToBlockRecord)\n{"<<endl;

	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
	ptree ptListOfBlockRecords;
	std::string attributName = "name";
	std::string name = "Johnny Serup";	
	std::vector<unsigned char> attributValue(name.begin(), name.end());
	std::string realmName = "profile";
	long maxBlockRecordSize=64000;	

	std::string transGuid = "F4C23762ED2823A27E62A64B95C024EF";
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid, ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName, attributValue)); 
 	// check that list now contain basic 'listOfBlockRecords' - which is necessary	
	optional< ptree& > child = ptListOfBlockRecords.get_child_optional( "listOfBlockRecords" );
	BOOST_CHECK(child);

	// verify that attribut has been added
	BOOST_FOREACH(ptree::value_type &vt, ptListOfBlockRecords.get_child("listOfBlockRecords"))
	{
		cout << " - first : " << vt.first << endl;

	}

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record.chunk_ddid" );
	BOOST_CHECK(child);

	std::string chunk_ddid = ptListOfBlockRecords.get_child( "BlockRecord.chunk_data.chunk_record.chunk_ddid" ).data();
	cout << "chunk_ddid : " << chunk_ddid << endl;		
	BOOST_CHECK(chunk_ddid == attributName);	

	// fetch Data from xml node
	std::string hexdata = ptListOfBlockRecords.get_child( "BlockRecord.chunk_data.chunk_record.Data" ).data();
	// convert Data 
	unsigned char* data_in_unhexed_buf = (unsigned char*) malloc (hexdata.size());
        ZeroMemory(data_in_unhexed_buf,hexdata.size()); // make sure no garbage is inside the newly allocated space
        boost::algorithm::unhex(hexdata.begin(),hexdata.end(), data_in_unhexed_buf);// convert the hex array to an array containing byte values

	cout << "hexdata : < " << hexdata << " > " << endl; 
        cout << "hexdata size: " << hexdata.size() << endl;
	// initialize decoder with Data 
        DED_PUT_DATA_IN_DECODER(decoder_ptr,data_in_unhexed_buf,hexdata.size());
        BOOST_CHECK(decoder_ptr != 0);

        EntityChunkDataInfo _chunk;
 	// decode data ...
        DED_GET_STRUCT_START( decoder_ptr, "chunk_record" );
            BOOST_CHECK(DED_GET_STDSTRING	( decoder_ptr, "attribut_chunk_id", _chunk.entity_chunk_id )); // key of particular item
            DED_GET_ULONG   	( decoder_ptr, "attribut_aiid", _chunk.aiid ); // this number is continuesly increasing all thruout the entries in this table
            DED_GET_ULONG   	( decoder_ptr, "attribut_chunk_seq", _chunk.entity_chunk_seq ); // sequence number of particular item
            DED_GET_STDVECTOR	( decoder_ptr, "attribut_chunk_data", _chunk.entity_chunk_data ); //
        DED_GET_STRUCT_END( decoder_ptr, "chunk_record" );
	
	cout << "entity_chunk_id : " << _chunk.entity_chunk_id << endl;
	cout << "entity_aiid : " << _chunk.aiid << endl;
	cout << "entity_chunk_seq : " << _chunk.entity_chunk_seq << endl;

	// verify decoded Data
	BOOST_CHECK(_chunk.entity_chunk_id == attributName); 
	std::string strtmp(_chunk.entity_chunk_data.begin(), _chunk.entity_chunk_data.end());
	BOOST_CHECK(strtmp == name); 
	BOOST_CHECK(_chunk.entity_chunk_data == attributValue); 

		
	cout<<"}"<<endl;
}



BOOST_AUTO_TEST_CASE(add2AttributsToBlockRecord)
{
	using boost::optional;
	using boost::property_tree::ptree;

	cout<<"BOOST_AUTO_TEST(add2AttributsToBlockRecord)\n{"<<endl;

	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
	ptree ptListOfBlockRecords;

	// attribut 1
	std::string attributName = "name";
	std::string name = "Johnny Serup";	
	std::vector<unsigned char> attributValue(name.begin(), name.end());

	// attribut 2
	std::string attributName2 = "mobil";
	std::string mobil = "555 - 332 211 900";	
	std::vector<unsigned char> attributValue2(mobil.begin(), mobil.end());

	std::string realmName = "profile";
	long maxBlockRecordSize=64000;	


	cout << "BlockRecord size before: " << maxBlockRecordSize << endl;
	std::string transGuid = "F4C23762ED2823A27E62A64B95C024EF";
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName, attributValue)); 
	cout << "BlockRecord size after 1 attribut add : " << maxBlockRecordSize << endl;
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName2, attributValue2)); 
	cout << "BlockRecord size after 2 atrribut add : " << maxBlockRecordSize << endl;
 

	// check that list now contain basic 'listOfBlockRecords' - which is necessary	
	optional< ptree& > child = ptListOfBlockRecords.get_child_optional( "listOfBlockRecords" );
	BOOST_CHECK(child);

	// verify that BlockRecord has been added
	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data" );
	BOOST_CHECK(child);

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record" );
	BOOST_CHECK(child);

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record.chunk_ddid" );
	BOOST_CHECK(child);

	cout << "________________________________________" << endl;
	cout << "attributs added : " << endl;
	int amountOfBlockRecords = 0;
	int amountOfchunk_records = 0;
	std::string hexdata_attribut1="";
	std::string hexdata_attribut2="";


	BOOST_FOREACH(ptree::value_type &vt, ptListOfBlockRecords.get_child("listOfBlockRecords"))
	{
		cout << " - record id : " << vt.second.get_child("chunk_id").data() << endl;
		if(vt.first == "BlockRecord")
		{
			amountOfBlockRecords++;
			BOOST_FOREACH(ptree::value_type &vt2 , vt.second)
			{
				if(vt2.first == "chunk_data")
				{
					cout << " - chunk_data : " << vt2.first << endl; 
					BOOST_FOREACH(ptree::value_type &vt3, vt2.second)
					{
						if(vt3.first == "chunk_record") {
							cout << " -- chunk_record : " << vt3.second.get_child("chunk_ddid").data() << endl; 
							amountOfchunk_records++;
							if(amountOfchunk_records==1) {
								hexdata_attribut1 = vt3.second.get_child("Data").data();
							}
							if(amountOfchunk_records==2) {
								hexdata_attribut2 = vt3.second.get_child("Data").data();
							}
						}
					}
				}
			}
		}
	}

	BOOST_CHECK(amountOfBlockRecords == 1); // Only one BlockRecord - the attributs should be added to BlockRecord until it is full, then new BlockRecord will be added
	BOOST_CHECK(amountOfchunk_records == 2); // There should be two chunk_records - one for attribut1 and one for attribut2
	cout << "________________________________________" << endl;

	cout << "verify if data is correctly stored - attributs 1 and 2" << endl;		

	std::string chunk_ddid = ptListOfBlockRecords.get_child( "BlockRecord.chunk_data.chunk_record.chunk_ddid" ).data();
	cout << "chunk_ddid : " << chunk_ddid << endl;		
	BOOST_CHECK(chunk_ddid == attributName);	


	// attribut 1
	// fetch Data from xml node
	//std::string hexdata = ptListOfBlockRecords.get_child( "BlockRecord.chunk_data.chunk_record.Data" ).data();
	std::string hexdata = hexdata_attribut1;

	// convert Data 
	unsigned char* data_in_unhexed_buf = (unsigned char*) malloc (hexdata.size());
        ZeroMemory(data_in_unhexed_buf,hexdata.size()); // make sure no garbage is inside the newly allocated space
        boost::algorithm::unhex(hexdata.begin(),hexdata.end(), data_in_unhexed_buf);// convert the hex array to an array containing byte values

	cout << "hexdata : < " << hexdata << " > " << endl; 
        cout << "hexdata size: " << hexdata.size() << endl;
	long attribut1_aiid=0;
	// initialize decoder with Data 
	{
        DED_PUT_DATA_IN_DECODER(decoder_ptr,data_in_unhexed_buf,hexdata.size());
        BOOST_CHECK(decoder_ptr != 0);

        EntityChunkDataInfo _chunk;
 	// decode data ...
        DED_GET_STRUCT_START( decoder_ptr, "chunk_record" );
            BOOST_CHECK(DED_GET_STDSTRING	( decoder_ptr, "attribut_chunk_id", _chunk.entity_chunk_id )); // key of particular item
            DED_GET_ULONG   	( decoder_ptr, "attribut_aiid", _chunk.aiid ); // this number is continuesly increasing all thruout the entries in this table
            DED_GET_ULONG   	( decoder_ptr, "attribut_chunk_seq", _chunk.entity_chunk_seq ); // sequence number of particular item
            DED_GET_STDVECTOR	( decoder_ptr, "attribut_chunk_data", _chunk.entity_chunk_data ); //
        DED_GET_STRUCT_END( decoder_ptr, "chunk_record" );
	
	attribut1_aiid = _chunk.aiid;

	cout << "entity_chunk_id : " << _chunk.entity_chunk_id << endl;
	cout << "entity_aiid : " << _chunk.aiid << endl;
	cout << "entity_chunk_seq : " << _chunk.entity_chunk_seq << endl;

	// verify decoded Data
	BOOST_CHECK(_chunk.entity_chunk_id == attributName); 
	std::string strtmp(_chunk.entity_chunk_data.begin(), _chunk.entity_chunk_data.end());
	BOOST_CHECK(strtmp == name); 
	BOOST_CHECK(_chunk.entity_chunk_data == attributValue); 
	BOOST_CHECK(_chunk.entity_chunk_seq == 1);
	}

	// attribut 2
	// fetch Data from xml node
	hexdata = hexdata_attribut2;

	// convert Data 
	unsigned char* data_in_unhexed_buf2 = (unsigned char*) malloc (hexdata.size());
        ZeroMemory(data_in_unhexed_buf2,hexdata.size()); // make sure no garbage is inside the newly allocated space
        boost::algorithm::unhex(hexdata.begin(),hexdata.end(), data_in_unhexed_buf2);// convert the hex array to an array containing byte values

	cout << "hexdata : < " << hexdata << " > " << endl; 
        cout << "hexdata size: " << hexdata.size() << endl;
	// initialize decoder with Data 
	{
        DED_PUT_DATA_IN_DECODER(decoder_ptr2,data_in_unhexed_buf2,hexdata.size());
        BOOST_CHECK(decoder_ptr2 != 0);

        EntityChunkDataInfo _chunk2;
 	// decode data ...
        DED_GET_STRUCT_START( decoder_ptr2, "chunk_record" );
            BOOST_CHECK(DED_GET_STDSTRING	( decoder_ptr2, "attribut_chunk_id", _chunk2.entity_chunk_id )); // key of particular item
            DED_GET_ULONG   	( decoder_ptr2, "attribut_aiid", _chunk2.aiid ); // this number is continuesly increasing all thruout the entries in this table
            DED_GET_ULONG   	( decoder_ptr2, "attribut_chunk_seq", _chunk2.entity_chunk_seq ); // sequence number of particular item
            DED_GET_STDVECTOR	( decoder_ptr2, "attribut_chunk_data", _chunk2.entity_chunk_data ); //
        DED_GET_STRUCT_END( decoder_ptr2, "chunk_record" );
	
	cout << "entity_chunk_id : " << _chunk2.entity_chunk_id << endl;
	cout << "entity_aiid : " << _chunk2.aiid << endl;
	cout << "entity_chunk_seq : " << _chunk2.entity_chunk_seq << endl;

	// verify decoded Data
	BOOST_CHECK(_chunk2.entity_chunk_id == attributName2); 
	std::string strtmp2(_chunk2.entity_chunk_data.begin(), _chunk2.entity_chunk_data.end());
	BOOST_CHECK(strtmp2 == mobil); 
	BOOST_CHECK(_chunk2.entity_chunk_data == attributValue2); 
	BOOST_CHECK(_chunk2.entity_chunk_seq == 1);
	// verify that internal aiid is incrementing
	BOOST_CHECK(_chunk2.aiid > attribut1_aiid);
	}
	
	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(add2AttributsOneLargeOneSmallToBlockRecord)
{
	using boost::optional;
	using boost::property_tree::ptree;

	cout<<"BOOST_AUTO_TEST(add2AttributsOneLargeOneSmallToBlockRecord)\n{"<<endl;

	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
	ptree ptListOfBlockRecords;
	ptListOfBlockRecords.clear();
	

	// attribut 1 - large
	std::string attributName = "foto"; // it should be ddid -- datadictionary id which refers to attribut description
	std::vector<unsigned char> attributValue;
	std::string fn = "testImage.png"; // should be of size 10.5 Kb
	std::ifstream is (fn, ios::binary);
	if (is)
	{
		long length = boost::filesystem::file_size(fn);
		std::cout << "[readFile] Reading file: " << fn << " ; amount " << length << " characters... \n";
		// Make sure receipient has room
		attributValue.resize(length,0);
		//read content of infile
		is.read ((char*)&attributValue[0],length);
		std::cout << "[readFile] size: " << (int) attributValue.size() << '\n';
		std::cout << "[readFile] capacity: " << (int) attributValue.capacity() << '\n';
		std::cout << "[readFile] max_size: " << (int) attributValue.max_size() << '\n';
		is.close();
	}
	BOOST_CHECK(attributValue.size() > 0);

	// attribut 2
	std::string attributName2 = "mobil";
	std::string mobil = "555 - 332 211 900";	
	std::vector<unsigned char> attributValue2(mobil.begin(), mobil.end());

	std::string realmName = "profile";
	long maxBlockRecordSize=64000;	


	cout << "BlockRecord size before: " << maxBlockRecordSize << endl;
	std::string transGuid = "F4C23762ED2823A27E62A64B95C024EF";
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName, attributValue)); 
	cout << "BlockRecord size after 1 attribut add : " << maxBlockRecordSize << endl;
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName2, attributValue2)); 
	cout << "BlockRecord size after 2 atrribut add : " << maxBlockRecordSize << endl;
 

	// check that list now contain basic 'listOfBlockRecords' - which is necessary	
	optional< ptree& > child = ptListOfBlockRecords.get_child_optional( "listOfBlockRecords" );
	BOOST_CHECK(child);

	// verify that BlockRecord has been added
	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data" );
	BOOST_CHECK(child);

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record" );
	BOOST_CHECK(child);

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record.chunk_ddid" );
	BOOST_CHECK(child);

	long maxBlockEntitySize=27000; // should result in 1 BlockEntity 
	boost::property_tree::ptree ptBlockEntity = ptestDataDictionaryControl->addBlockRecordToBlockEntity(transGuid, ptListOfBlockRecords, maxBlockEntitySize);
	BOOST_CHECK(ptBlockEntity.size()>0);

	cout << "*{{{" << endl;
	write_xml(std::cout, ptBlockEntity, boost::property_tree::xml_writer_make_settings<std::string>('\t', 1) );
	cout << "*}}}" << endl;

	std::vector< pair<std::string ,int> > listOfBlockEntityFiles = ptestDataDictionaryControl->writeBlockEntityToBFiFile(ptBlockEntity);
	BOOST_CHECK(listOfBlockEntityFiles.size()==1);

	pair <std::string,int> block;
	BOOST_FOREACH(block, listOfBlockEntityFiles)
	{
		cout << "- OK Created file : " << block.first << " size : " << block.second << endl;
		cout << "cleanup file " << endl;
		boost::filesystem::remove(block.first);
	}

	cout << "________________________________________" << endl;
	cout << "attributs added : " << endl;
	int amountOfBlockRecords = 0;
	int amountOfAttributchunk_records = 0;
	std::string hexdata_attribut1="";
	std::string hexdata_attribut2="";
	std::string chunk_ddid2="";

	BOOST_FOREACH(ptree::value_type &vt, ptListOfBlockRecords.get_child("listOfBlockRecords"))
	{
		cout << " - record id : " << vt.second.get_child("chunk_id").data() << endl;
		if(vt.first == "BlockRecord")
		{
			amountOfBlockRecords++;
			BOOST_FOREACH(ptree::value_type &vt2 , vt.second)
			{
				if(vt2.first == "chunk_data")
				{
					cout << " - chunk_data : " << vt2.first; 
					std::string strPrevId = "";
					BOOST_FOREACH(ptree::value_type &vt3, vt2.second)
					{
						if(vt3.first == "chunk_record") {
							if(strPrevId != vt3.second.get_child("chunk_ddid").data()) {
								strPrevId = vt3.second.get_child("chunk_ddid").data();
								cout << endl;
								cout << " -- chunk_record : " << vt3.second.get_child("chunk_ddid").data() << " "; 
								amountOfAttributchunk_records++;
								if(amountOfAttributchunk_records==2) { // fetch only 2nd attribut, since that only has one record - 1st attribut is foto and consists of many records
									chunk_ddid2 = strPrevId;
									cout << endl;	
									hexdata_attribut2 = vt3.second.get_child("Data").data();
								}
							}
							else
								cout << ".";
						}
					}
				}
			}
		}
	}

	BOOST_CHECK(amountOfBlockRecords == 1); // Only one BlockRecord - the attributs should be added to BlockRecord until it is full, then new BlockRecord will be added
	BOOST_CHECK(amountOfAttributchunk_records == 2); // There should be two chunk_records - one for attribut1 and one for attribut2
	cout << "________________________________________" << endl;


	cout << "verify if data is correctly stored - attributs 1 and 2" << endl;		

	std::string chunk_ddid = ptListOfBlockRecords.get_child( "BlockRecord.chunk_data.chunk_record.chunk_ddid" ).data();
	cout << "chunk_ddid : " << chunk_ddid << endl;		
	BOOST_CHECK(chunk_ddid == attributName);	


	// attribut 1
	// fetch Data from xml node
	//TODO: assemble all chunks to a full attribut then compare with original
	//
	cout << "TODO:  assemble all chunks to a full attribut then compare with original" << endl;

	// attribut 2
	// fetch Data from xml node
	std::string hexdata = hexdata_attribut2;
	cout << "chunk_ddid : " << chunk_ddid2 << endl;		

	// convert Data 
	unsigned char* data_in_unhexed_buf2 = (unsigned char*) malloc (hexdata.size());
        ZeroMemory(data_in_unhexed_buf2,hexdata.size()); // make sure no garbage is inside the newly allocated space
        boost::algorithm::unhex(hexdata.begin(),hexdata.end(), data_in_unhexed_buf2);// convert the hex array to an array containing byte values

	cout << "hexdata : < " << hexdata << " > " << endl; 
	cout << "hexdata size: " << hexdata.size() << endl;
	// initialize decoder with Data 
	{
        DED_PUT_DATA_IN_DECODER(decoder_ptr2,data_in_unhexed_buf2,hexdata.size());
        BOOST_CHECK(decoder_ptr2 != 0);

        EntityChunkDataInfo _chunk2;
		// decode data ...
        DED_GET_STRUCT_START( decoder_ptr2, "chunk_record" );
            BOOST_CHECK(DED_GET_STDSTRING	( decoder_ptr2, "attribut_chunk_id", _chunk2.entity_chunk_id )); // key of particular item
            DED_GET_ULONG   	( decoder_ptr2, "attribut_aiid", _chunk2.aiid ); // this number is continuesly increasing all thruout the entries in this table
            DED_GET_ULONG   	( decoder_ptr2, "attribut_chunk_seq", _chunk2.entity_chunk_seq ); // sequence number of particular item
            DED_GET_STDVECTOR	( decoder_ptr2, "attribut_chunk_data", _chunk2.entity_chunk_data ); //
        DED_GET_STRUCT_END( decoder_ptr2, "chunk_record" );
	
		cout << "entity_chunk_id : " << _chunk2.entity_chunk_id << endl;
		cout << "entity_aiid : " << _chunk2.aiid << endl;
		cout << "entity_chunk_seq : " << _chunk2.entity_chunk_seq << endl;

		// verify decoded Data
		BOOST_CHECK(_chunk2.entity_chunk_id == attributName2); 
		std::string strtmp2(_chunk2.entity_chunk_data.begin(), _chunk2.entity_chunk_data.end());
		BOOST_CHECK(strtmp2 == mobil); 
		BOOST_CHECK(_chunk2.entity_chunk_data == attributValue2); 
		BOOST_CHECK(_chunk2.entity_chunk_seq == 1);
	}
	
	cout<<"}"<<endl;
}


BOOST_AUTO_TEST_CASE(listDataDictionaryAttributs)
{
	cout<<"BOOST_AUTO_TEST(listDataDictionaryAttributs)\n{"<<endl;

	using boost::optional;
	using boost::property_tree::ptree;

	CDataDictionaryControl *pDDC = new CDataDictionaryControl();
	std::list<std::string> listResult = pDDC->ls();	
	BOOST_FOREACH(std::string attribut, listResult)
	{
		cout << "- OK attribut : " << attribut << endl;
	}
	BOOST_CHECK(listResult.size() <= 0);

	// create BFi files
	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
	ptree ptListOfBlockRecords;

	// attribut 1
	std::string attributName = "name";
	std::string name = "Johnny Serup";	
	std::vector<unsigned char> attributValue(name.begin(), name.end());

	// attribut 2
	std::string attributName2 = "mobil";
	std::string mobil = "555 - 332 211 900";	
	std::vector<unsigned char> attributValue2(mobil.begin(), mobil.end());

	std::string realmName = "profile";
	long maxBlockRecordSize=64000;	


	cout << "BlockRecord size before: " << maxBlockRecordSize << endl;
	std::string transGuid = "F4C23762ED2823A27E62A64B95C024EF";
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName, attributValue)); 
	cout << "BlockRecord size after 1 attribut add : " << maxBlockRecordSize << endl;
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName2, attributValue2)); 
	cout << "BlockRecord size after 2 atrribut add : " << maxBlockRecordSize << endl;
 
	long maxBlockEntitySize=27000; // should result in 1 BlockEntity 
	boost::property_tree::ptree ptBlockEntity = ptestDataDictionaryControl->addBlockRecordToBlockEntity(transGuid, ptListOfBlockRecords, maxBlockEntitySize);
	BOOST_CHECK(ptBlockEntity.size()>0);

	cout << "*{{{" << endl;
	write_xml(std::cout, ptBlockEntity, boost::property_tree::xml_writer_make_settings<std::string>('\t', 1) );
	cout << "*}}}" << endl;

	std::vector< pair<std::string ,int> > listOfBlockEntityFiles = ptestDataDictionaryControl->writeBlockEntityToBFiFile(ptBlockEntity);
	BOOST_CHECK(listOfBlockEntityFiles.size()==1);

	pair <std::string,int> block;
	std::list<std::string> listBFiFiles;
	BOOST_FOREACH(block, listOfBlockEntityFiles)
	{
		cout << "- OK Created file : " << block.first << " size : " << block.second << endl;
		listBFiFiles.push_back(block.first);
	}


	// check that list now contain basic 'listOfBlockRecords' - which is necessary	
	optional< ptree& > child = ptListOfBlockRecords.get_child_optional( "listOfBlockRecords" );
	BOOST_CHECK(child);

	// verify that BlockRecord has been added
	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data" );
	BOOST_CHECK(child);

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record" );
	BOOST_CHECK(child);

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record.chunk_ddid" );
	BOOST_CHECK(child);

	cout << "________________________________________" << endl;
	cout << "attributs added : " << endl;
		
	int amountOfBlockRecords = 0;
	int amountOfchunk_records = 0;
	std::string hexdata_attribut1="";
	std::string hexdata_attribut2="";


	BOOST_FOREACH(ptree::value_type &vt, ptListOfBlockRecords.get_child("listOfBlockRecords"))
	{
		cout << " - record id : " << vt.second.get_child("chunk_id").data() << endl;
		if(vt.first == "BlockRecord")
		{
			amountOfBlockRecords++;
			BOOST_FOREACH(ptree::value_type &vt2 , vt.second)
			{
				if(vt2.first == "chunk_data")
				{
					cout << " - chunk_data : " << vt2.first << endl; 
					BOOST_FOREACH(ptree::value_type &vt3, vt2.second)
					{
						if(vt3.first == "chunk_record") {
							cout << " -- chunk_record : " << vt3.second.get_child("chunk_ddid").data() << endl; 
							amountOfchunk_records++;
							if(amountOfchunk_records==1) {
								hexdata_attribut1 = vt3.second.get_child("Data").data();
							}
							if(amountOfchunk_records==2) {
								hexdata_attribut2 = vt3.second.get_child("Data").data();
							}
						}
					}
				}
			}
		}
	}

	BOOST_CHECK(amountOfBlockRecords == 1); // Only one BlockRecord - the attributs should be added to BlockRecord until it is full, then new BlockRecord will be added
	BOOST_CHECK(amountOfchunk_records == 2); // There should be two chunk_records - one for attribut1 and one for attribut2
	cout << "________________________________________" << endl;


	listResult = pDDC->ls();	
	//expected : reads like this: <GUID> has a profile folder with attribut name and mobil
	std::string expected1 =  "F4C23762ED2823A27E62A64B95C024EF./profile/name";
	std::string expected2 =  "F4C23762ED2823A27E62A64B95C024EF./profile/mobil";

	BOOST_CHECK(listResult.size() > 0);

	int c=0;
	BOOST_FOREACH(std::string attribut, listResult)
	{
		c++;
		cout << "- OK attribut : " << attribut << endl;
		if(c==1) BOOST_CHECK(expected1 == attribut);
		if(c==2) BOOST_CHECK(expected2 == attribut);
	}


	cout << "________________________________________" << endl;
	// Clean up section - must be in bottom
	BOOST_FOREACH(std::string filename, listBFiFiles)
	{
		cout << "- OK Cleanup file : " << filename << endl;
		boost::filesystem::path p = boost::filesystem::path(filename);
//		std::cout << "filename and extension : " << p.filename().string() << std::endl; // file.ext
//		std::cout << "filename only          : " << p.stem().string() << std::endl;     // file
		boost::filesystem::remove(filename);
	}


	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(listDataDictionaryAttributsWithOneLarge)
{
	cout<<"BOOST_AUTO_TEST(listDataDictionaryAttributsWithOneLarge)\n{"<<endl;

	using boost::optional;
	using boost::property_tree::ptree;

	CDataDictionaryControl *pDDC = new CDataDictionaryControl();
	std::list<std::string> listResult = pDDC->ls();	
	BOOST_FOREACH(std::string attribut, listResult)
	{
		cout << "- OK attribut : " << attribut << endl;
	}
	BOOST_CHECK(listResult.size() <= 0);

	// create BFi files
	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
	ptree ptListOfBlockRecords;

	// attribut 1
	std::string attributName = "name";
	std::string name = "Johnny Serup";	
	std::vector<unsigned char> attributValue(name.begin(), name.end());

	// attribut 2
	std::string attributName2 = "mobil";
	std::string mobil = "555 - 332 211 900";	
	std::vector<unsigned char> attributValue2(mobil.begin(), mobil.end());

	// attribut 3 - large
	std::string attributName3= "foto"; // it should be ddid -- datadictionary id which refers to attribut description
	std::vector<unsigned char> attributValue3;
	std::string fn = "testImage.png"; // should be of size 10.5 Kb
	std::ifstream is (fn, ios::binary);
	if (is)
	{
		long length = boost::filesystem::file_size(fn);
		std::cout << "[readFile] Reading file: " << fn << " ; amount " << length << " characters... \n";
		// Make sure receipient has room
		attributValue3.resize(length,0);
		//read content of infile
		is.read ((char*)&attributValue3[0],length);
		std::cout << "[readFile] size: " << (int) attributValue3.size() << '\n';
		std::cout << "[readFile] capacity: " << (int) attributValue3.capacity() << '\n';
		std::cout << "[readFile] max_size: " << (int) attributValue3.max_size() << '\n';
		is.close();
	}
	BOOST_CHECK(attributValue3.size() > 0);

	std::string realmName = "profile";
	long maxBlockRecordSize=27000;	


	cout << "BlockRecord size before: " << maxBlockRecordSize << endl;
	std::string transGuid = "F4C23762ED2823A27E62A64B95C024EF";
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName, attributValue)); 
	cout << "BlockRecord size after 1 attribut add : " << maxBlockRecordSize << endl;
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName2, attributValue2)); 
	cout << "BlockRecord size after 2 atrribut add : " << maxBlockRecordSize << endl;
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName3, attributValue3)); 
	cout << "BlockRecord size after 3 atrribut add : " << maxBlockRecordSize << endl;
	 
	long maxBlockEntitySize=37000; // should result in 1 BlockEntity 
	boost::property_tree::ptree ptBlockEntity = ptestDataDictionaryControl->addBlockRecordToBlockEntity(transGuid, ptListOfBlockRecords, maxBlockEntitySize);
	BOOST_CHECK(ptBlockEntity.size()>0);

	cout << "*{{{" << endl;
	write_xml(std::cout, ptBlockEntity, boost::property_tree::xml_writer_make_settings<std::string>('\t', 1) );
	cout << "*}}}" << endl;

	std::vector< pair<std::string ,int> > listOfBlockEntityFiles = ptestDataDictionaryControl->writeBlockEntityToBFiFile(ptBlockEntity);
	BOOST_CHECK(listOfBlockEntityFiles.size()==1);

	pair <std::string,int> block;
	std::list<std::string> listBFiFiles;
	BOOST_FOREACH(block, listOfBlockEntityFiles)
	{
		cout << "- OK Created file : " << block.first << " size : " << block.second << endl;
		listBFiFiles.push_back(block.first);
	}


	// check that list now contain basic 'listOfBlockRecords' - which is necessary	
	optional< ptree& > child = ptListOfBlockRecords.get_child_optional( "listOfBlockRecords" );
	BOOST_CHECK(child);

	// verify that BlockRecord has been added
	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data" );
	BOOST_CHECK(child);

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record" );
	BOOST_CHECK(child);

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record.chunk_ddid" );
	BOOST_CHECK(child);

	cout << "________________________________________" << endl;
	cout << "attributs added : " << endl;
		
	int amountOfBlockRecords = 0;
	int amountOfchunk_records = 0;
	std::string hexdata_attribut1="";
	std::string hexdata_attribut2="";


	BOOST_FOREACH(ptree::value_type &vt, ptListOfBlockRecords.get_child("listOfBlockRecords"))
	{
		cout << " - record id : " << vt.second.get_child("chunk_id").data() << endl;
		if(vt.first == "BlockRecord")
		{
			amountOfBlockRecords++;
			BOOST_FOREACH(ptree::value_type &vt2 , vt.second)
			{
				if(vt2.first == "chunk_data")
				{
					cout << " - " << vt2.first << " : "; 
					std::string attributName="";
					std::string prevattributName="";
					BOOST_FOREACH(ptree::value_type &vt3, vt2.second)
					{
						if(vt3.first == "chunk_record") {
							attributName = vt3.second.get_child("chunk_ddid").data();
							if(prevattributName!=attributName) {
								cout << endl;
								cout << " -- chunk_record : " << vt3.second.get_child("chunk_ddid").data() << " "; 
								prevattributName=attributName;
							}
							else
								cout << ".";
							amountOfchunk_records++;
							if(amountOfchunk_records==1) {
								hexdata_attribut1 = vt3.second.get_child("Data").data();
							}
							if(amountOfchunk_records==2) {
								hexdata_attribut2 = vt3.second.get_child("Data").data();
							}
						}
					}
				}
			}
		}
	}

	BOOST_CHECK(amountOfBlockRecords == 1); // Only one BlockRecord - the attributs should be added to BlockRecord until it is full, then new BlockRecord will be added
	cout << endl << "- amount of chunk records : " << amountOfchunk_records << endl;
	BOOST_CHECK(amountOfchunk_records == 37); 
	cout << "________________________________________" << endl;


	listResult = pDDC->ls();	
	//expected : reads like this: <GUID> has a profile folder with attribut name, mobil and foto
	std::string expected1 =  "F4C23762ED2823A27E62A64B95C024EF./profile/name";
	std::string expected2 =  "F4C23762ED2823A27E62A64B95C024EF./profile/mobil";
	std::string expected3 =  "F4C23762ED2823A27E62A64B95C024EF./profile/foto";

	BOOST_CHECK(listResult.size() > 0);

	int c=0;
	BOOST_FOREACH(std::string attribut, listResult)
	{
		c++;
		cout << "- OK attribut : " << attribut << endl;
		if(c==1) BOOST_CHECK(expected1 == attribut);
		if(c==2) BOOST_CHECK(expected2 == attribut);
		if(c==3) BOOST_CHECK(expected3 == attribut);
	}


	cout << "________________________________________" << endl;

	// Clean up section - must be in bottom
	BOOST_FOREACH(std::string filename, listBFiFiles)
	{
		cout << "- OK Cleanup file : " << filename << endl;
		boost::filesystem::path p = boost::filesystem::path(filename);
		boost::filesystem::remove(filename);
	}

	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(handle_cmd_line)
{
	cout<<"BOOST_AUTO_TEST(handle_cmd_line)\n{"<<endl;

	using boost::optional;
	using boost::property_tree::ptree;

	// create BFi files
	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
	ptree ptListOfBlockRecords;

	// attribut 1
	std::string attributName = "name";
	std::string name = "Johnny Serup";	
	std::vector<unsigned char> attributValue(name.begin(), name.end());

	// attribut 2
	std::string attributName2 = "mobil";
	std::string mobil = "555 - 332 211 900";	
	std::vector<unsigned char> attributValue2(mobil.begin(), mobil.end());

	// attribut 3 - large
	std::string attributName3= "foto"; // it should be ddid -- datadictionary id which refers to attribut description
	std::vector<unsigned char> attributValue3;
	std::string fn = "testImage.png"; // should be of size 10.5 Kb
	std::ifstream is (fn, ios::binary);
	if (is)
	{
		long length = boost::filesystem::file_size(fn);
		std::cout << "[readFile] Reading file: " << fn << " ; amount " << length << " characters... \n";
		// Make sure receipient has room
		attributValue3.resize(length,0);
		//read content of infile
		is.read ((char*)&attributValue3[0],length);
		std::cout << "[readFile] size: " << (int) attributValue3.size() << '\n';
		std::cout << "[readFile] capacity: " << (int) attributValue3.capacity() << '\n';
		std::cout << "[readFile] max_size: " << (int) attributValue3.max_size() << '\n';
		is.close();
	}
	BOOST_CHECK(attributValue3.size() > 0);

	std::string realmName = "profile";
	long maxBlockRecordSize=27000;	


	cout << "BlockRecord size before: " << maxBlockRecordSize << endl;
	std::string transGuid = "F4C23762ED2823A27E62A64B95C024EF";
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName, attributValue)); 
	cout << "BlockRecord size after 1 attribut add : " << maxBlockRecordSize << endl;
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName2, attributValue2)); 
	cout << "BlockRecord size after 2 atrribut add : " << maxBlockRecordSize << endl;
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName3, attributValue3)); 
	cout << "BlockRecord size after 3 atrribut add : " << maxBlockRecordSize << endl;
	 
	long maxBlockEntitySize=37000; // should result in 1 BlockEntity 
	boost::property_tree::ptree ptBlockEntity = ptestDataDictionaryControl->addBlockRecordToBlockEntity(transGuid, ptListOfBlockRecords, maxBlockEntitySize);
	BOOST_CHECK(ptBlockEntity.size()>0);

	cout << "*{{{" << endl;
	write_xml(std::cout, ptBlockEntity, boost::property_tree::xml_writer_make_settings<std::string>('\t', 1) );
	cout << "*}}}" << endl;

	std::vector< pair<std::string ,int> > listOfBlockEntityFiles = ptestDataDictionaryControl->writeBlockEntityToBFiFile(ptBlockEntity);
	BOOST_CHECK(listOfBlockEntityFiles.size()==1);

	pair <std::string,int> block;
	std::list<std::string> listBFiFiles;
	BOOST_FOREACH(block, listOfBlockEntityFiles)
	{
		cout << "- OK Created file : " << block.first << " size : " << block.second << endl;
		listBFiFiles.push_back(block.first);
	}


	// check that list now contain basic 'listOfBlockRecords' - which is necessary	
	optional< ptree& > child = ptListOfBlockRecords.get_child_optional( "listOfBlockRecords" );
	BOOST_CHECK(child);

	// verify that BlockRecord has been added
	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data" );
	BOOST_CHECK(child);

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record" );
	BOOST_CHECK(child);

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record.chunk_ddid" );
	BOOST_CHECK(child);

	cout << "________________________________________" << endl;
	cout << "attributs added : " << endl;
		
	int amountOfBlockRecords = 0;
	int amountOfchunk_records = 0;
	std::string hexdata_attribut1="";
	std::string hexdata_attribut2="";


	BOOST_FOREACH(ptree::value_type &vt, ptListOfBlockRecords.get_child("listOfBlockRecords"))
	{
		cout << " - record id : " << vt.second.get_child("chunk_id").data() << endl;
		if(vt.first == "BlockRecord")
		{
			amountOfBlockRecords++;
			BOOST_FOREACH(ptree::value_type &vt2 , vt.second)
			{
				if(vt2.first == "chunk_data")
				{
					cout << " - " << vt2.first << " : "; 
					std::string attributName="";
					std::string prevattributName="";
					BOOST_FOREACH(ptree::value_type &vt3, vt2.second)
					{
						if(vt3.first == "chunk_record") {
							attributName = vt3.second.get_child("chunk_ddid").data();
							if(prevattributName!=attributName) {
								cout << endl;
								cout << " -- chunk_record : " << vt3.second.get_child("chunk_ddid").data() << " "; 
								prevattributName=attributName;
							}
							else
								cout << ".";
							amountOfchunk_records++;
							if(amountOfchunk_records==1) {
								hexdata_attribut1 = vt3.second.get_child("Data").data();
							}
							if(amountOfchunk_records==2) {
								hexdata_attribut2 = vt3.second.get_child("Data").data();
							}
						}
					}
				}
			}
		}
	}

	BOOST_CHECK(amountOfBlockRecords == 1); // Only one BlockRecord - the attributs should be added to BlockRecord until it is full, then new BlockRecord will be added
	cout << endl << "- amount of chunk records : " << amountOfchunk_records << endl;
	BOOST_CHECK(amountOfchunk_records == 37); 
	cout << "________________________________________" << endl;


	CUtils utils;
	std::string cmd = "ls";
	std::list<std::string> listResult = utils.handle_cmdline_input(cmd); 

	std::string expected1 =  "F4C23762ED2823A27E62A64B95C024EF./profile/name";
	std::string expected2 =  "F4C23762ED2823A27E62A64B95C024EF./profile/mobil";
	std::string expected3 =  "F4C23762ED2823A27E62A64B95C024EF./profile/foto";

	BOOST_CHECK(listResult.size() > 0);

	int c=0;
	BOOST_FOREACH(std::string attribut, listResult)
	{
		c++;
		cout << "- OK attribut : " << attribut << endl;
		if(c==1) BOOST_CHECK(expected1 == attribut);
		if(c==2) BOOST_CHECK(expected2 == attribut);
		if(c==3) BOOST_CHECK(expected3 == attribut);
	}

	cout << "________________________________________" << endl;

	// Clean up section - must be in bottom
	BOOST_FOREACH(std::string filename, listBFiFiles)
	{
		cout << "- OK Cleanup file : " << filename << endl;
		boost::filesystem::path p = boost::filesystem::path(filename);
		boost::filesystem::remove(filename);
	}

cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(addLargeAttributOver2BFifiles)
{
	cout<<"BOOST_AUTO_TEST(addLargeAttributOver2BFifiles)\n{"<<endl;

	using boost::optional;
	using boost::property_tree::ptree;

	CDataDictionaryControl *pDDC = new CDataDictionaryControl();
	std::list<std::string> listResult = pDDC->ls();	
	BOOST_FOREACH(std::string attribut, listResult)
	{
		cout << "- OK attribut : " << attribut << endl;
	}
	BOOST_CHECK(listResult.size() <= 0);

	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
	ptree ptListOfBlockRecords;

	// attribut 1
	std::string attributName = "name";
	std::string name = "Johnny Serup";	
	std::vector<unsigned char> attributValue(name.begin(), name.end());

	// attribut 2
	std::string attributName2 = "mobil";
	std::string mobil = "555 - 332 211 900";	
	std::vector<unsigned char> attributValue2(mobil.begin(), mobil.end());

	// attribut 3 - large
	std::string attributName3= "foto"; // it should be ddid -- datadictionary id which refers to attribut description
	std::vector<unsigned char> attributValue3;
	std::string fn = "testImage.png"; // should be of size 10.5 Kb
	std::ifstream is (fn, ios::binary);
	if (is)
	{
		long length = boost::filesystem::file_size(fn);
		std::cout << "[readFile] Reading file: " << fn << " ; amount " << length << " characters... \n";
		// Make sure receipient has room
		attributValue3.resize(length,0);
		//read content of infile
		is.read ((char*)&attributValue3[0],length);
		std::cout << "[readFile] size: " << (int) attributValue3.size() << '\n';
		std::cout << "[readFile] capacity: " << (int) attributValue3.capacity() << '\n';
		std::cout << "[readFile] max_size: " << (int) attributValue3.max_size() << '\n';
		is.close();
	}
	BOOST_CHECK(attributValue3.size() > 0);

	std::string realmName = "profile";
	long maxBlockRecordSize=10456; // should result in multiple BlockRecords inside a BlockEntity	


	cout << "BlockRecord size before: " << maxBlockRecordSize << endl;
	std::string transGuid = "F8C23762ED2823A27E62A64B95C024EF";
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName, attributValue)); 
	cout << "BlockRecord size after 1 attribut add : " << maxBlockRecordSize << endl;
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName2, attributValue2)); 
	cout << "BlockRecord size after 2 atrribut add : " << maxBlockRecordSize << endl;
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName3, attributValue3)); 
	cout << "BlockRecord size after 3 atrribut add : " << maxBlockRecordSize << endl;
	 
	long maxBlockEntitySize=15000; // should result in 2 BlockEntity 
	boost::property_tree::ptree ptBlockEntity = ptestDataDictionaryControl->addBlockRecordToBlockEntity(transGuid, ptListOfBlockRecords, maxBlockEntitySize);
	BOOST_CHECK(ptBlockEntity.size()>0);

	cout << "XML output of ALL attributs - the foto attribut will be spanning over 2 .BFi files, however here is shown all attributs together: " << endl;
	cout << "*{{{" << endl;
	write_xml(std::cout, ptBlockEntity, boost::property_tree::xml_writer_make_settings<std::string>('\t', 1) );
	cout << "*}}}" << endl;

	// create BFi files
	std::vector< pair<std::string ,int> > listOfBlockEntityFiles = ptestDataDictionaryControl->writeBlockEntityToBFiFile(ptBlockEntity);
	cout << "Created : " << listOfBlockEntityFiles.size() << " .BFi files " << endl;
	BOOST_CHECK(listOfBlockEntityFiles.size()==2);

	pair <std::string,int> block;
	std::list<std::string> listBFiFiles;
	BOOST_FOREACH(block, listOfBlockEntityFiles)
	{
		cout << "- OK Created file : " << block.first << " size : " << block.second << endl;
		listBFiFiles.push_back(block.first);
	}


	// check that list now contain basic 'listOfBlockRecords' - which is necessary	
	optional< ptree& > child = ptListOfBlockRecords.get_child_optional( "listOfBlockRecords" );
	BOOST_CHECK(child);

	// verify that BlockRecord has been added
	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data" );
	BOOST_CHECK(child);

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record" );
	BOOST_CHECK(child);

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record.chunk_ddid" );
	BOOST_CHECK(child);

	cout << "________________________________________" << endl;
	cout << "attributs added : " << endl;
		
	int amountOfBlockRecords = 0;
	int amountOfchunk_records = 0;
	std::string hexdata_attribut1="";
	std::string hexdata_attribut2="";


	BOOST_FOREACH(ptree::value_type &vt, ptListOfBlockRecords.get_child("listOfBlockRecords"))
	{
		cout << " - record id : " << vt.second.get_child("chunk_id").data() << endl;
		if(vt.first == "BlockRecord")
		{
			amountOfBlockRecords++;
			BOOST_FOREACH(ptree::value_type &vt2 , vt.second)
			{
				if(vt2.first == "chunk_data")
				{
					cout << " - " << vt2.first << " : "; 
					std::string attributName="";
					std::string prevattributName="";
					BOOST_FOREACH(ptree::value_type &vt3, vt2.second)
					{
						if(vt3.first == "chunk_record") {
							attributName = vt3.second.get_child("chunk_ddid").data();
							if(prevattributName!=attributName) {
								cout << endl;
								cout << " -- chunk_record : " << vt3.second.get_child("chunk_ddid").data() << " "; 
								prevattributName=attributName;
							}
							else
								cout << ".";
							amountOfchunk_records++;
							if(amountOfchunk_records==1) {
								hexdata_attribut1 = vt3.second.get_child("Data").data();
							}
							if(amountOfchunk_records==2) {
								hexdata_attribut2 = vt3.second.get_child("Data").data();
							}
						}
					}
				}
			}
		}
		cout << endl;
	}

	cout << "________________________________________" << endl;
	if(amountOfBlockRecords==2)
		   	cout << "- OK amount of BlockRecords created: " << amountOfBlockRecords << endl;
	else
		   	cout << "- FAIL: amount of BlockRecords created: " << amountOfBlockRecords << endl;
	BOOST_CHECK(amountOfBlockRecords == 2); // Only one BlockRecord - the attributs should be added to BlockRecord until it is full, then new BlockRecord will be added
	if(amountOfchunk_records == 47)
		cout << "- OK amount of chunk records : " << amountOfchunk_records << endl;
	else
		cout << "- FAIL: amount of chunk records : " << amountOfchunk_records << endl;
	BOOST_CHECK(amountOfchunk_records == 47); 
	cout << "________________________________________" << endl;

	cout << "call ls() - list attributs - validate expected results " << endl;
	listResult = pDDC->ls();	
	//expected : reads like this: <GUID> has a profile folder with attribut name, mobil and foto
	std::string expected1 =  "F8C23762ED2823A27E62A64B95C024EF./profile/name";
	std::string expected2 =  "F8C23762ED2823A27E62A64B95C024EF./profile/mobil";
	std::string expected3 =  "F8C23762ED2823A27E62A64B95C024EF./profile/foto";
	std::string expected4 =  "F8C23762ED2823A27E62A64B95C024EF./profile/foto";

	BOOST_CHECK(listResult.size() > 0);

	int c=0;
	BOOST_FOREACH(std::string attribut, listResult)
	{
		c++;
		cout << "- OK attribut : " << attribut << endl;
		if(c==1) BOOST_CHECK(expected1 == attribut);
		if(c==2) BOOST_CHECK(expected2 == attribut);
		if(c==3) BOOST_CHECK(expected3 == attribut);
		if(c==4) BOOST_CHECK(expected4 == attribut);
	}

	cout << "TODO: ls should show attribut spanning over multiple .BFi files in a different way " << endl;

	cout << "________________________________________" << endl;

	// Clean up section - must be in bottom
	BOOST_FOREACH(std::string filename, listBFiFiles)
	{
		cout << "- OK Cleanup file : " << filename << endl;
		boost::filesystem::path p = boost::filesystem::path(filename);
		boost::filesystem::remove(filename);
	}

	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(addLargeAttributOver3BFifiles)
{
	cout<<"BOOST_AUTO_TEST(addLargeAttributOver3BFifiles)\n{"<<endl;

	using boost::optional;
	using boost::property_tree::ptree;

	CDataDictionaryControl *pDDC = new CDataDictionaryControl();
	std::list<std::string> listResult = pDDC->ls();	
	BOOST_FOREACH(std::string attribut, listResult)
	{
		cout << "- OK attribut : " << attribut << endl;
	}
	BOOST_CHECK(listResult.size() <= 0);

	CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
	ptree ptListOfBlockRecords;

	// attribut 1
	std::string attributName = "name";
	std::string name = "Johnny Serup";	
	std::vector<unsigned char> attributValue(name.begin(), name.end());

	// attribut 2
	std::string attributName2 = "mobil";
	std::string mobil = "555 - 332 211 900";	
	std::vector<unsigned char> attributValue2(mobil.begin(), mobil.end());

	// attribut 3 - large
	std::string attributName3= "foto"; // it should be ddid -- datadictionary id which refers to attribut description
	std::vector<unsigned char> attributValue3;
	std::string fn = "testImage.png"; // should be of size 10.5 Kb
	std::ifstream is (fn, ios::binary);
	if (is)
	{
		long length = boost::filesystem::file_size(fn);
		std::cout << "[readFile] Reading file: " << fn << " ; amount " << length << " characters... \n";
		// Make sure receipient has room
		attributValue3.resize(length,0);
		//read content of infile
		is.read ((char*)&attributValue3[0],length);
		std::cout << "[readFile] size: " << (int) attributValue3.size() << '\n';
		std::cout << "[readFile] capacity: " << (int) attributValue3.capacity() << '\n';
		std::cout << "[readFile] max_size: " << (int) attributValue3.max_size() << '\n';
		is.close();
	}
	BOOST_CHECK(attributValue3.size() > 0);

	std::string realmName = "profile";
	long maxBlockRecordSize=5456; // should result in multiple BlockRecords inside a BlockEntity	


	cout << "BlockRecord size before: " << maxBlockRecordSize << endl;
	std::string transGuid = "F9C23762ED2823A27E62A64B95C024EF";
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName, attributValue)); 
	cout << "BlockRecord size after 1 attribut add : " << maxBlockRecordSize << endl;
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName2, attributValue2)); 
	cout << "BlockRecord size after 2 atrribut add : " << maxBlockRecordSize << endl;
	BOOST_CHECK(ptestDataDictionaryControl->addAttributToBlockRecord(transGuid,ptListOfBlockRecords, maxBlockRecordSize, realmName, attributName3, attributValue3)); 
	cout << "BlockRecord size after 3 atrribut add : " << maxBlockRecordSize << endl;
	 
	long maxBlockEntitySize=15000; // should result in 2 BlockEntity 
	boost::property_tree::ptree ptBlockEntity = ptestDataDictionaryControl->addBlockRecordToBlockEntity(transGuid, ptListOfBlockRecords, maxBlockEntitySize);
	BOOST_CHECK(ptBlockEntity.size()>0);

	cout << "XML output of ALL attributs - the foto attribut will be spanning over 3 .BFi files, however here is shown all attributs together: " << endl;
	cout << "*{{{" << endl;
	write_xml(std::cout, ptBlockEntity, boost::property_tree::xml_writer_make_settings<std::string>('\t', 1) );
	cout << "*}}}" << endl;

	// create BFi files
	std::vector< pair<std::string ,int> > listOfBlockEntityFiles = ptestDataDictionaryControl->writeBlockEntityToBFiFile(ptBlockEntity);
	cout << "Created : " << listOfBlockEntityFiles.size() << " .BFi files " << endl;
	BOOST_CHECK(listOfBlockEntityFiles.size()==3);

	pair <std::string,int> block;
	std::list<std::string> listBFiFiles;
	BOOST_FOREACH(block, listOfBlockEntityFiles)
	{
		cout << "- OK Created file : " << block.first << " size : " << block.second << endl;
		listBFiFiles.push_back(block.first);
	}


	// check that list now contain basic 'listOfBlockRecords' - which is necessary	
	optional< ptree& > child = ptListOfBlockRecords.get_child_optional( "listOfBlockRecords" );
	BOOST_CHECK(child);

	// verify that BlockRecord has been added
	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data" );
	BOOST_CHECK(child);

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record" );
	BOOST_CHECK(child);

	child = ptListOfBlockRecords.get_child_optional( "BlockRecord.chunk_data.chunk_record.chunk_ddid" );
	BOOST_CHECK(child);

	cout << "________________________________________" << endl;
	cout << "attributs added : " << endl;
		
	int amountOfBlockRecords = 0;
	int amountOfchunk_records = 0;
	std::string hexdata_attribut1="";
	std::string hexdata_attribut2="";


	BOOST_FOREACH(ptree::value_type &vt, ptListOfBlockRecords.get_child("listOfBlockRecords"))
	{
		cout << " - record id : " << vt.second.get_child("chunk_id").data() << endl;
		if(vt.first == "BlockRecord")
		{
			amountOfBlockRecords++;
			BOOST_FOREACH(ptree::value_type &vt2 , vt.second)
			{
				if(vt2.first == "chunk_data")
				{
					cout << " - " << vt2.first << " : "; 
					std::string attributName="";
					std::string prevattributName="";
					BOOST_FOREACH(ptree::value_type &vt3, vt2.second)
					{
						if(vt3.first == "chunk_record") {
							attributName = vt3.second.get_child("chunk_ddid").data();
							if(prevattributName!=attributName) {
								cout << endl;
								cout << " -- chunk_record : " << vt3.second.get_child("chunk_ddid").data() << " "; 
								prevattributName=attributName;
							}
							else
								cout << ".";
							amountOfchunk_records++;
							if(amountOfchunk_records==1) {
								hexdata_attribut1 = vt3.second.get_child("Data").data();
							}
							if(amountOfchunk_records==2) {
								hexdata_attribut2 = vt3.second.get_child("Data").data();
							}
						}
					}
				}
			}
		}
		cout << endl;
	}

	cout << "________________________________________" << endl;
	if(amountOfBlockRecords==3)
		   	cout << "- OK amount of BlockRecords created: " << amountOfBlockRecords << endl;
	else
		   	cout << "- FAIL: amount of BlockRecords created: " << amountOfBlockRecords << endl;
	BOOST_CHECK(amountOfBlockRecords == 3); // Only one BlockRecord - the attributs should be added to BlockRecord until it is full, then new BlockRecord will be added
	if(amountOfchunk_records == 68)
		cout << "- OK amount of chunk records : " << amountOfchunk_records << endl;
	else
		cout << "- FAIL: amount of chunk records : " << amountOfchunk_records << endl;
	BOOST_CHECK(amountOfchunk_records == 68); 
	cout << "________________________________________" << endl;

	cout << "call ls() - list attributs - validate expected results " << endl;
	listResult = pDDC->ls();	
	//expected : reads like this: <GUID> has a profile folder with attribut name, mobil and foto
	std::string expected1 =  "F9C23762ED2823A27E62A64B95C024EF./profile/foto";
	std::string expected2 =  "F9C23762ED2823A27E62A64B95C024EF./profile/name";
	std::string expected3 =  "F9C23762ED2823A27E62A64B95C024EF./profile/mobil";

	BOOST_CHECK(listResult.size() > 0);

	int c=0;
	BOOST_FOREACH(std::string attribut, listResult)
	{
		c++;
		cout << "- OK attribut : " << attribut << endl;
		if(c==1) BOOST_CHECK(expected1 == attribut);
		if(c==2) BOOST_CHECK(expected2 == attribut);
		if(c==3) BOOST_CHECK(expected3 == attribut);
	}

	cout << "TODO: ls should show attribut spanning over multiple .BFi files in a different way " << endl;

	cout << "________________________________________" << endl;

	// Clean up section - must be in bottom
	BOOST_FOREACH(std::string filename, listBFiFiles)
	{
		cout << "- OK Cleanup file : " << filename << endl;
		boost::filesystem::path p = boost::filesystem::path(filename);
		boost::filesystem::remove(filename);
	}

	cout<<"}"<<endl;
}
