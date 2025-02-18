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
    
    // Constructor with built-in boundary trimming
    BlockOfGrid(int x_min, int x_max, int y_min, int y_max) 
        : xMin(x_min), 
          xMax(x_max), 
          yMin(y_min), 
          yMax(y_max) {

        localGrid.resize(xMax - xMin, std::vector<int>(yMax - yMin, 0));
        //boundary = (x_min == 0 || x_max == ROWS || y_min == 0 || y_max == COLS);
    }

    void computeNextStateAll(const std::vector<std::vector<int>>& grid) {
        //Computes the next state for all the blocks including the boundary blocks. For boundary blocks, it trims the edges
        for (int i = std::max(1, xMin); i < std::min(xMax, ROWS - 1); ++i) {
            for (int j = std::max(1, yMin); j < std::min(yMax, COLS -1); ++j) {
                // Unrolled neighbor count
                int count = 0;
                count += grid[i - 1][j - 1]; count += grid[i - 1][j]; count += grid[i - 1][j + 1];
                count += grid[i][j - 1];                             count += grid[i][j + 1];
                count += grid[i + 1][j - 1]; count += grid[i + 1][j]; count += grid[i + 1][j + 1];

                // Apply the Game of Life rules
                localGrid[i - xMin][j - yMin] = (grid[i][j] == 1 && (count == 2 || count == 3)) ||
                                                (grid[i][j] == 0 && count == 3);
            }
        }
        computeNextStateEdgeCells();
    }

    void computeNextStateEdgeCells() {
        // Exit early if the block has no edge cells (i.e., it's entirely internal)
        if (xMin > 0 && xMax < ROWS && yMin > 0 && yMax < COLS) {
            return;
        }

        // Set edge cells to 0 only if the block touches a boundary
        if (xMin == 0) {  // Top boundary
            for (int j = yMin; j < yMax; ++j) {
                localGrid[0][j - yMin] = 0; // Accessing the top row of localGrid
            }
        }
        if (xMax == ROWS) {  // Bottom boundary
            for (int j = yMin; j < yMax; ++j) {
                localGrid[xMax - xMin - 1][j - yMin] = 0; // Accessing the bottom row of localGrid
            }
        }
        if (yMin == 0) {  // Left boundary
            for (int i = xMin; i < xMax; ++i) {
                localGrid[i - xMin][0] = 0; // Accessing the left column of localGrid
            }
        }
        if (yMax == COLS) {  // Right boundary
            for (int i = xMin; i < xMax; ++i) {
                localGrid[i - xMin][yMax - yMin - 1] = 0; // Accessing the right column of localGrid
            }
        }
    }
          
    // Function to update the global grid with the current block's state
    void updateGlobalGrid(std::vector<std::vector<int>>& grid) {
        //#pragma omp parallel for schedule(runtime)
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

    // Explosion
    //mainGrid[10][2] = mainGrid[11][4] = mainGrid[12][1] = mainGrid[12][2] = mainGrid[12][5] = mainGrid[12][6] = mainGrid[12][7] = 1;

    // Oscillator
    //mainGrid[10][10] = mainGrid[10][11] = mainGrid[10][12] = 1;
    
    // Glider
    //mainGrid[0][1] = 1; mainGrid[1][2] = 1; mainGrid[2][2] = 1; mainGrid[2][1] = 1; mainGrid[2][0] = 1;

    // Gliders moving in different directions

    // Standard Right-Down Glider (↘)
    mainGrid[0][1] = 1; mainGrid[1][2] = 1; mainGrid[2][2] = 1; mainGrid[2][1] = 1; mainGrid[2][0] = 1;

    // Left-Down Glider (↙)
    mainGrid[10][8] = 1; mainGrid[9][9] = 1; mainGrid[8][9] = 1; mainGrid[8][8] = 1; mainGrid[8][7] = 1;

    // Right-Up Glider (↗)
    mainGrid[15][5] = 1; mainGrid[16][4] = 1; mainGrid[17][4] = 1; mainGrid[17][5] = 1; mainGrid[17][6] = 1;

    // Left-Up Glider (↖)
    mainGrid[25][20] = 1; mainGrid[24][21] = 1; mainGrid[23][21] = 1; mainGrid[23][20] = 1; mainGrid[23][19] = 1;

    // Right Glider (→)
    mainGrid[5][30] = 1; mainGrid[6][30] = 1; mainGrid[7][30] = 1; mainGrid[7][29] = 1; mainGrid[6][28] = 1;

    // Left Glider (←)
    mainGrid[12][50] = 1; mainGrid[11][50] = 1; mainGrid[10][50] = 1; mainGrid[10][51] = 1; mainGrid[11][52] = 1;

    // Upward Glider (↑)
    mainGrid[35][10] = 1; mainGrid[35][9] = 1; mainGrid[35][8] = 1; mainGrid[34][8] = 1; mainGrid[33][9] = 1;

    // Downward Glider (↓)
    mainGrid[40][20] = 1; mainGrid[40][21] = 1; mainGrid[40][22] = 1; mainGrid[41][22] = 1; mainGrid[42][21] = 1;

    // Random Glider 1 (↘)
    mainGrid[50][5] = 1; mainGrid[51][6] = 1; mainGrid[52][6] = 1; mainGrid[52][5] = 1; mainGrid[52][4] = 1;

    // Random Glider 2 (↙)
    mainGrid[60][30] = 1; mainGrid[59][31] = 1; mainGrid[58][31] = 1; mainGrid[58][30] = 1; mainGrid[58][29] = 1;


    // Run the Game of Life for 20 generations
    for (int step = 0; step < 200; ++step) {
        std::cout << "Generation " << step << ":\n";
        showGrid(mainGrid);
        
        // Compute the next state for each block
        #pragma omp parallel for schedule(runtime)
        for (size_t i = 0; i < blocks.size(); ++i) {
            blocks[i].computeNextStateAll(mainGrid);
        }

        #pragma omp parallel for schedule(runtime)
        for (size_t i = 0; i < blocks.size(); ++i) {
            blocks[i].updateGlobalGrid(mainGrid);
        }
    }
    return 0;
}