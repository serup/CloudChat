#ifndef CUTILS_H
#define CUTILS_H

#include <boost/regex.hpp>
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>                
#include <boost/filesystem.hpp>                      
#include <boost/filesystem/operations.hpp>           
#include <iostream>           
#include <string>
#include <ctype.h>

using namespace std;

class CUtils
{
	public:
		std::list<std::string> handle_cmdline_input(std::string cmd);
		static void showDataBlock(bool usefold,bool usecolor, std::vector<unsigned char> &chunkdata)
		{
			if(usefold) cout << "/*{{{*/" << endl;
			if(usecolor) fprintf(stdout,"%s", "\e[1;35m"); 
			bool flipflop=false;
			for(int n=0;n<chunkdata.size(); n++)
			{
				if(( n + 1 ) % 16 == 0) {
					if(usecolor)
						fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "\e[1;35m" : " " );
					else
						fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "" : " " );
					cout << " ";
					for(int c=0;c<16;c++)
					{
						if(isalpha(chunkdata[c]))
							cout << chunkdata[c];
						else
							cout << ".";
					}
					cout << endl;
				}
				else {
					if(usecolor)
						fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "\n\e[1;35m" : " " );
					else
						fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "\n" : " " );
				}

				if(usecolor) {
					if(( n + 1 ) % 16 == 0) flipflop=!flipflop;
					if(flipflop) { 
						fprintf(stdout,"%s", "\e[0m"); 
					}
				}
				else
					fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "\n" : " " );

			}
			if(usecolor) fprintf(stdout,"%s", "\e[0m"); 
			if(usefold) cout << "/*}}}*/" << endl;
		}

		static bool showDataBlockDiff(bool usefold,bool usecolor, std::vector<unsigned char> &chunkdata, std::vector<unsigned char> &diffchunkdata)
		{
			bool bFoundError=false;

			if(usefold) cout << "/*{{{*/" << endl;
			if(usecolor) fprintf(stdout,"%s", "\e[1;35m"); 
			bool flipflop=false;
			for(int n=0;n<chunkdata.size(); n++)
			{
				if(( n + 1 ) % 16 == 0) {
					if(usecolor)
						fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "\e[1;35m" : " " );
					else
						fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "" : " " );
					cout << " ";
					for(int c=0;c<16;c++)
					{
						if(isalpha(chunkdata[c]))
							cout << chunkdata[c];
						else
							cout << ".";
					}
					cout << endl;
				}
				else {
					if(usecolor)
						fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "\n\e[1;35m" : " " );
					else
						fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "\n" : " " );
				}

				if( chunkdata[n] != diffchunkdata[n] ) {
					cout << "FAIL:";	
					fprintf(stdout, "%02X%s", diffchunkdata[n], ( n + 1 ) % 16 == 0 ? "\n" : " " );
					cout << "!=";	
					bFoundError=true;
				}

				if(usecolor) {
					if(( n + 1 ) % 16 == 0) flipflop=!flipflop;
					if(flipflop) { 
						fprintf(stdout,"%s", "\e[0m"); 
					}
				}
				else
					fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "\n" : " " );

			}
			if(usecolor) fprintf(stdout,"%s", "\e[0m"); 
			if(usefold) cout << "/*}}}*/" << endl;
			
			
	//		if(usefold) cout << "/*{{{*/" << endl;
	//		if(usecolor) fprintf(stdout,"%s", "\e[1;35m"); 
			
	//	
	//		bool flipflop=false;
	//		for(int n=0;n<chunkdata.size(); n++)
	//		{
	//			if(( n + 1 ) % 16 == 0) {
	//				if(usecolor)
	//					fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "\e[1;35m" : " " );
	//				else
	//					fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "" : " " );
	//				cout << " ";
	//				for(int c=0;c<16;c++)
	//				{
	//					if(isalpha(chunkdata[c]))
	//						cout << chunkdata[c];
	//					else
	//						cout << ".";
	//				}
	//				cout << endl;
	//			}
	//			else {
	//				if(usecolor)
	//					fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "\n\e[1;35m" : " " );
	//				else
	//					fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "\n" : " " );
	//			}

	//			if( chunkdata[n] != diffchunkdata[n] ) {
	//				cout << "FAIL:";	
	//				fprintf(stdout, "%02X%s", diffchunkdata[n], ( n + 1 ) % 16 == 0 ? "\n" : " " );
	//				cout << "!=";	
	//				bFoundError=true;
	//			}
	//			if(usecolor) {
	//				fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "\n\e[1;35m" : " " );
	//				if(( n + 1 ) % 16 == 0) flipflop=!flipflop;
	//				if(flipflop) fprintf(stdout,"%s", "\e[0m"); 
	//			}
	//			else
	//				fprintf(stdout, "%02X%s", chunkdata[n], ( n + 1 ) % 16 == 0 ? "\n" : " " );

	//		}
	//		if(usecolor) fprintf(stdout,"%s", "\e[0m"); 
	//		if(usefold) cout << "/*}}}*/" << endl;
	//		
		}
};



#endif // CUTILS_H
