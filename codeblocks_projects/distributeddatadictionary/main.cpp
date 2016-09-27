#include "datadictionarycontrol.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <tclap/CmdLine.h>

using namespace TCLAP;
using namespace std;

/**
 *
 * MAIN
 *
 */
int main(int argc, char* argv[])
{
	try
	{
		system("echo -n '1. Current Directory is '; pwd");
		system("cat DOPS_outline.txt ");

		// Check command line arguments.
		if (argc != 2)
		{
			std::cerr << "Usage:  DDDAdmin <command>\n";
			return 1;
		}

//		std::cout << "command: " << argv[1] << std::endl;

		// Initialise 
		CDataDictionaryControl *pDDDControl = new CDataDictionaryControl();
	
		// Run the until stopped by user.
		

	}
	catch (std::exception& e)
	{
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}

