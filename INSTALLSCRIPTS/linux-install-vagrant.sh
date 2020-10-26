#!/bin/bash
# https://www.vagrantup.com/downloads.html
vagrant_url="https://releases.hashicorp.com/vagrant/2.2.5/vagrant_2.2.5_x86_64.deb"
vagrant_filename="$(basename "$vagrant_url")"
wget "$vagrant_url"
sudo dpkg -i "$vagrant_filename"
rm -f "$vagrant_filename"
vagrant --version
