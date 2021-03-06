#!/usr/bin/env bash

echo "remove -if any- old tmux installation"
sudo apt-get remove --auto-remove tmux
sudo rm -rf ~/.tmux
echo "fetch tmux and resurect plugin"
sudo apt-get install tmux
git clone https://github.com/tmux-plugins/tmux-resurrect ~/.tmux
git clone https://github.com/tmux-plugins/tpm ~/.tmux/plugins/tpm
echo "copy .bash_profile and .tmux.conf files"
sudo cp ../.bash_profile ~/.
sudo cp ../.tmux.conf ~/.
sudo cp ../.bash_aliases ~/.
echo "-done -  sudo start tmux, then write <prefix> I  -- this will install tmux plugins - NB! NOW RESTART PC inorder for resurrect to work " 
