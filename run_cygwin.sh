find . -type f -name "*.o" -exec rm -f {} \;
cd codeblocks_projects
make -f makefile_cygwin.mak total 
cd ../javascript_projects
make -f makefile_cygwin.mak total
