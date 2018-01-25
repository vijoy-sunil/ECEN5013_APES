#!/bin/bash

# script to compile and install kernel

# dependency update
echo "updating dependencies...."
sudo apt-get install build-essential ncurses-dev xz-utils libssl-dev bc

cp /boot/config-$(uname -r) .config
echo "checking boot configuration...."

sudo make defconfig
sudo make modules_install
sudo make install

echo "update complete"

 

