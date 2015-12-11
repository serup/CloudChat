#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
# IMPORTANT! HAVE LINKS AFTER THE OBJECTS FILES, LINK ORDER MATTERS
# TODO: try to make this makefile fetch obj files from obj folder, thus making
# it compatible with the C::B project build - sofar I did not succeed in this

# http://stackoverflow.com/questions/3140974/how-to-use-sed-in-a-makefile

all:
	@ echo "------------------------"
	@ echo " Build options..."
	@ echo "------------------------"
	@ echo "dataencoder     -- builds the dataencoderdecoder project"
	@ echo "compress        -- builds the compression-lib project"
	@ echo "ssapi           -- builds the socketserverAPI project (NB node and ws needed)"
	@ echo "cloudmanager    -- builds the cloudchat project "
	@ echo "total           -- builds ALL projects"
	@ echo "------------------------"
	@ echo " ex. : make -f makefile_ubuntu.mak total"
	@ echo "------------------------"

total:	msgbegintotal dataencoder2 compress2 ssapi2 cloudmanager2 msgend 	

msgbegin:
	@ echo "-------------------------------------------"
	@ echo "JavaScript Build started -- please wait..."	
	@ echo "-------------------------------------------"
msgbegintotal:
	@ echo "----------------------------------"
	@ echo "TOTAL JavaScript Build started...  " 
	@ echo "----------------------------------"
	@ echo " " > output.txt
dataencoder: msgbegin dataencoder2 msgend	
dataencoder2:
	@ cd DataEncoderDecoder; make -f Makefile > ../output.txt
compress: msgbegin compress2 msgend
compress2:
	@ cd DataEncoderDecoder/compression-lib; make -f Makefile >> ../../output.txt
ssapi: msgbegin ssapi2 msgend
ssapi2:
	@ cd socketserverAPI; sudo make -f makefile_ubuntu.mak >> ../output.txt
cloudmanager: msgbegin cloudmanager2 msgend
cloudmanager2:
	@ cd CloudChat; make -f makefile_ubuntu.mak >> ../output.txt
msgend:
	@ echo "Build ended --"
	@ echo "------------------------"
	@ (cat output.txt | grep 'Failed\|Passed\|---\|Test of\|OK\|FAIL' > result.txt; cat result.txt; (cat result.txt|grep -c 'OK';printf "")>pass.txt; (cat result.txt|grep -c 'FAIL';printf "")>fail.txt; echo Total; printf "Passed: ";cat pass.txt; printf "Failed: "; cat fail.txt ; echo ---------)>_total_result.txt; rm result.txt; rm fail.txt; rm pass.txt ; rm output.txt 
	@ cat _total_result.txt | sed '$$!N; /^\(.*\)\n\1$$/!P; D' > total_result.txt
	@ cat total_result.txt
	@ rm _total_result.txt

.PHONY: info 
