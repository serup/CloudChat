BUILD := build
CONVERT=xsltproc -o test_results.html ./test/test_results.xslt ./test/test_results.xml

#
# Run command line tests
# http://sc.tamu.edu/help/general/unix/redirection.html
# http://www.thegeekstuff.com/2011/10/grep-or-and-not-operators/
test:
	@ cat dataencoderdecoder.js > test/result_index.js
	@ cat compression-lib/compression.js >> test/result_index.js
	@ cat test/index.js >> test/result_index.js
	@ node test/result_index.js > error.txt
	@ cat error.txt | grep "Test of\|OK:\|FAIL:\|---\|Tests\|Passed\|Failed\|Expected\|Got\|Exception" > _error.txt
	@ echo "<TestSuite name=\"DataEncoderDecoderTest\">" > test/test_results.xml
	@ cat error.txt | grep "TestCase" >> test/test_results.xml
	@ echo "</TestSuite>" >> test/test_results.xml
	@ cat _error.txt > error.txt
	@ rm _error.txt
	@ $(CONVERT) 2>&1
	@ cat error.txt
	@ rm test/test_results.xml

.PHONY: test 
