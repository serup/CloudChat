//////////////////////////////////////////
// TESTCASE
//////////////////////////////////////////
// howto compile :  g++ -g -o md5test webserver_test.cpp ../../request_handler.hpp ../../request_handler.cpp ../../reply.hpp ../../reply.cpp -lboost_system
// HOWTO COMPILE ON WINDOWS CYGWIN
// g++ -mthreads -g -o md5test webserver_test.cpp ../../request_handler.hpp ../../request_handler.cpp ../../reply.hpp ../../reply.cpp -I"/cygdrive/c/cygwin/usr/include/w32api/" -I"/cygdrive/c/cygwin/usr/local/lib/" -L"/cygdrive/c/Program Files/boost_1_53_0/libs" -D_WIN32 -D_WIN32_WINNT -D__USE_W32_SOCKETS -D__CYGWIN__ -D__SunOS -D__MINGW32__ -D__MSABI_LONG=long -lws2_32 -lboost_system-mt.dll -lboost_signals-mt.dll -lboost_thread-mt.dll -std=gnu++11
//
// history:
// g++ -g -o md5test webserver_test.cpp request_handler_test.cpp ../../request_handler.hpp ../../request_handler.cpp ../../reply.hpp ../../reply.cpp -I"/cygdrive/c/Program Files/boost/boost_1_53_0" -L"/cygdrive/c/Program Files/boost_1_53_0/libs" -D_WIN32 -D__MINGW32__ -D__CYGWIN__ -D__SunOS  -std=gnu++0x
//
// Make sure that boost_1_53_0 has been downloaded to /cygdrive/c/usr/local/contribute/
// Then use ./bootstrap.sh and afterwards ./b2 install
// This will then build and copy files to /usr/local/include ....
// If this is not done prior to build then an error such as
// /usr/lib/gcc/i686-pc-cygwin/4.7.2/cc1plus.exe: error while loading shared libraries: ?: cannot open shared object file: No such file or directory
// /usr/lib/gcc/i686-pc-cygwin/4.7.2/cc1plus.exe: error while loading shared libraries: ?: cannot open shared object file: No such file or directory
// /usr/lib/gcc/i686-pc-cygwin/4.7.2/cc1plus.exe: error while loading shared libraries: ?: cannot open shared object file: No such file or directory
// could occur
// NB! its important to use -std=gnu++0x or -std=gnu++11
//  http://www.fir3net.com/Cygwin/cygwin-package-installation.html
//////////////////////////////////////////
// how to test:
//
// (without the ReportRedirector struct added below; comment out the BOOST_AUTO_TEST_MAIN define)
// ./md5test --report_format=XML --report_level=detailed 2> log.xml
// or without detail xml log
// ./md5test
// how to run(with ReportRedirector struct added)
// ./md5test --report_format=XML --report_level=detailed
// output will be in file: test_results.xml
// test output with
// http://www.w3schools.com/xsl/tryxslt.asp?xmlfile=cdcatalog&xsltfile=cdcatalog
// xsltproc -o test.html ../test_results.xslt test_results.xml
//
//////////////////////////////////////////
//////////////////////////////////////////
// http://www.drdobbs.com/cpp/c11-uniqueptr/240002708
// http://gcc.gnu.org/wiki/C11Status
// http://gcc.gnu.org/gcc-4.8/cxx0x_status.html
// http://bitten.edgewall.org/wiki/BoostTest
// http://wiki.hudson-ci.org/display/HUDSON/xUnit+Plugin
// http://www.w3schools.com/xsl/tryxslt.asp?xmlfile=cdcatalog&xsltfile=cdcatalog
// http://xmlsoft.org/XSLT/downloads.html
// http://www.brothersoft.com/xsltransformer-download-138931.html
// http://stackoverflow.com/questions/6527112/are-there-any-xslt-processing-command-line-tools
// http://linux.about.com/library/cmd/blcmdl1_xsltproc.htm
// http://www.sagehill.net/docbookxsl/InstallingAProcessor.html  // xsltproc for cygwin help installation
// http://www.fir3net.com/Cygwin/cygwin-package-installation.html
// how to install xsltproc on cygwin: apt-cyg install libxslt
/////
// boost build:
// ./b2 -j4 cxxflags="-std=c++11" -d+2 --prefix="/cygdrive/c/Program Files/boost/boost_1_53_0" --build-type=complete --layout=tagged --with-date_time --with-thread --with-program_options --with-regex --with-test --with-system --with-serialization --with-graph --with-filesystem= --disable-filesystem2 stage
//
//////////////////////////////////////////
