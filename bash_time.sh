#!/bin/bash

# Define the number of steps and iterations
num_steps=2500000000
iterations=10

# Set the number of threads
export OMP_NUM_THREADS=2

# Define the programs to run
programs=(
    "integration_sequential_unoptimized"
    "integration_blockwise_unoptimized"
    "integration_round_robin_unoptimized"
    "integration_reduction_unoptimized"
)

# Run each program for the specified number of iterations
for program in "${programs[@]}"; do
    output_file="${program}.csv"
    echo "Iteration,Elapsed Time" > "$output_file"
    
    for ((i=1; i<=iterations; i++)); do
        output=$(./$program $num_steps)
        elapsed_time=$(echo "$output" | grep "Time elapsed:" | awk '{print $3}')
        echo "$i,$elapsed_time" >> "$output_file"
    done
done


# Run this for multiple threads