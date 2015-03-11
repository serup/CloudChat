THIS cygwin Makefile can NOT run node.js with websockets -- use index.html test file with browser instead
REMEMBER to KILL scanvaserver process when done - this should happen after you press a key

NB! If node and npm-cli.js is working in cygwin and ws is installed, then use makefile :
makefile_cygwin_ws.mak
this should build and test with node, like running under linux
