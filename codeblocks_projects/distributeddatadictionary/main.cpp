#include "datadictionarycontrol.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <tclap/CmdLine.h>
#include <curses.h>
#include <unistd.h>
#include <signal.h>
#include <boost/regex.hpp>
#include <iostream>
#include <string>

static void finish(int sig);

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

void init_curses()
{
	(void) signal(SIGINT, finish);      /* arrange interrupts to terminate */

	(void) initscr();      /* initialize the curses library */
	keypad(stdscr, TRUE);  /* enable keyboard mapping */
	(void) nonl();         /* tell curses not to do NL->CR/NL on output */
	(void) cbreak();       /* take input chars one at a time, no wait for \n */
	(void) echo();         /* echo input - in color */

	if (has_colors())
	{
		start_color();

		/*
		 * Simple color assignment, often all we need.  Color pair 0 cannot
		 * be redefined.  This example uses the same value for the color
		 * pair as for the foreground color, though of course that is not
		 * necessary:
		 */
		init_pair(1, COLOR_RED,     COLOR_BLACK);
		init_pair(2, COLOR_GREEN,   COLOR_BLACK);
		init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
		init_pair(4, COLOR_BLUE,    COLOR_BLACK);
		init_pair(5, COLOR_CYAN,    COLOR_BLACK);
		init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(7, COLOR_WHITE,   COLOR_BLACK);
	}
}

void show_logo()
{
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
}

static void finish(int sig)
{
	endwin();

	/* do your non-curses wrapup here */

	exit(0);
}

void handle_cmd_input()
{
	char incmd[80];
	boost::regex pat( "^Subject: (Re: |Aw: )*(.*)" );

	mvprintw(20,0, "%s", "FS> ");

	std::string input="";
	while (input!="quit")
	{
		getstr(incmd);
		std::string tmp(incmd);
		input=tmp;

		std::string line(input);
		mvprintw(20,0, "%s", " ");
		clrtoeol();
		mvprintw(20,0, "%s", "FS> ");

		boost::smatch matches;
		if (boost::regex_match(line, matches, pat))
		{
			spinner(1, 20, 2); // spin
			std::string tmp(matches[2]);
			mvprintw(25,0, "%s %s", "result ",tmp.c_str());
			clrtoeol();
			mvprintw(20,0, "%s", " ");
			clrtoeol();
			mvprintw(20,0, "%s", "FS> ");
		}
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


			init_curses();			
			show_logo();

			spinner(3, 20, 2); // spin
			handle_cmd_input();
		}

	} catch (ArgException &e)  // catch any exceptions
	{ cerr << "error: " << e.error() << " for arg " << e.argId() << endl; }

	finish(0);
}


