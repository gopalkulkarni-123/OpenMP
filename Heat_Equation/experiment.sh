#!/bin/bash

# Loop through grid sizes from 1000 to 5000 in steps of 1000
for n in 1000 2000 3000 4000 5000; do
    # Compute cut points based on n
    cut1=$((n / 4))
    cut2=$((n / 2))
    cut3=$((3 * n / 4))
    
    # Define the output file for this grid size
    output_file="Results/experiment_${n}.gz"
    time_log="Results/timeValues${n}.txt"
    
    # Measure the start time
    start_time=$(date +%s%N)
    
    # Run the command and log time values
    echo "Running heat equation for grid size ${n}x${n}..."
    ./heatEqnTrial "$output_file" "$n" "$n" 3 3 "$cut1" "$cut2" "$cut3" "$cut1" "$cut2" "$cut3" > "$time_log"
    
    # Measure the end time
    end_time=$(date +%s%N)
    
    # Compute elapsed time in milliseconds
    elapsed_time=$(( (end_time - start_time) / 1000000 ))
    
    # Append elapsed time to log file
    echo "Execution time: ${elapsed_time} ms" >> "$time_log"
done

echo "All experiments completed."
