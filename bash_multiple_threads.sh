#!/bin/bash

# Define the number of steps and iterations
num_steps=2500000000
num_iter=10

# Set the number of threads
num_threads=(1 2 3 4 5 6 7 8)

# Run the program for each thread count
for i in "${num_threads[@]}"; do
    output_file="integration_reduction_optimized_with_${i}_threads.csv"
    echo "Iteration,Elapsed Time" > "$output_file"
    for ((j=1; j<=num_iter; j++)); do
        export OMP_NUM_THREADS=$i
        output=$(./integration_reduction_optimized $num_steps)
        elapsed_time=$(echo "$output" | grep "Time elapsed:" | awk '{print $3}')
        echo "$j,$elapsed_time" >> "$output_file"
    done
done