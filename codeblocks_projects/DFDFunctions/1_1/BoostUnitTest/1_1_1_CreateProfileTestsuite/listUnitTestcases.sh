echo '**************************';echo 'UNIT TESTCASES :';echo '**************************';echo; fgrep BOOST_AUTO_TEST_CASE *.cpp | sed -e 's/BOOST_AUTO_TEST_CASE(//g' | sed -e 's/)//g'; echo '**************************';

