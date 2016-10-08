:0,$d|:read !echo -e "$(make test | sed -e 's/OK/\\033[0;32mOK\\033[0m/g;s/FAIL/\\033[0;31mFAIL\\033[0m/g;s/TODO:/\\033[1;33mTODO:\\033[0m/g;s/></>\n</g;s/WARNING:/\\033[1;33mWARNING:\\033[0m/g')"
