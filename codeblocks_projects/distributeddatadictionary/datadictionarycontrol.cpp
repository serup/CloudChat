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
	vector< pair<char*, int> > vp = readFile(filename.c_str());
	if(vp.size()>0) {
		amountOfBlocks = vp.size();
		int filenumber=0;
		pair <char*,int> block;
		BOOST_FOREACH( block, vp )
		{
			filenumber++;
			std::string blockfilename = filename + "_" + std::to_string(filenumber) + ".BFi";
		    ofstream blockFile (blockfilename.c_str(), ios::out | ios::binary);
		    blockFile.write(block.first, block.second); // buffer, size
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

