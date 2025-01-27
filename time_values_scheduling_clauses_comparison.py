import pandas as pd
import matplotlib.pyplot as plt

# Load the data
file_path = '/home/gopal/Desktop/OpenMP/scheduling_clauses_exp/results_nochunk.csv'
data = pd.read_csv(file_path)

# Clean column names (remove leading/trailing spaces)
data.columns = data.columns.str.strip()

# Compute the average time for each executable over 10 runs
average_times = data.groupby('Executable')['Time Elapsed'].mean().reset_index()

# Plot all values and the average as a scatter plot
plt.figure(figsize=(20, 10))

# Plot all values
for executable in data['Executable'].unique():
    subset = data[data['Executable'] == executable]
    plt.scatter([executable] * len(subset), subset['Time Elapsed'], color='gray', alpha=0.6, label='Individual Runs' if executable == data['Executable'].unique()[0] else "")

# Plot the averages
plt.scatter(average_times['Executable'], average_times['Time Elapsed'], color='blue', s=100, label='Average Time')

plt.xticks(rotation=45, ha='right')
plt.xlabel('Executable', fontsize=12)
plt.ylabel('Time Elapsed (s)', fontsize=12)
plt.title('Time Elapsed for Each Executable (Individual Runs and Averages)', fontsize=14)
plt.legend()
plt.grid(True, linestyle='--', alpha=0.6)
plt.tight_layout()

# Show the plot
plt.show()
