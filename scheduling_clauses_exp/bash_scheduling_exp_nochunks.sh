#!/bin/bash

# Create or clear the CSV file
output_file="results_nochunk.csv"
echo "Executable, Run, Time Elapsed" > $output_file

# List of specific executables to run
executables=("integration_reduction_baseline" "integration_reduction_static_nochunks" "integration_reduction_dynamic_nochunks" "integration_reduction_guided_nochunks" "integration_reduction_auto")

# Loop through each specified executable
for executable in "${executables[@]}"; do
    if [[ -x "$executable" ]]; then
        # Run the executable 10 times
        for run in {1..10}; do
            # Capture the output and extract the time elapsed
            output=$("./$executable" "25000000")
            time_elapsed=$(echo "$output" | grep "Time elapsed:" | awk '{print $3}')
            
            # Write the results to the CSV file
            echo "$(basename "$executable"),$run,$time_elapsed" >> $output_file
        done
    fi
done
