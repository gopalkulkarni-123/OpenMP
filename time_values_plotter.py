import os
import pandas as pd
import matplotlib.pyplot as plt

# Get all CSV files in the current directory
csv_files = [file for file in os.listdir('.') if file.endswith('_optimized.csv')]

#csv_files = [csv_files[2], csv_files[1], csv_files[3], csv_files[0]] # Custom order for unoptimzed compilation

csv_files = [csv_files[3], csv_files[0], csv_files[2], csv_files[1]] # Custom order for optimized compilation

# Initialize a dictionary to store the average time values for each file
average_time_values = {}

# Initialize a dictionary to store all time values for each file
all_time_values = {}

# Read each CSV file and calculate the average time value
for file in csv_files:
    df = pd.read_csv(file)
    if 'Elapsed Time' in df.columns:
        average_time = df['Elapsed Time'].mean()
        average_time_values[file] = average_time
        all_time_values[file] = df['Elapsed Time'].tolist()

# Define custom labels
custom_labels = ["Reduction", "Round-robin", "Sequential", "Block-wise"]

# Set the figure size (width, height)
width, height = 15, 9  # Customize these values as needed
plt.figure(figsize=(width, height))

# Plot all the time values for each file
for i, (file, times) in enumerate(all_time_values.items(), start=1):
    plt.scatter([i] * len(times), times, label=f'{custom_labels[i-1]} Data Points', alpha=0.5)

# Plot the average time values for each file
average_time_values_formatted = [f'{value:.3f}' for value in average_time_values.values()]
plt.scatter(range(1, len(average_time_values) + 1), average_time_values.values(), color='red', label='Average Time Values')

# Annotate the average time values
for i, (file, avg_time) in enumerate(average_time_values.items(), start=1):
    plt.text(i, avg_time, f'{avg_time:.3f}', ha='center', va='bottom', fontsize=10, color='black')

# Add labels and legend
plt.xlabel('Filename')
plt.ylabel('Time')
plt.title('Time Values from CSV Files for optimized')
plt.xticks(ticks=range(1, len(csv_files) + 1), labels=custom_labels, rotation=45, ha='right')
plt.legend(loc='upper left', bbox_to_anchor=(1, 1), fontsize='large')

# Show the plot
plt.tight_layout()
plt.savefig('time_values_with_averages_optimized.png')