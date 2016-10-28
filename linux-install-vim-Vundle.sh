#!/usr/bin/env bash
echo "-- first some essentials"
sudo apt-get install -yq build-essential cmake
sudo apt-get install -yq python-dev python3-dev
echo '-- install Vundle'
git clone https://github.com/VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim
