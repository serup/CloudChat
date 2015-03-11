/*
 ***********
 * main.cpp
 ***********
 */

#include "1_1_Profile.hpp"

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>


//////////////////////////////////////////
// howto compile :
// use makefile_cygwin.mak from inside cygwin terminal :
// use makefile_ubuntu.mak from linux prompt
//
// ex.
// make -f makefile_cygwin.mak
// make -f makefile_ubuntu.mak
//////////////////////////////////////////

static bool bDirWasSet=false;
void SetDir()
{
    if(bDirWasSet==false) {
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::cout << "[DFD_1.1] : Current path is : " << full_path << std::endl;
    chdir("../../../../"); /// this is somehow needed when running from bin/Debug - however when running from C::B then its outside of bin, unless execution working dir is set to bin/Debug
///    chdir("../../");
    boost::filesystem::path new_full_path( boost::filesystem::current_path() );
    std::cout << "[DFD_1.1] : Current path is : " << new_full_path << std::endl;
    bDirWasSet = true;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        SetDir();

        // Check command line arguments.
        if (argc != 3)
        {
            std::cerr << "Usage: DFD_1_1 <address> <port>\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    DFD_1_1 127.0.0.1 7777\n";


            return 1;
        }

        std::cout << "Server address: " << argv[1] << std::endl;
        std::cout << "Server port: " << argv[2] << std::endl;
        std::cout << "Server is starting..." << std::endl;

        C1_1_Profile C11;

        std::string strUrl="";
        strUrl = (std::string)"ws://" + (std::string)argv[1] + (std::string)":" + (std::string) argv[2];
        //bool bResult = C11.ConnectToWebserver("ws://127.0.0.1:8998");
        bool bResult = C11.ConnectToWebserver(strUrl); // dataframes will be handled inside C1_1_Profile::HandleDataframe_Response
        if(bResult)
        {
            /// wait some time before stop
            boost::posix_time::seconds workTime(32140800);
            boost::this_thread::sleep(workTime); // wait !!!
            C11.StopHandling(); // should run until program is shutdown using a dataframe or timed out
            bResult = C11.WaitForCompletion();
        }
        else
            std::cerr << "ERROR DFD_1_1 could NOT connect to server\n";

    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    return 0;
}
