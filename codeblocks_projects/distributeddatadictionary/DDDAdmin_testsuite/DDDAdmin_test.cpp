
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
    CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
    BOOST_CHECK(ptestDataDictionaryControl != 0);
    delete ptestDataDictionaryControl;
}

BOOST_AUTO_TEST_CASE(create_BFi_blockfile)
{
    CDataDictionaryControl *ptestDataDictionaryControl = new CDataDictionaryControl();
    bool bFileCreated = ptestDataDictionaryControl->CreateBlockFile("test.BFi");

    BOOST_CHECK(bFileCreated == true);

    //Cleanup
    boost::filesystem::wpath file(L"test.BFi");
    if(boost::filesystem::exists(L"test.BFi"))
         boost::filesystem::remove(file);

}

BOOST_AUTO_TEST_CASE(create3Blockfiles)
{
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

}


BOOST_AUTO_TEST_CASE(writeBlockIntoBFiStructure)
{
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
	}
	else{
		std::string filename(fn);
		throw std::runtime_error("Error: Could NOT read file: " + filename );
	}

	if(vp.size()>0) {
		amountOfBlocks = vp.size();
		pair <char*,int> block;
		BlockRecord ans;
		BOOST_FOREACH( block, vp )
		{
			boost::property_tree::ptree pt = ptestDataDictionaryControl->createBFiBlockRecord(testfilename.c_str(),block.first, block.second);

			BlockRecordEntry f;
			f.TransGUID = pt.get<std::string>("BlockRecord.TransGUID");
			f.chunk_id  = pt.get<std::string>("BlockRecord.chunk_id");
			f.aiid 		= pt.get<unsigned long>("BlockRecord.aiid");
			f.chunk_seq = pt.get<unsigned long>("BlockRecord.chunk_seq");
			f.chunk_data_in_hex = pt.get<std::string>("BlockRecord.chunk_data_in_hex");
			f.chunk_size = pt.get<unsigned long>("BlockRecord.chunk_size");
			f.chunk_md5 = pt.get<std::string>("BlockRecord.chunk_md5");

			// check if data is in structure, and of correct size
			BOOST_CHECK(f.chunk_size == f.chunk_data_in_hex.size());
		 	// check if data has correct md5 sum
			std::string strMD5(CMD5((const char*)f.chunk_data_in_hex.c_str()).GetMD5s());
			BOOST_CHECK(f.chunk_md5 == strMD5);
			// check if chunk_id is correct - it should be name of realm file, inorder to follow specs
			BOOST_CHECK(f.chunk_id == testfilename);
			
		
		}
	}
	
    //Cleanup
    boost::filesystem::remove(testfilename);
	
}
