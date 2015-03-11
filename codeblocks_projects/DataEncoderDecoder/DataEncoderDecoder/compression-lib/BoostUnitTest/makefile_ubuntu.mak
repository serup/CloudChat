CC=g++
CFLAGS=-c -lboost_system -std=c++11
LDFLAGS=
SOURCES=compressionTest.cpp ../compression.h ../compression.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/Debug/compressiontest
TEST=./bin/Debug/compressiontest
TESTFLAGS= --report_format=XML --report_level=detailed
CONVERT=xsltproc -o test_results.html ../test_results.xslt test_results.xml

all: $(SOURCES) $(EXECUTABLE) clean test

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

test:
	$(TEST) $(TESTFLAGS) > error.txt;$(CONVERT)

clean:
	rm *.o
