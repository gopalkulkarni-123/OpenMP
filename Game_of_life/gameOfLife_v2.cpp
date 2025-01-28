#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>

int ROWS; // Number of rows in the grid
int COLS; // Number of columns in the grid

//Function to store rows and columns of the grid
void initializeGrid(int argc, char* argv[]){
    if (argc != 3){
        std::cerr << "Usage: " << argv[0] << " <rows> <cols> " <<std::endl;
        exit(1);
    }

    ROWS = std::atoi(argv[1]);
    COLS = std::atoi(argv[2]);
}


// Function to display the grid
void showGrid(const std::vector<std::vector<int>>& grid){
    //#pragma omp parallel for schedule(runtime)
    for (int i=0; i<ROWS; ++i){
        for(int j=0; j<COLS; ++j){
            if (grid[i][j] == 0){
                std::cout << " " << " ";}
                else {std::cout << "*";} 
        }
        std::cout << std::endl;
    }
}

// Function to count the number of live neighbours around a cell
int countNeighbours(const std::vector<std::vector<int>>& grid, int x, int y){
    int count = 0;

    //#pragma omp parallel for schedule(runtime)
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
void updateGrid(std::vector<std::vector<int>>& grid, std::vector<std::vector<int>>& newGrid){
    int neighbours;

    //#pragma omp parallel for private (neighbours) schedule(runtime) 
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

    std::swap(grid, newGrid); 
    //grid = newGrid; // Update the original grid with the new state
}

int main(int argc, char* argv[]){
    //Set the rows and columns
    initializeGrid(argc, argv);    

    // Initialize the grid with all cells dead
    std::vector<std::vector<int>> test_grid(ROWS, std::vector<int>(COLS, 0));
    std::vector<std::vector<int>> buffer_grid(ROWS, std::vector<int>(COLS, 0));

    // Set initial live cells
    test_grid[0][0] = 1;
    test_grid[0][2] = 1;
    test_grid[1][2] = 1;
    test_grid[1][1] = 1;
    test_grid[2][1] = 1;

    // Run the Game of Life for 15 iterations
    for (int i=0; i<=15; ++i)
    {
        showGrid(test_grid); // Display the current state of the grid
        updateGrid(test_grid, buffer_grid); // Update the grid to the next state
        std::cout << "----------------------------------- at " << i << std::endl;
    }

}
