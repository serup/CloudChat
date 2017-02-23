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
#include <type_traits>
#include <vector>
#include <tuple>
#include <cmath>
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

		static void showDataBlock(bool usefold,bool usecolor, std::vector<char> chunkdata)
		{
			std::vector<unsigned char>&cache = reinterpret_cast<std::vector<unsigned char>&>(chunkdata);
			showDataBlock(usefold, usecolor, cache);
		}


		static bool showDataBlockDiff(bool usefold,bool usecolor, std::vector<unsigned char> &chunkdata, std::vector<unsigned char> &diffchunkdata)
		{
			bool bFoundError=false;

			if(diffchunkdata.size() != chunkdata.size())
				bFoundError=true;

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
		
			return bFoundError;
		}

		////////////////////////////////////////////
		// TERMINAL GRAPH FEATURE
		////////////////////////////////////////////
		// snippet from : https://github.com/JamezQ/cli-graph/blob/master/graph.cpp
		// info : https://youtu.be/xGDd9higrzM
		//
		//
		static double round(double number)
		{
			  return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
		}

		static int fixtostep(double n)
		{
			double step = (1.0);
			return int(round(n/step)); 
		}

		static void terminalGraph()
		{
			static string axis = "+ ";
			static string line = "# ";
			static string graph = ". ";
   
			std::vector<int> values;
			values.push_back( fixtostep(4.228) ); 
			values.push_back( fixtostep(4.314) );
			values.push_back( fixtostep(4.350) );
			values.push_back( fixtostep(4.357) );
			values.push_back( fixtostep(4.361) );
			values.push_back( fixtostep(4.364) );
			values.push_back( fixtostep(4.368) );
			values.push_back( fixtostep(4.373) );
			values.push_back( fixtostep(4.376) );
			values.push_back( fixtostep(4.379) );
			values.push_back( fixtostep(4.383) );
			values.push_back( fixtostep(4.387) );
			values.push_back( fixtostep(4.390) );
			values.push_back( fixtostep(4.393) );
			values.push_back( fixtostep(4.397) );
			values.push_back( fixtostep(4.402) );
			int size = values.size();


			cout << "terminal graph: " << endl;
			cout << "*{{{" << endl;

		/*
			for(int i = 0;i< size+size+1;++i) {
				double x = (i - (size));
				x = x*step;
				// EQUATION
				double value = x*x;
				/// EQUATION
				value = value/step;
				values[i] = int(round(value));
			}
*/

			
			// cout << "TABLE:" << endl;
			// for(int i = 0; i < size+size+1;++i) {
			//   cout << (i - (size)) << " : " << values[i] << endl;
			// }
			////for(int i = 0; i < size+size+1;++i) {
			////	cout << endl;
			////}
		

			for(int i = 0;i < size;++i) {

				for(int b = 0; b < size;++b) {
					if(size - i == values[b]) 
						cout << line;
					else 
						cout << graph;
				}

				if(size -i == values[size]) 
					cout << line;
				else
					cout << axis;

				for(int b = 0; b < size;++b) {
					if(size - i == values[b+size+1])
						cout << line;
					else
						cout << graph;
				}
				cout << endl;
			}
			for(int i = 0;i < size*2+1;++i) {
				if( 0 == values[i] )
					cout << line;
				else
					cout << axis;
			}
			cout << endl;

			for(int i = 0;i < size;++i) {
				for(int b = 0; b < size;++b) {
					if(0-i-1 == values[b])
						cout << line;
					else
						cout << graph;
				}
				if(0 -i-1 == values[size]) 
					cout << line;
				else
					cout << axis;

				for(int b = 0; b < size;++b) {
					if(0-i-1 == values[b+size+1])
						cout << line;
					else
						cout << graph;
				}
				cout << endl;
			}

			cout << "*}}}" << endl;
			//delete[] values;
		}	
		////////////////////////////////////////////

};


//*************************
//
//parallell for-each version
//*************************
// code recursively divides up the range into smaller and smaller pieces. 
// Obviously an empty range doesn't require anything to happen, and a single-point range just requires calling f on the one and only value. 
// For bigger ranges then an asynchronous task is spawned to handle the first half, and then the second half is handled by a recursive call.

template<typename Iterator,typename Func>
void parallel_for_each(Iterator first,Iterator last,Func f)
{
	ptrdiff_t const range_length=last-first;
	if(!range_length)
		return;
	if(range_length==1)
	{
		f(*first);
		return;
	}

	Iterator const mid=first+(range_length/2);

	std::future<void> bgtask=std::async(&parallel_for_each<Iterator,Func>, first,mid,f);
	try
	{
		parallel_for_each(mid,last,f);
	}
	catch(...)
	{
		bgtask.wait();
		throw;
	}
	bgtask.get();   
}



// TODO: TBD
///////////////////////////////////////
// variadic params in lambda handling
// indices are a classic
// *{{{

//#include <type_traits>
//#include <vector>
//#include <tuple>
//#include <iostream>

// indices are a classic

template< std::size_t... Ns >
struct indices
{
	using next = indices< Ns..., sizeof...( Ns ) >;
};


template< std::size_t N >
struct make_indices
{
	using type = typename make_indices< N - 1 >::type::next;
};


template<>
struct make_indices< 0 >
{
	using type = indices<>;
};



// we need something to find a type's index within a list of types
template<typename T, typename U, std::size_t=0>
struct _index {};


template<typename T, typename... Us, std::size_t N>
struct _index<T,std::tuple<T,Us...>,N>
: std::integral_constant<std::size_t, N> {};

template<typename T, typename U, typename... Us, std::size_t N>
struct _index<T,std::tuple<U,Us...>,N>
: _index<T,std::tuple<Us...>,N+1> {};


// we need a way to remove duplicate types from a list of types
template<typename T,typename I=void> struct unique;

// step 1: generate indices
template<typename... Ts>
struct unique< std::tuple<Ts...>, void >
: unique< std::tuple<Ts...>, typename make_indices<sizeof...(Ts)>::type >
{

};


// step 2: remove duplicates. Note: No recursion here!
template<typename... Ts, std::size_t... Is>
struct unique< std::tuple<Ts...>, indices<Is...> >
{
	using type = decltype( std::tuple_cat( std::declval<
				typename std::conditional<_index<Ts,std::tuple<Ts...>>::value==Is,std::tuple<Ts>,std::tuple<>>::type >()... ) );
};



// a helper to turn Ts... into std::vector<Ts>...
template<typename> struct vectorize;

template<typename... Ts>
struct vectorize<std::tuple<Ts...>>
{
	using type = std::tuple< std::vector<Ts>... >;
};

// now you can easily use it to define your Store
template<typename... Ts> class Store
{
	using Storage = typename vectorize<typename unique<std::tuple<Ts...>>::type>::type;
	Storage storage;

	template<typename T>
		decltype(std::get<_index<T,typename unique<std::tuple<Ts...>>::type>::value>(storage))
		slot()
		{
			return std::get<_index<T,typename unique<std::tuple<Ts...>>::type>::value>(storage);
		}

	public:

	template<typename T> void add(T mValue) { 
		slot<T>().push_back(mValue); 
	}

	template<typename T> std::vector<T>& get() { 
		return slot<T>();
	}    
};


// *}}}
///////////////////////////////////////



#endif // CUTILS_H
