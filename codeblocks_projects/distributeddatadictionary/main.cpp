#include "datadictionarycontrol.hpp"
#include "md5.h"                                  

#include <stdio.h>
#include <stdlib.h>
#include <tclap/CmdLine.h>
#include <curses.h>
#include <unistd.h>
#include <signal.h>
#include <boost/regex.hpp>
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>                
#include <boost/filesystem.hpp>                      
#include <boost/filesystem/operations.hpp>           
#include <iostream>                                  
#include <fstream>      // std::ifstream             
#include "utils.hpp"

static void finish(int sig);

using namespace TCLAP;
using namespace std;

#define yprompt 20
#define xprompt 0
#define promptdisplacement 4
#define yinfoposStart 27
#define xinfoposStart 0
#define yresultpos yprompt +1 
#define xresultpos xprompt +5 
#define errInfoWaitSeconds 8
#define spinnerpos 2

void spinner(int spin_seconds, int y, int x) 
{
	static char const spin_chars[] = "/-\\|";
	unsigned long i, num_iterations = (spin_seconds * 10);
	mvprintw(y,1, "%s", "[ ]");

	curs_set(0);
	for (i=0; i<num_iterations; ++i) {
		mvaddch(y, x, spin_chars[i & 3]);
		refresh();
		usleep(100000);
	}
	curs_set(1);
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
	clear();
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

void perform_cmd_action(int actionId)
{
	spinner(actionId, yprompt+1, spinnerpos); //TODO: spin should run in its own thread for the duration of this action, or stepwise turn spinner
	//TODO: case/swith action commands
}

void show_cmd_result(boost::smatch matches)
{
	std::string tmp(matches[2]);
	mvprintw(yresultpos,0, "%s %s", "result ",tmp.c_str());
	clrtoeol();
	mvprintw(yprompt,xprompt, "%s", " ");
	clrtoeol();
	mvprintw(yprompt,xprompt, "%s", "FS> ");
	refresh();
}

void show_prompt()
{
	mvprintw(yprompt,xprompt, "%s", "FS> ");
}


void reset_cmd_prompt()
{
	show_logo();
	mvprintw(yprompt,xprompt, "%s", " ");
	clrtoeol();
	show_prompt();
}

void show_cmd_ls_result(std::list<std::string> listResult)
{
	// example of output : F4C23762ED2823A27E62A64B95C024EF./profile/mobil     

	int c=0;                                                                        
	BOOST_FOREACH(std::string attribut, listResult)                                 
	{                                                                               
		mvprintw(yresultpos+c,xresultpos, "%s", attribut.c_str());
		c++;                                                                    
	}                                                                               

	if(c<=0) {
		mvprintw(yprompt+1,0, "%s", "------ No Data Dictionary files found, hence nothing to show ");
		refresh();
		spinner(errInfoWaitSeconds, yprompt+1, spinnerpos); // spin
		reset_cmd_prompt();
	}
	else
		show_prompt();
}

void show_cmd_options()
{
//	 info place holder
    int ypos = yinfoposStart; 
	mvprintw(ypos++,0, "%s", "commands: ");
	mvprintw(ypos++,0, "%s", "---------------------------------------------------------------- ");
	mvprintw(ypos++,0, "%s", "wrong input shows this screen ");
	mvprintw(ypos++,0, "%s", "quit 	- exit application ");
	mvprintw(ypos++,0, "%s", "ls 	- list attributs ");
	mvprintw(ypos++,0, "%s", " ");
	mvprintw(ypos++,0, "%s", "- if this administrator is started far away from .BFi files, ");
	mvprintw(ypos++,0, "%s", "- then the search may take some time ");
	mvprintw(ypos++,0, "%s", "---------------------------------------------------------------- ");
	move(yprompt, xprompt+promptdisplacement);
	refresh();
}

//TODO: consider using a state-machine for handling multiple command inputs -
/**
 * State-machine for handling input commands
 */
constexpr int COMBINATION(bool a,bool b,bool c,bool d)
{
	return ((int)a<<3) | ((int)b<<2) | ((int)c<<1) | ((int)d<<0);
}

void cmdStateMachine(bool conditionX,bool conditionY,bool condition1,bool condition2)
{
	switch (COMBINATION(conditionX,conditionY,condition1,condition2))
	{
		case COMBINATION(0,0,0,0):           break;
		case COMBINATION(0,0,0,1):           break;
		case COMBINATION(0,0,1,0):           break;
		case COMBINATION(0,0,1,1):           break;
		case COMBINATION(0,1,0,0):           break;
//		case COMBINATION(0,1,0,1): CodeY2(); break;
//		case COMBINATION(0,1,1,0): CodeY1(); break;
//		case COMBINATION(0,1,1,1): CodeY1(); break;
		case COMBINATION(1,0,0,0):           break;
//		case COMBINATION(1,0,0,1): CodeX2(); break;
//		case COMBINATION(1,0,1,0): CodeX1(); break;
//		case COMBINATION(1,0,1,1): CodeX1(); break;
		case COMBINATION(1,1,0,0):           break;
//		case COMBINATION(1,1,0,1): CodeX2(); break;
//		case COMBINATION(1,1,1,0): CodeX1(); break;
//		case COMBINATION(1,1,1,1): CodeX1(); break;
	}

}

std::string getInput()
{
	char incmd[80];
	getstr(incmd);
	std::string tmp(incmd);
	return tmp;
}



void handle_cmd_input()
{
	boost::smatch matches;
	boost::regex pat( "^Subject: (Re: |Aw: )*(.*)" );
	boost::regex pat_ls( "^ls *(.*)" );

	show_prompt();

	std::string input="";
	while (input!="quit")
	{
		input = getInput();
		reset_cmd_prompt();
		if (boost::regex_match(input, matches, pat))
		{
			perform_cmd_action(1);
			show_cmd_result(matches);
		}
		else {
			if (boost::regex_match(input, matches, pat_ls))
			{
				CDataDictionaryControl *pDDC = new CDataDictionaryControl();
				perform_cmd_action(1); //TODO: consider using state-machine handling for deciding whith action to perform
				std::list<std::string> listResult = pDDC->ls();    
				show_cmd_ls_result(listResult);
			}
			else 
				show_cmd_options();
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
		TCLAP::SwitchArg filesystemSwitch("f","filesystem","Starts interactiv app for filesystem command line", cmd, false);

		TCLAP::ValueArg<std::string> cmdArg("c","command","Give commands to filesystem",true,"default","string");
		cmd.add( cmdArg );

		// Parse the args.
		cmd.parse( argc, argv );
		bool startFs = filesystemSwitch.getValue();
		if( startFs ) {

			init_curses();			
			show_logo();

			spinner(2, yprompt+1, spinnerpos); // spin
			reset_cmd_prompt();
			handle_cmd_input();
		}
		else {
			std::string cmd = cmdArg.getValue();
			CUtils utils;
			utils.handle_cmdline_input(cmd);	
		}

	} catch (ArgException &e)  // catch any exceptions
	{ cerr << "error: " << e.error() << " for arg " << e.argId() << endl; }

	finish(0);
}


