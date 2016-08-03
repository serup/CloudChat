#!/usr/bin/env bash

mkdir -p  ~/GitHub
cd ~/GitHub
git clone http://git.imagemagick.org/repos/ImageMagick.git
export MAGICK_HOME="$HOME/GitHub/ImageMagick"
path_ok=$(echo $PATH|grep $MAGICK_HOME)
if [ "" == "$path_ok" ]; then
   echo "- add magick home to PATH env variable"
   export PATH="$MAGICK_HOME/bin:$PATH"
else
   echo "- magick home already in PATH variable"
fi
LD_LIBRARY_PATH="${LD_LIBRARY_PATH:+$LD_LIBRARY_PATH:}$MAGICK_HOME/lib"
export LD_LIBRARY_PATH
