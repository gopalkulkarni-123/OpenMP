#!/bin/bash

# Loop through grid sizes from 1000 to 5000 in steps of 1000
for n in 2 4 8 16 32; do
    # Compute cut points based on n
    #cut1=$((n / 4))
    #cut2=$((n / 2))
    #cut3=$((3 * n / 4))

    # Set the number of threads 
    export OMP_NUM_THREADS=$n
    export OMP_SCHEDULE="dynamic"
    echo $OMP_NUM_THREADS
	
    # Define the output file for this grid size
    output_file="./Results/NumThrds_2_experiment_${n}.csv"
    time_log="./Results/NumThrds_2_timeValues_${n}.txt"
    
    # Measure the start time
    start_time=$(date +%s%N)
    
    # Run the command and log time values
    echo "Running heat equation for grid size 200 x 200 for threads "$n" ..."
    
    #./heatEqnTrial "$output_file" "$n" "$n" 3 3 "$cut1" "$cut2" "$cut3" "$cut1" "$cut2" "$cut3" > "$time_log"
    
    ./heatEqnTrial "$output_file" "200" "200" 0 0 > "$time_log"
    
    # Measure the end time
    end_time=$(date +%s%N)
    
    # Compute elapsed time in milliseconds
    elapsed_time=$(( (end_time - start_time) / 1000000 ))
    
    # Append elapsed time to log file
    echo "Execution time: ${elapsed_time} ms" >> "$time_log"
    
    # Sleep for 10 seconds
    sleep 60
done

echo "All experiments completed."
