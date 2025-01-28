#include <iostream>
#include <vector>

const int ROWS = 15; // Number of rows in the grid
const int COLS = 15; // Number of columns in the grid

// Function to display the grid
void showGrid(const std::vector<std::vector<int>>& grid){
    for (int i=0; i<ROWS; ++i){
        for(int j=0; j<COLS; ++j){
            std::cout << grid[i][j] << " "; 
        }
        std::cout << std::endl;
    }
}

// Function to count the number of live neighbours around a cell
int countNeighbours(const std::vector<std::vector<int>>& grid, int x, int y){
    int count = 0;

    for (int di=-1; di<=1; ++di){
        for (int dj=-1; dj<=1; ++dj){
            int pos_x = x+di;
            int pos_y = y+dj;
            // Check if the neighbouring cell is within bounds and is alive
            if (pos_x >= 0 && pos_x < ROWS 
                && pos_y >= 0 && pos_y < COLS 
                && !(di == 0 && dj == 0) 
                && grid[pos_x][pos_y] == 1){
                count += 1;
            }
        }
    }
    return count;
}

// Function to update the grid based on the rules of the Game of Life
void updateGrid(std::vector<std::vector<int>>& grid){
    int neighbours;
    std::vector<std::vector<int>> newGrid = grid;
    
    for (int i=0; i<ROWS; ++i){
        for (int j=0; j<COLS; ++j){
            neighbours = countNeighbours(grid, i, j);

            // Apply the rules of the Game of Life
            if (grid[i][j] == 1 && (neighbours == 2 || neighbours == 3)){
                newGrid[i][j] = 1;
            }
            else if (grid[i][j] == 0 && neighbours == 3){
                newGrid[i][j] = 1;
            }
            else{
                newGrid[i][j] = 0;
            }
        }
    }

    grid = newGrid; // Update the original grid with the new state
}

int main(){
    // Initialize the grid with all cells dead
    std::vector<std::vector<int>> test_grid(ROWS, std::vector<int>(COLS, 0));
    
    // Set initial live cells
    test_grid[2][1] = 1;
    test_grid[2][3] = 1;
    test_grid[3][2] = 1;
    test_grid[3][3] = 1;
    test_grid[4][2] = 1;

    // Run the Game of Life for 15 iterations
    for (int i =0; i<=15; ++i)
    {
        showGrid(test_grid); // Display the current state of the grid
        updateGrid(test_grid); // Update the grid to the next state
        std::cout << "----------------------------------- at " << i << std::endl;
    }

}
