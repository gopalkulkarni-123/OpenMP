import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

def read_data(file_path):
    with open(file_path, 'r') as file:
        data = []
        current_generation = []
        for line in file:
            if line.startswith("Time step"):
                if current_generation:
                    data.append(np.array(current_generation))
                    current_generation = []
            else:
                try:
                    row = list(map(float, line.split()))
                    current_generation.append(row)
                except ValueError:
                    continue
        if current_generation:
            data.append(np.array(current_generation))
    return data

def save_frame(data, frame, filename):
    plt.imshow(data[frame], cmap='viridis', interpolation='nearest', vmin=0, vmax=100)
    plt.colorbar()
    plt.title(f'Heatmap at Time {round(frame*0.1, 3)}s')
    plt.savefig(filename)
    plt.close()

def animate_heatmap(data, interval=100):
    fig, ax = plt.subplots()
    cax = ax.imshow(data[0], cmap='viridis', interpolation='nearest', vmin=0, vmax=100)
    fig.colorbar(cax)
    plt.title('Heatmap Animation of test_reference')

    def update(frame):
        cax.set_array(data[frame])
        ax.set_title(f'Time {round(frame*0.1, 3)}s')
        return cax,

    ani = animation.FuncAnimation(fig, update, frames=len(data), interval=interval, blit=False, repeat=False)
    ani.save('test_reference.mp4', fps=10)

if __name__ == "__main__":
    file_path = 'test_output.txt'
    data = read_data(file_path)

    # Sample 100 data points evenly from the data
    if len(data) > 100:
        data = data[::len(data)//1000]

    # Save specific frames
    save_frame(data, 0, 'frame_0.png')
    middle_frame_1 = len(data) // 2
    middle_frame_2 = middle_frame_1 + 1
    save_frame(data, middle_frame_1, f'frame_{middle_frame_1}.png')
    save_frame(data, middle_frame_2, f'frame_{middle_frame_2}.png')
    save_frame(data, len(data) - 1, f'frame_{len(data) - 1}.png')

    animate_heatmap(data, interval=25)  # Set the interval to 25 milliseconds to speed up by 100x