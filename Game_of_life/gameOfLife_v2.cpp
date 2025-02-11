#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>

// Global variables for grid dimensions and cut coordinates
int ROWS, COLS, NUM_X_CUTS, NUM_Y_CUTS;
std::vector<int> X_CORDS(0), Y_CORDS(0);

// Structure to represent a block of the grid
struct BlockOfGrid {
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    std::vector<std::vector<int>> localGrid;
    bool boundary;
    
    // Constructor to initialize the BlockOfGrid
    BlockOfGrid(int x_min, int x_max, int y_min, int y_max) 
        : xMin(x_min), xMax(x_max), yMin(y_min), yMax(y_max), boundary(false) {
        localGrid.resize(xMax - xMin, std::vector<int>(yMax - yMin, 0));
        boundary = (xMin == 0 || xMax == ROWS || yMin == 0 || yMax == COLS);
    }

    // Function to count the number of alive neighbours for a given cell
    int countNeighbours(const std::vector<std::vector<int>>& grid, int x, int y) {
        int count = 0;

        #pragma omp parallel for reduction(+:count) schedule(runtime)
        for (int di=-1; di<=1; ++di){
            for (int dj=-1; dj<=1; ++dj){
                int pos_x = x+di;
                int pos_y = y+dj;
                // Check if the neighbouring cell is within bounds and is alive
                if (boundary == true){
                    if (pos_x >= 0 && pos_x < ROWS 
                        && pos_y >= 0 && pos_y < COLS 
                        && !(di == 0 && dj == 0) 
                        && grid[pos_x][pos_y] == 1){
                        count += 1;
                    }
                } else{
                    if (!(di == 0 && dj == 0) 
                    && grid[pos_x][pos_y] == 1){
                    count += 1;}

                }
            }
        }
        return count;
    }

    // Function to update the grid based on the rules of the Game of Life
    void computeNextState(std::vector<std::vector<int>>& grid){
        int neighbours;

        #pragma omp parallel for schedule(runtime)
        for (int i=xMin; i<xMax; ++i){
            for (int j=yMin; j<yMax; ++j){
                neighbours = countNeighbours(grid, i, j);

                // Apply the rules of the Game of Life
                if (grid[i][j] == 1 && (neighbours == 2 || neighbours == 3)){
                    localGrid[i - xMin][j - yMin] = 1;
                }
                else if (grid[i][j] == 0 && neighbours == 3){
                    localGrid[i - xMin][j - yMin] = 1;
                }
                else{
                    localGrid[i - xMin][j - yMin] = 0;
                }
            }
        }
    }

    // Function to update the global grid with the current block's state
    void updateGlobalGrid(std::vector<std::vector<int>>& grid) {
        #pragma omp parallel for schedule(runtime)
        for (int i = xMin; i < xMax; ++i) {  // Ensures xMax - 1 is included
            for (int j = yMin; j < yMax; ++j) {  // Ensures yMax - 1 is included
                grid[i][j] = localGrid[i - xMin][j - yMin];
            }
        }
    }
};

// Function to initialize the grid and read input parameters
void initializeGrid(int argc, char* argv[]) {
    if (argc < 5) { // At least 4 values needed before coordinates
        std::cerr << "Usage: " << argv[0] << " <rows> <cols> <num_x_cords> <num_y_cords> <X1> ... <Xn> <Y1> ... <Ym>" << std::endl;
        exit(1);
    }

    // Read grid dimensions
    ROWS = std::atoi(argv[1]);
    COLS = std::atoi(argv[2]);

    // Read number of X and Y coordinates
    NUM_X_CUTS = std::atoi(argv[3]);
    NUM_Y_CUTS = std::atoi(argv[4]);

    int expectedArgs = 5 + NUM_X_CUTS + NUM_Y_CUTS;
    if (argc != expectedArgs) {
        std::cerr << "Error: Expected " << expectedArgs - 1 << " arguments, but received " << argc - 1 << "." << std::endl;
        exit(1);
    }

    // Read X coordinates
    X_CORDS.push_back(0);
    for (int i = 0; i < NUM_X_CUTS; i++) {
        X_CORDS.push_back(std::atoi(argv[i+5]));
    }
    X_CORDS.push_back(ROWS);

    // Read Y coordinates
    Y_CORDS.push_back(0);
    for (int i = 0; i < NUM_Y_CUTS; i++) {
        Y_CORDS.push_back(std::atoi(argv[i+5+NUM_X_CUTS]));
    }
    Y_CORDS.push_back(COLS);
} 

// Function to display the grid
void showGrid(const std::vector<std::vector<int>>& grid) {
    for (const auto& row : grid) {
        for (int cell : row) {
            std::cout << (cell ? "# " : ". ");
        }
        std::cout << "\n";
    }
    std::cout << "----------------\n";
}

int main(int argc, char* argv[]){
    initializeGrid(argc, argv);
    std::vector<std::vector<int>> mainGrid(ROWS, std::vector<int>(COLS, 0));

    // Create blocks of the grid based on the cut coordinates
    std::vector<BlockOfGrid> blocks;
    for (int i = 0; i <= NUM_X_CUTS; ++i) {
        for (int j = 0; j <= NUM_Y_CUTS; ++j) {
            blocks.emplace_back(X_CORDS[i], X_CORDS[i+1], Y_CORDS[j], Y_CORDS[j+1]);
        }
    }

    // Initialize a glider pattern in the grid
    mainGrid[2][3] = mainGrid[3][4] = mainGrid[4][2] = mainGrid[4][3] = mainGrid[4][4] = 1;

    // Run the Game of Life for 20 generations
    for (int step = 0; step < 20; ++step) {
        std::cout << "Generation " << step << ":\n";
        showGrid(mainGrid);
        
        // Compute the next state for each block
        #pragma omp parallel for schedule(runtime)
        for (size_t i = 0; i < blocks.size(); ++i) {
            blocks[i].computeNextState(mainGrid);
        }
        
        // Update the global grid with the new state of each block
        #pragma omp parallel for schedule(runtime)
        for (size_t i = 0; i < blocks.size(); ++i) {
            blocks[i].updateGlobalGrid(mainGrid);
        }
    }
    return 0;
}