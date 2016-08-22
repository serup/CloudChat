:0,$d|:read !echo -e "$(make -f makefile_ubuntu.mak test | sed -e 's/OK/\\033[0;32mOK\\033[0m/g;s/FAIL/\\033[0;31mFAIL\\033[0m/g')"
