import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV file
df = pd.read_csv('/home/gopal/Desktop/OpenMP/scheduling_clauses_exp/results.csv')

# Group by Executable and Chunk Size
grouped = df.groupby(['Executable', 'Chunk Size'])

# Colors for different scheduling clauses
colors = {
    'integration_reduction_static': 'blue',
    'integration_reduction_dynamic': 'green',
    'integration_reduction_guided': 'red'
}

# Titles for plots
titles = {
    'integration_reduction_static': 'Static Scheduling',
    'integration_reduction_dynamic': 'Dynamic Scheduling',
    'integration_reduction_guided': 'Guided Scheduling'
}

# Plot each group
for name in colors.keys():
    plt.figure(figsize=(20, 10))
    filtered_df = df[df['Executable'] == name]
    grouped_filtered = filtered_df.groupby('Chunk Size')
    for chunk_size, group in grouped_filtered:
        plt.scatter(group['Chunk Size'], group['Time Elapsed'], color=colors[name], label=f'Chunk Size {chunk_size}')
        avg_time = group['Time Elapsed'].mean()
        plt.scatter(chunk_size, avg_time, color='red', s=100, zorder=5, label=f'Chunk Size {chunk_size} Avg')
    plt.title(titles[name])
    plt.xlabel('Chunk Size')
    plt.ylabel('Time Elapsed')
    plt.xscale('log')  # Set x-axis to logarithmic scale
    plt.legend()
    plt.grid(True)  # Enable grid
    #plt.show()
    plt.savefig(f"{name}.png")