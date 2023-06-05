#!/bin/bash

# Compile the new kernel module
echo "Compiling the new kernel module..."
make

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful."

    # Remove the existing kernel module
    echo "Removing the existing kernel module..."
    rmmod CSC1107_7_kernel

    # Insert the new kernel module
    echo "Inserting the new kernel module..."
    insmod CSC1107_7_kernel.ko

    # Check if the insertion was successful
    if [ $? -eq 0 ]; then
        echo "New module inserted successfully."
    else
        echo "Failed to insert the new module."
    fi
else
    echo "Compilation failed. Exiting..."
fi