import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import gzip

def read_binary(filename, step_interval=1):
    frames = []
    time_steps = []
    with gzip.open(filename, 'rb') as file:
        while True:
            # Read the time step identifier (4 bytes)
            time_step_data = file.read(4)
            if not time_step_data:
                break  # Stop when EOF is reached

            time_step = int.from_bytes(time_step_data, "little")
            #print(f"Time step: {time_step}")  # Print the time step

            # Read matrix size (rows and cols, each 4 bytes)
            rows = int.from_bytes(file.read(4), "little")
            cols = int.from_bytes(file.read(4), "little")

            # Print matrix size for debugging purposes
            #print(f"Reading matrix with dimensions: {rows} x {cols}")

            # Read the matrix data (rows * cols floats, 4 bytes each)
            matrix_data = file.read(rows * cols * 4)
            if len(matrix_data) < rows * cols * 4:
                print("Error: Not enough data for the matrix values")
                break

            matrix = np.frombuffer(matrix_data, dtype=np.float32).reshape((rows, cols))

            # Append the matrix and time step to frames and time_steps
            frames.append(matrix)
            time_steps.append(time_step)

            # Skip extra time steps if step_interval > 1
            for _ in range(step_interval - 1):
                # Skip reading the rest of the data for skipped time steps
                time_step_data = file.read(4)
                rows = int.from_bytes(file.read(4), "little")
                cols = int.from_bytes(file.read(4), "little")
                file.read(rows * cols * 4)  # Skip matrix data for the skipped time steps

    return frames, time_steps

def animate_heat_simulation(frames, time_steps, interval=50, temp_min=None, temp_max=None):
    """Animates the heat simulation using Matplotlib."""
    fig, ax = plt.subplots()
    
    # Set vmin and vmax for the temperature limits
    vmin = temp_min if temp_min is not None else np.min(frames)
    vmax = temp_max if temp_max is not None else np.max(frames)
    
    # Display the first frame
    cax = ax.imshow(frames[0], cmap="inferno", interpolation="nearest", animated=True, vmin=vmin, vmax=vmax)
    
    # Add a color bar with the specified temperature limits
    cbar = plt.colorbar(cax)
    cbar.set_label('Temperature')  # Set the label for the color bar
    cbar.set_ticks([vmin, vmax])  # Optionally, set the ticks at
    
    def update(frame_index):
        cax.set_array(frames[frame_index])
        ax.set_title(f"Time step: {time_steps[frame_index]}")
        return cax,

    ani = animation.FuncAnimation(fig, update, frames=len(frames), interval=interval, blit=True)
    ani.save("heat_simulation_2000.mp4", fps=10)
    #plt.show()

if __name__ == "__main__":
    filename = "Results/experiment_2000.gz"  # Change this to the path of your binary file
    frames, time_steps = read_binary(filename)
    
    # Set your desired temperature limits (min, max) here
    temp_min = 0.0  # Example: minimum temperature
    temp_max = 100.0  # Example: maximum temperature
    
    animate_heat_simulation(frames, time_steps, interval=50, temp_min=temp_min, temp_max=temp_max)
