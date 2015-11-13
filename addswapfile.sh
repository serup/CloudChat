#!/usr/bin/env bash -l

echo "- Create Storage File"
dd if=/dev/zero of=/swapfile1 bs=1024 count=524288
echo "- Secure swap file"
chown root:root /swapfile1
chmod 0600 /swapfile1
echo "- Set up Linux swap area"
mkswap /swapfile1
echo "- Enabling the swap file"
swapon /swapfile1
echo "- Update /etc/fstab file"
echo "/swapfile1 none swap sw 0 0" >> /etc/fstab
echo "- Result:"
free -m
