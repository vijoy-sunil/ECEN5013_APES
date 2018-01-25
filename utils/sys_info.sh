#!/bin/bash

# script that can collect some of the important parts about your
# operating system platform and write that to a file

filename="sysinfo"
my_path="/home/vijoy/Desktop/ECEN-5013/utils"

echo "-----------------------------------------------------------------" >$my_path/$filename 
echo "                            System Information                   ">>$my_path/$filename
echo "-----------------------------------------------------------------">>$my_path/$filename

echo -e "User Info\t\t\t: $(uname -n)"					>>$my_path/$filename
echo -e "OS type/brand\t\t\t: $(uname -o)"			 	>>$my_path/$filename
echo -e "OS distribution\t\t\t: $(lsb_release -i -s)"			>>$my_path/$filename
echo -e "OS version\t\t\t: $(lsb_release -r -s)"			>>$my_path/$filename
echo -e "Kernel version\t\t\t: $(uname -r)"				>>$my_path/$filename
echo -e "Kernel build time\t\t: $(uname -v)"				>>$my_path/$filename
echo -e "System architecture\t\t: $(uname -m)"				>>$my_path/$filename

echo -e "\nKernel build information\t:| OS type      |  Kernel version |    Build user   |       GCC version         |     OS distribution    |">>$my_path/$filename
echo -e "\t\t\t\t $(cat /proc/version)"					>>$my_path/$filename

echo -e "\nFile system memory\t\t:"					>>$my_path/$filename
echo -e "-------------------\n$(df -h):"				>>$my_path/$filename

echo "-----------------------------------------------------------------">>$my_path/$filename

