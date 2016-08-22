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
	//boost::filesystem::path full_path( boost::filesystem::current_path() );
	//std::string strFilepath = full_path.string() + "/" + filename;
	std::ofstream blockfile;
//	if(!boost::filesystem::exists(strFilepath))
	if(!boost::filesystem::exists(filename))
	{
//        blockfile.open(strFilepath.c_str());
        blockfile.open(filename.c_str());
		bResult = blockfile.is_open();
        //assert( blockfile.is_open() );
	}
	return bResult;
}

