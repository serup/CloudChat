#ifndef CDATADICTIONARYCONTROL_H
#define CDATADICTIONARYCONTROL_H

#include <string>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <stdexcept>

using namespace std;

class CDataDictionaryControl
{
    public:
        /** Default constructor */
        CDataDictionaryControl();
        /** Default destructor */
        virtual ~CDataDictionaryControl();

	bool CreateBlockFile(std::string filename);
	int splitFileIntoBlocks(std::string filename);
	
    protected:
	
    private:
	vector< pair<char*, int> > readFile(const char* fn);
	ifstream::pos_type fileS(const char* fn);

};

#endif // CDATADICTIONARYCONTROL_H
