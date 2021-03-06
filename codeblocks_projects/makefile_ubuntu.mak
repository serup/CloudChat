#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
# IMPORTANT! HAVE LINKS AFTER THE OBJECTS FILES, LINK ORDER MATTERS
# TODO: try to make this makefile fetch obj files from obj folder, thus making
# it compatible with the C::B project build - sofar I did not succeed in this
#
# IMPORTANT! It is important to be careful when using parallel execution 
# (the -j switch; see Parallel Execution) and archives. If multiple ar commands run
# at the same time on the same archive file, they will not know about each other 
# and can corrupt the file.
#
# Possibly a future version of make will provide a mechanism to circumvent this 
# problem by serializing all recipes that operate on the same archive file.
# But for the time being, you must either write your makefiles to avoid this problem in some other way, or not use -j. 
#

info:
	@ echo "------------------------"
	@ echo " Build options..."
	@ echo "------------------------"
	@ echo "compress        -- builds the compression-lib project"
	@ echo "dataencoder     -- builds the dataencoderdecoder project"
	@ echo "database        -- builds the databasecontrol project"
	@ echo "websocketserver -- builds the scanvaserver"
	@ echo "ringbuf         -- builds the Ringbuffer project"
	@ echo "dfdfunc         -- builds DFDFunctions projects"
	@ echo "dict            -- builds the DataDictionary project"
	@ echo "total           -- builds ALL projects"
	@ echo "------------------------"
	@ echo " ex. : screen -m -d -S codeblocks make -f makefile_ubuntu.mak total &"
	@ echo " use screen -ls -- to list the make jobs "
	@ echo " use screen -r codeblocks -- to enter a make job, then ctrl-A ctrl-D to exit screen without stopping job"
	@ echo "------------------------"


all:	msgbegin compress2 dataencoder2 database2 websocketserver2 ringbuf2 dfdfunc2 dfdfunc3 dfdfunc4 dfdfunc5 dict2 msgend 	
total:	msgbegin compress2 dataencoder2 database2 websocketserver2 ringbuf2 dfdfunc2 dfdfunc3 dfdfunc4 dfdfunc5 dict2 msgend 	

clean:
	@ echo "Cleaning..."
	@ sudo rm *.gcno *.gcda total_result.txt output.txt *.xml *.html output*.txt
	@ find -type f -name *.gcno -exec rm -f {} \;
	@ find -type f -name *.o -exec rm -f {} \;
	@ find -type f -wholename '*/bin/Debug/*' -exec echo {} \;|grep -Ev '.gitignore|*.txt' | xargs rm

msgbegin:
	@ echo "-----------------------------------"
	@ echo "C++ Build started -- please wait..."	
	@ echo "-----------------------------------"
compress: msgbegin compress2 msgend
compress2: 
	@ cd DataEncoderDecoder/DataEncoderDecoder/compression-lib/BoostUnitTest;\
	make -f makefile_ubuntu.mak > ../../../../output1.txt
compress3:
	@ cd DataEncoderDecoder/DataEncoderDecoder/compression-lib/BoostUnitTest;\
	make -f makefile_ubuntu.mak test > ../../../../output1.txt
dataencoder: msgbegin dataencoder2 msgend	
dataencoder2:
	@ cd DataEncoderDecoder/DataEncoderDecoder/BoostUnitTest/DataEncoderTestSuite;\
	make -f makefile_ubuntu.mak _total > ../../../../output2.txt
database: msgbegin database2 msgend
database2:
	@ cd DataBaseControl/BoostUnitTest/databasecontrol_testsuite;\
	make -f makefile_ubuntu.mak > ../../../output3.txt
websocketserver: msgbegin websocketserver2 msgend
websocketserver2:
	@ cd websocket_server;\
	make -f makefile_webserver_ubuntu.mak > ../output4.txt
	@ cd websocket_server/BoostUnitTest/websocketserver_testsuite;\
	make -f makefile_ubuntu.mak > ../../../output5.txt
ringbuf: msgbegin ringbuf2 msgend
ringbuf2:
	@ cd Ringbuffer/BoostUnitTest/ringbuffer_testsuite;\
	make -f makefile_ubuntu.mak > ../../../output6.txt
dfdfunc: msgbegin dfdfunc2 dfdfunc3 dfdfunc4 dfdfunc5 msgend
dfdfunc2:
	@ cd DFDFunctions/1_1;\
	make -f makefile_ubuntu.mak > ../../output7.txt
dfdfunc3:
	@ cd DFDFunctions/1_1/BoostUnitTest/1_1_1_CreateProfileTestsuite;\
	make -f makefile_ubuntu.mak total > ../../../../output8.txt
dfdfunc4:
	@ cd DFDFunctions/1_1/BoostUnitTest/1_1_6_LoginProfileTestsuite;\
	make -f makefile_ubuntu.mak total > ../../../../output9.txt
dfdfunc5:
	@ cd DFDFunctions/1_1/BoostUnitTest/1_1_9_ProfileLogTestsuite;\
        make -f makefile_ubuntu.mak total > ../../../../output10.txt
dict: msgbegin dict2 msgend
dict2:
	@ cd distributeddatadictionary;\
        make -f makefile_ubuntu.mak depend > ../output11.txt;\
		cd DDDAdmin_testsuite;\
		make -f makefile_ubuntu.mak all > ../../output12.txt
msgend:
	@ echo "Build ended --"
	@ echo "------------------------"
	@ cat output*.txt > output.txt
	@ (cat output.txt | grep 'Failed\|Passed\|---\|Test of\|OK\|FAIL' > result.txt; cat result.txt; (cat result.txt|grep -c 'OK';printf "")>pass.txt; (cat result.txt|grep -c 'FAIL';printf "")>fail.txt; echo Total; printf "Passed: ";cat pass.txt; printf "Failed: "; cat fail.txt ; echo ---------)>_total_result.txt; rm result.txt; rm fail.txt; rm pass.txt ; rm output.txt
	@ cat _total_result.txt | sed '$$!N; /^\(.*\)\n\1$$/!P; D' > total_result.txt
	-@ rm _total_result.txt
	-@ rm output*.txt
	@ cat total_result.txt


.PHONY: info 
