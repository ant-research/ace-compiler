#!/bin/bash

# Check the number of arguments passed
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <cu_file> <graph_file>"
    exit 1
fi

# Gets the input file and the target directory
cu_file=$1
inc_file=$2
dest_dir=$(dirname "$inc_file")

# Check if the destination directory exists and create it if not
if [ ! -d "$dest_dir" ]; then
    echo "Destination directory does not exist. Creating: $dest_dir"
    mkdir -p "$dest_dir"
fi

# Copy the .cu file to the destination directory
cp "$cu_file" "$dest_dir"

# Copy the graph file to the destination directory
# cp "$graph_file" "$dest_dir"

# Check whether the copy operation was successful
if [ $? -eq 0 ]; then
    echo "File copied successfully."
else
    echo "Failed to copy file."
fi

# Execute instructions
echo "Running command..."
cmake --build /app/ace-compiler/release_gpu -- -j

echo "Command executed"

exit 0