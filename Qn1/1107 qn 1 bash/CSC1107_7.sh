#!/bin/bash

# Display currently logged in user and current working directory
echo "Currently logged in user: $(whoami)"
echo "Current working directory: $(pwd)"

# Create new user: CSC1107_7
sudo useradd CSC1107_7

# Change default shell for new user from sh to bash (/etc/passwd)
sudo usermod --shell /bin/bash CSC1107_7

# Display the line containing the new user with grep command
sudo grep CSC1107_7 /etc/passwd

# Check if folder (Folder_7) is in home directory:
#    > yes = delete all
#    > no = create new
if [ -d "$HOME/Folder_7" ]; then
  sudo rm -rf "$HOME/Folder_7"
fi

sudo mkdir "$HOME/Folder_7"

# Change directory into new folder
cd "$HOME/Folder_7"
echo "Current working directory: $(pwd)"

# Copy files from folder /linux/csc1107_assignment into new folder
sudo cp ~/linux/CSC1107_assignment/* .

# Compile loadable kernel module with makefile utility
make -C /lib/modules/6.1.21-v8+/build M=/home/lk-pi/linux/CSC1107_assignment modules

# List all files in current folder
ls

# Ensure the compiled loadable kernel module into the linux kernel
if [ $? -eq 0 ]; then
  sudo insmod CSC1107_7_kernel.ko
fi

# Use lsmod and grep to display if insertion successful
lsmod | grep CSC1107_7_kernel

# modinfo to display loadable kernel module information
modinfo CSC1107_7_kernel.ko

# dmesg to display last 5 messages in the kernel log buffer
dmesg | tail -n 5

# Compile the user space application program into an executable
sudo gcc CSC1107_7_user.c -o CSC1107_7_user -lssl -lcrypto

# Invoke and execute compiled program
sudo ./CSC1107_7_user

# Display last 12 lines of /var/log/syslog file
tail -n 12 /var/log/syslog

# Remove the loadable kernel module
sudo rmmod CSC1107_7_kernel

# dmesg to display last 3 messages in kernel log buffer
dmesg | tail -n 3

# Delete the user from os
sudo rm -f CSC1107_7

# Display the line containing new user if still present. else, deleted successfully
sudo grep CSC1107_7 /etc/passwd

# Display current working directory
echo "Current working directory: $(pwd)"

# List all files in current directory
ls

# Print a message:
#      “The bash shell script of CSC1107_7 has finished all tasks, and stop here.”
echo "The bash shell script of CSC1107_7 has finished all tasks, and stop here."

