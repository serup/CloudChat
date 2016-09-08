#!/bin/bash
echo "fetch tmux resurect plugin"
git clone https://github.com/tmux-plugins/tmux-resurrect ~/.tmux
echo "fetch tmux plugin manager"
git clone https://github.com/tmux-plugins/tpm ~/.tmux/plugins/tpm
echo "add to .tmux.conf file"
echo "run-shell ~/.tmux/ressurect.tmux" >> ~/.tmux.conf
