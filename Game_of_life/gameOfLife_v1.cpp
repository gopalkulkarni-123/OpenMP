#include <iostream>
#include <vector>
#include <unistd.h> // For sleep

using namespace std;

// Dimensions of the grid
const int ROWS = 20;
const int COLS = 40;

// Function to print the grid
void printGrid(const vector<vector<int>>& grid) {
    system("clear"); // Clear the screen for better visualization
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            cout << (grid[i][j] ? "█" : " ");
        }
        cout << endl;
    }
}

// Function to count live neighbors of a cell
int countNeighbors(const vector<vector<int>>& grid, int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue; // Skip the cell itself
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < ROWS && ny >= 0 && ny < COLS) {
                count += grid[nx][ny];
            }
        }
    }
    return count;
}

// Function to update the grid based on the Game of Life rules
void updateGrid(vector<vector<int>>& grid) {
    vector<vector<int>> newGrid = grid;

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            int liveNeighbors = countNeighbors(grid, i, j);

            if (grid[i][j] == 1) {
                // A live cell dies if it has fewer than 2 or more than 3 neighbors
                if (liveNeighbors < 2 || liveNeighbors > 3) {
                    newGrid[i][j] = 0;
                }
            } else {
                // A dead cell comes to life if it has exactly 3 neighbors
                if (liveNeighbors == 3) {
                    newGrid[i][j] = 1;
                }
            }
        }
    }

    grid = newGrid; // Update the original grid
}

int main() {
    // Initialize the grid with a glider pattern
    vector<vector<int>> grid(ROWS, vector<int>(COLS, 0));
    grid[1][2] = 1;
    grid[2][3] = 1;
    grid[3][1] = 1;
    grid[3][2] = 1;
    grid[3][3] = 1;

    // Run the simulation
    while (true) {
        printGrid(grid);
        updateGrid(grid);
        usleep(200000); // Pause for 200 milliseconds
    }

    return 0;
}
