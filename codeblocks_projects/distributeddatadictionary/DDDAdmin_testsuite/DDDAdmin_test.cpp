
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

BOOST_GLOBAL_FIXTURE(ReportRedirector)
#endif
#endif


BOOST_AUTO_TEST_SUITE ( datadictionarycontrol ) // name of the test suite
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(datadictionarycontrol_instantiated)
{
	cout<<"BOOS_AUTO_TEST(datadictionarycontrol_instantiated)\n{"<<endl;
    CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
    BOOST_CHECK(ptestDataDictionaryControl != 0);
    delete ptestDataDictionaryControl;
	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(create_BFi_blockfile)
{
	cout<<"BOOS_AUTO_TEST(create_BFi_blockfile)\n{"<<endl;

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
	cout<<"BOOS_AUTO_TEST(create3Blockfiles)\n{"<<endl;
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
	cout<<"BOOS_AUTO_TEST(writeBlockIntoBFiStructure)\n{"<<endl;
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

		}
	}

	//Cleanup
	boost::filesystem::remove(testfilename);

	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(splitAttributIntoDEDchunks)
{
	cout<<"BOOS_AUTO_TEST(splitAttributIntoDEDchunks)\n{"<<endl;

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
	std::vector< pair<unsigned char*,int> > listOfDEDchunks = ptestDataDictionaryControl->splitAttributIntoDEDchunks(0, attributName, FileDataBytesInVector, maxDEDchunkSize);

	std::cout << "listOfDEDchunks : " << listOfDEDchunks.size() << '\n';
	// verify that image is in DED blocks
	BOOST_CHECK(listOfDEDchunks.size() == 35);
	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(addChunkDataToBlockRecord)
{
	cout<<"BOOS_AUTO_TEST(addChunkDataToBlockRecord)\n{"<<endl;

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

	std::vector< pair<unsigned char*,int> > listOfDEDchunks = ptestDataDictionaryControl->splitAttributIntoDEDchunks(0, attributName, FileDataBytesInVector, maxDEDchunkSize);
	std::cout << "listOfDEDchunks : " << listOfDEDchunks.size() << '\n';
	BOOST_CHECK(listOfDEDchunks.size() == 35);


	long aiid=0;
	std::string realmName = "profile";
	boost::property_tree::ptree pt = ptestDataDictionaryControl->addDEDchunksToBlockRecords(aiid, realmName, attributName, listOfDEDchunks, maxBlockRecordSize);

	BOOST_CHECK(pt.size() > 0);
	
	cout<<"}"<<endl;
}

BOOST_AUTO_TEST_CASE(addBlockRecordToBlockEntity)
{
	cout<<"BOOS_AUTO_TEST(addBlockRecordToBlockEntity)\n{"<<endl;

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

	std::vector< pair<unsigned char*,int> > listOfDEDchunks = ptestDataDictionaryControl->splitAttributIntoDEDchunks(0, attributName, FileDataBytesInVector, maxDEDchunkSize);
	std::cout << "listOfDEDchunks : " << listOfDEDchunks.size() << '\n';
	BOOST_CHECK(listOfDEDchunks.size() == 35);


	long aiid=0;
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
	cout<<"BOOS_AUTO_TEST(writeBlockEntitiesToBFiFiles)\n{"<<endl;

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

	std::vector< pair<unsigned char*,int> > listOfDEDchunks = ptestDataDictionaryControl->splitAttributIntoDEDchunks(0, attributName, FileDataBytesInVector, maxDEDchunkSize);
	std::cout << "listOfDEDchunks : " << listOfDEDchunks.size() << '\n';
	BOOST_CHECK(listOfDEDchunks.size() == 35);


	long aiid=0;
	std::string realmName = "profile";
	boost::property_tree::ptree ptListOfBlockRecords = ptestDataDictionaryControl->addDEDchunksToBlockRecords(aiid, realmName, attributName, listOfDEDchunks, maxBlockRecordSize);

	BOOST_CHECK(ptListOfBlockRecords.size() > 0);

	long maxBlockEntitySize=27000; // should result in 2 BlockEntity 
	std::string transGuid = "E4C23762ED2823A27E62A64B95C024E7";
	boost::property_tree::ptree ptBlockEntity = ptestDataDictionaryControl->addBlockRecordToBlockEntity(transGuid, ptListOfBlockRecords, maxBlockEntitySize);

	BOOST_CHECK(ptBlockEntity.size()>0);

	std::vector< pair<std::string ,int> > listOfBlockEntityFiles = ptestDataDictionaryControl->writeBlockEntityToBFiFile(ptBlockEntity);

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
	ptBlockEntity = ptestDataDictionaryControl->addBlockRecordToBlockEntity(transGuid, ptListOfBlockRecords, maxBlockEntitySize);

	BOOST_CHECK(ptBlockEntity.size()>0);

	listOfBlockEntityFiles = ptestDataDictionaryControl->writeBlockEntityToBFiFile(ptBlockEntity);

	BOOST_CHECK(listOfBlockEntityFiles.size()==1);

	BOOST_FOREACH(block, listOfBlockEntityFiles)
	{
			cout << "- OK Created file : " << block.first << " size : " << block.second << endl;
			cout << "cleanup file " << endl;
			boost::filesystem::remove(block.first);
	}

	cout<<"}"<<endl;
}

