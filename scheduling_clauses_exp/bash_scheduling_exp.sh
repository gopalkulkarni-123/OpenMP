#!/bin/bash

# Define the chunk sizes
chunk_sizes=(1 10 100 1000 10000 1000000 10000000)

# Create or clear the CSV file
output_file="results.csv"
echo "Executable,Chunk Size,Run,Time Elapsed" > $output_file

# List of specific executables to run
executables=("integration_reduction_static" "integration_reduction_dynamic" "integration_reduction_guided")

# Loop through each specified executable
for executable in "${executables[@]}"; do
    if [[ -x "$executable" ]]; then
        # Loop through each chunk size
        for chunk_size in "${chunk_sizes[@]}"; do
            # Run the executable 10 times for each chunk size
            for run in {1..10}; do
                # Capture the output and extract the time elapsed
                output=$("./$executable" "25000000" "$chunk_size")
                time_elapsed=$(echo "$output" | grep "Time elapsed:" | awk '{print $3}')
                
                # Write the results to the CSV file
                echo "$(basename "$executable"),$chunk_size,$run,$time_elapsed" >> $output_file
            done
        done
    fi
done
