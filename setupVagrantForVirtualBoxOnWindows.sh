#!/bin/bash

echo "Setting up access rights for Vagrant to start VirtualBox entities outside WSL and inside virtualbox windows installation"

echo "This must be run as source - like this :   .  ./<filename>.sh"

export VAGRANT_WSL_ENABLE_WINDOWS_ACCESS="1"
export PATH="$PATH:/mnt/c/Programmer/Oracle/VirtualBox"
export VAGRANT_WSL_WINDOWS_ACCESS_USER_HOME_PATH="/mnt/c/tmp/VagrantTestdir"
#export VAGRANT_WSL_WINDOWS_ACCESS_USER_HOME_PATH="/mnt/c/Users/serup"
export VAGRANT_WSL_WINDOWS_ACCESS_USER_HOME_PATH="/mnt/c/"
export VAGRANT_DEFAULT_PROVIDER="virtualbox"
