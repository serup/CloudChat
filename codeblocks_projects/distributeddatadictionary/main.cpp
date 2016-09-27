#include "datadictionarycontrol.hpp"

/**
 *
 * MAIN
 *
 */
int main(int argc, char* argv[])
{
	try
	{
		// Check command line arguments.
		if (argc != 3)
		{
			std::cerr << "Usage:  DDDAdmin <command>\n";
			return 1;
		}

		std::cout << "command: " << argv[1] << std::endl;

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

