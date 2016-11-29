#!/usr/bin bash
# 
# Installation
#
# Download the latest vimball from http://conque.googlecode.com
#
# Open the .vba or .vmb file with sudo Vim and run the following commands: :so % :q That's it! The :ConqueTerm command will be available the next time you start Vim. You can delete the .vba file when you've verified Conque was successfully installed.
#

echo "fetch the conque plugin for vim - inside terminal and gdb debugger for wim windows"
#sudo wget -O "conque_2.3.tar.gz" https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/conque/conque_2.3.tar.gz
sudo wget -O "conque_2.3.vmb" https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/conque/conque_2.3.vmb
#sudo vim conque_2.3.vmb :so %
sudo wget -O "conque_gdb.vmb" http://www.vim.org/scripts/download_script.php?src_id=24138
#sudo vim conque_gdb.vom :so %
