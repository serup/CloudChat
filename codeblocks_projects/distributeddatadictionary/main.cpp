#include "datadictionarycontrol.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <tclap/CmdLine.h>
#include <curses.h>
#include <unistd.h>

using namespace TCLAP;
using namespace std;


void spinner(int spin_seconds, int y, int x) 
{
	static char const spin_chars[] = "/-\\|";
	unsigned long i, num_iterations = (spin_seconds * 10);
	for (i=0; i<num_iterations; ++i) {
		mvaddch(y, x, spin_chars[i & 3]);
		refresh();
		usleep(100000);
	}
}

/**
 *
 * MAIN
 *
 */
int main(int argc, char* argv[])
{

	if ( (argc <= 1) || (argv[argc-1] == NULL) ) {  // there is NO input...
		cerr << "No argument provided - Try -h for detailed info " << endl;
	}

	// Wrap everything in a try block.  Do this every time, 
	// because exceptions will be thrown for problems. 
	try {  

		// Define the command line object.
		CmdLine cmd("Command description message", ' ', "0.9");

		// Define a switch and add it to the command line.
		TCLAP::SwitchArg filesystemSwitch("f","filesystem","Starts filesystem command line", cmd, false);
	
		// Parse the args.
		cmd.parse( argc, argv );
		bool startFs = filesystemSwitch.getValue();
		if( startFs ) {

			char incmd[80];
			
   	initscr();				/* start the curses mode */
	mvprintw(0 ,0, "%s","                                                                     " );
	mvprintw(1 ,0, "%s","   DDDDDDDDDDDDD        DDDDDDDDDDDDD         DDDDDDDDDDDDD          " );
	mvprintw(2 ,0, "%s","   D::::::::::::DDD     D::::::::::::DDD      D::::::::::::DDD       " );
	mvprintw(3 ,0, "%s","   D:::::::::::::::DD   D:::::::::::::::DD    D:::::::::::::::DD     " );
	mvprintw(4 ,0, "%s","   DDD:::::DDDDD:::::D  DDD:::::DDDDD:::::D   DDD:::::DDDDD:::::D    " );
	mvprintw(5 ,0, "%s","     D:::::D    D:::::D   D:::::D    D:::::D    D:::::D    D:::::D   " );
	mvprintw(6 ,0, "%s","     D:::::D     D:::::D  D:::::D     D:::::D   D:::::D     D:::::D  " );
	mvprintw(7 ,0, "%s","     D:::::D     D:::::D  D:::::D     D:::::D   D:::::D     D:::::D  " );
	mvprintw(8 ,0, "%s","     D:::::D     D:::::D  D:::::D     D:::::D   D:::::D     D:::::D  " );
	mvprintw(9 ,0, "%s","     D:::::D     D:::::D  D:::::D     D:::::D   D:::::D     D:::::D  " );
	mvprintw(10,0, "%s","     D:::::D     D:::::D  D:::::D     D:::::D   D:::::D     D:::::D  " );
	mvprintw(11,0, "%s","     D:::::D     D:::::D  D:::::D     D:::::D   D:::::D     D:::::D  " );
	mvprintw(12,0, "%s","     D:::::D    D:::::D   D:::::D    D:::::D    D:::::D    D:::::D   " );
	mvprintw(13,0, "%s","   DDD:::::DDDDD:::::D  DDD:::::DDDDD:::::D   DDD:::::DDDDD:::::D    " );
	mvprintw(14,0, "%s","   D:::::::::::::::DD   D:::::::::::::::DD    D:::::::::::::::DD     " );
	mvprintw(15,0, "%s","   D::::::::::::DDD     D::::::::::::DDD      D::::::::::::DDD       " );
	mvprintw(16,0, "%s","   DDDDDDDDDDDDD        DDDDDDDDDDDDD         DDDDDDDDDDDDD          " );
	mvprintw(17,0, "%s","   Distributed Data Dictionary" );
	mvprintw(18,0, "%s","   Copyright @ Johnny Serup" );
	mvprintw(19,0, "%s","" );
	mvprintw(20,0, "%s","" );
	refresh();

			cout << endl;
	
			spinner(3, 20, 1); // spin

			cout << "\r" << "FS> " << std::flush;
	                
			getstr(incmd);
	endwin();  // cleanup curses
		}

	} catch (ArgException &e)  // catch any exceptions
	{ cerr << "error: " << e.error() << " for arg " << e.argId() << endl; }


	return 0;
}
