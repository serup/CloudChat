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
#include <future> 
#include "thread_safe_queue.h"

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
			int offset=0;
			int positionOnLine=0;
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
						if(isalpha(chunkdata[c+offset]))
							cout << chunkdata[c+offset];
						else
							cout << ".";
					}
					offset+=16;
					positionOnLine=16;
					cout << endl;
				}
				else {
					if(positionOnLine==16) positionOnLine=0;
					positionOnLine++;
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
			if(positionOnLine<16){
				if(usecolor) cout << "\e[1;35m" ;
				for(int l=0;l<(16-positionOnLine);l++)
					cout << "   ";
				for(int c=0;c<16;c++)
				{
					int pos = c+chunkdata.size()-positionOnLine;
					if(pos<chunkdata.size())
					{
						if(isalpha(chunkdata[pos]))
							cout << chunkdata[pos];
						else
							cout << ".";
					}
				}
				cout << endl;
			}
			if(usecolor) fprintf(stdout,"%s", "\e[0m"); 
			if(usefold) cout << "/*}}}*/" << endl;
		}


		static void showDataBlock(bool usefold,bool usecolor, std::vector<char> &chunkdata)
		{
			char* buf = chunkdata.data();
			unsigned char* membuf = reinterpret_cast<unsigned char*>(buf);
			std::vector<unsigned char> vec(std::move(membuf), std::move(membuf) + chunkdata.size());
			showDataBlock(usefold, usecolor, vec);
		}

		static void showDataBlock(bool usefold,bool usecolor, std::vector<char> chunkdata)
		{
			std::vector<unsigned char>&cache = reinterpret_cast<std::vector<unsigned char>&>(chunkdata);
			showDataBlock(usefold, usecolor, cache);
		}

		static bool showDataBlockDiff(bool usefold,bool usecolor, std::vector<unsigned char> &chunkdata, std::vector<unsigned char> &diffchunkdata)
		{
			bool bFoundError=false;

			if(usefold) cout << "/*{{{*/" << endl;
			if(usecolor) fprintf(stdout,"%s", "\e[1;35m"); 
			bool flipflop=false;
			int offset=0;
			int positionOnLine=0;
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
						if(isalpha(chunkdata[c+offset]))
							cout << chunkdata[c+offset];
						else
							cout << ".";
					}
					offset+=16;
					positionOnLine=16;
					cout << endl;
				}
				else {
					if(positionOnLine==16) positionOnLine=0;
					positionOnLine++;
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
			
			if(positionOnLine<16){
				if(usecolor) cout << "\e[1;35m" ;
				for(int l=0;l<(16-positionOnLine);l++)
					cout << "   ";
				for(int c=0;c<16;c++)
				{
					int pos = c+chunkdata.size()-positionOnLine;
					if(pos<chunkdata.size())
					{
						if(isalpha(chunkdata[pos]))
							cout << chunkdata[pos];
						else
							cout << ".";
					}
				}
				cout << endl;
			}
			if(usecolor) fprintf(stdout,"%s", "\e[0m"); 
			if(usefold) cout << "/*}}}*/" << endl;
			
		}
};


class aio
{
	class io_request
	{
		public:
			std::streambuf* is;
			unsigned read_count;
			std::promise<std::vector<char> > p;
			explicit io_request(std::streambuf& is_,unsigned count_):
				is(&is_),read_count(count_)
		{}

			io_request(io_request&& other):
				is(other.is),
				read_count(other.read_count),
				p(std::move(other.p))
		{}

			io_request():
				is(0),read_count(0)
		{}

			std::future<std::vector<char> > get_future()
			{
				return p.get_future();
			}

			void process()
			{
				try
				{
					std::vector<char> buffer(read_count);

					unsigned amount_read=0;
					while((amount_read != read_count) && 
							(is->sgetc()!=std::char_traits<char>::eof()))
					{
						amount_read+=is->sgetn(&buffer[amount_read],read_count-amount_read);
					}

					buffer.resize(amount_read);

					p.set_value(std::move(buffer));
				}
				catch(...)
				{
					p.set_exception(std::current_exception());
				}
			}
	};

	thread_safe_queue<io_request> request_queue;
	std::atomic_bool done;

	void io_thread()
	{
		while(!done)
		{
			io_request req = request_queue.pop();
			req.process();
		}
	}

	std::thread iot;

	public:
	aio():
		done(false),
		iot(&aio::io_thread,this)
	{}

	std::future<std::vector<char> > queue_read(std::streambuf& is,unsigned count)
	{
		io_request req(is,count);
		std::future<std::vector<char> > f(req.get_future());
		request_queue.push(req);
		return f;
	}

	~aio()
	{
		done=true;
		io_request req;
		request_queue.push(req);
		iot.join();
	}
};


#endif // CUTILS_H
