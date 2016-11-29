#!/usr/bin/env bash

sudo git clone https://github.com/powerman/vim-plugin-AnsiEsc.git ~/.vim/bundle/AnsiEsc/ 
echo "go into vim and write :helptags ~/.vim/"
echo "then write :AnsiEsc"
echo "now file will be showing color escape codes as color"
echo " - if this does NOT work - then do following : "
echo "sudo vim AnsiEsc.vba"
echo ":so %"
echo ":q"



