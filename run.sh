cd codeblocks_projects
sudo make -f makefile_ubuntu.mak total 
cd ../javascript_projects
sudo make -f makefile_ubuntu.mak total
cd ..
cat codeblocks_projects/total_result.txt > output.txt
cat javascript_projects/total_result.txt >> output.txt
(cat output.txt | grep 'Failed\|Passed\|---\|Test of\|OK\|FAIL' > result.txt; cat result.txt; (cat result.txt|grep -c 'OK';printf "")>pass.txt; (cat result.txt|grep -c 'FAIL';printf "")>fail.txt; echo Total; printf "Passed: ";cat pass.txt; printf "Failed: "; cat fail.txt ; echo ---------)>_total_result.txt
rm fail.txt
rm pass.txt
rm result.txt
cat _total_result.txt > test_results.txt
./result.sh test_results.txt > test_results_color.txt
cat test_results_color.txt
rm _total_result.txt
rm output.txt

