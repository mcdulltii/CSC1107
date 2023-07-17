#!/bin/bash

counter_file="~/linux/CSC1107_assignment/counter"

max=3

# Check if the counter file exists
if [ -f "$counter_file" ]; then
  # Read the current counter value from the file
  reboot_counter=$(cat "$counter_file")

else
  # Initialize the counter if the file doesn't exist
  reboot_counter=0

fi

# Increment the counter
((reboot_counter++))

# Check if the maximum number of reboots has been reached
if [ $reboot_counter -gt $max ]; then
  sudo systemctl disable CSC1107_startup3.service
  exit 0

else
  # Run the bash shell script
  ~/linux/CSC1107_assignment/CSC1107_7.sh
  # Save the updated counter value to the file
  echo "$reboot_counter" > "$counter_file"

fi
