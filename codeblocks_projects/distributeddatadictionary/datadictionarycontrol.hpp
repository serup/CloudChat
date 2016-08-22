#ifndef CDATADICTIONARYCONTROL_H
#define CDATADICTIONARYCONTROL_H

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class CDataDictionaryControl
{
    public:
        /** Default constructor */
        CDataDictionaryControl();
        /** Default destructor */
        virtual ~CDataDictionaryControl();

	bool CreateBlockFile(std::string filename);

    protected:

    private:
};

#endif // CDATADICTIONARYCONTROL_H
