#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
#include <math.h>
#include <iomanip>

// Global variables for grid dimensions and cut coordinates
int ROWS, COLS, NUM_X_CUTS, NUM_Y_CUTS;
const double EPS = 1e-2;
std::vector<float> X_CORDS(0), Y_CORDS(0);

// Structure to represent a block of the grid
struct BlockOfGrid {
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    float alpha = 0.2;
    float dx = 1.0;
    float dy = 1.0;
    float dt = 0.1;
    float r_x = alpha * dt/(2 * dx * dx);
    float r_y = alpha * dt/(2 * dy * dy);
    float tempDiff = 100.0;
    float maxTempDiff = 100.0;
    std::vector<std::vector<float>> localGrid;
    
    // Constructor with built-in boundary trimming
    BlockOfGrid(int x_min, int x_max, int y_min, int y_max) 
        : xMin(x_min), 
          xMax(x_max), 
          yMin(y_min), 
          yMax(y_max) {

        localGrid.resize(xMax - xMin, std::vector<float>(yMax - yMin, 0));
    }

    void computeNextStateAll(const std::vector<std::vector<float>>& grid) {
        //Computes the next state for all the blocks including the boundary blocks. For boundary blocks, it trims the edges
        maxTempDiff = 0.0;
        for (int i = std::max(1, xMin); i < std::min(xMax, ROWS - 1); ++i) {
            for (int j = std::max(1, yMin); j < std::min(yMax, COLS -1); ++j) {
                tempDiff = r_x * (grid[i+1][j] - 2*grid[i][j] + grid[i-1][j]) + r_y * (grid[i][j+1] - 2*grid[i][j] + grid[i][j-1]);
                localGrid[i - xMin][j - yMin] = grid[i][j] + tempDiff;
                maxTempDiff = std::max(maxTempDiff, tempDiff);
            }
        }
        //std::cout << "Maximum temperature difference is " << criterion <<std::endl;
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
                localGrid[0][j - yMin] = 100; // Accessing the top row of localGrid
            }
        }
        if (xMax == ROWS) {  // Bottom boundary
            for (int j = yMin; j < yMax; ++j) {
                localGrid[xMax - xMin - 1][j - yMin] = 100; // Accessing the bottom row of localGrid
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
    void updateGlobalGrid(std::vector<std::vector<float>>& grid) {
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
void showGrid(const std::vector<std::vector<float>>& grid) {
    for (const auto& row : grid) {
        for (float cell : row) {
            std::cout << std::fixed << std::setprecision(5) << cell << " ";
        }
        std::cout << "\n";
    }
    std::cout << "----------------\n";
}

int main(int argc, char* argv[]){
    initializeGrid(argc, argv);
    std::vector<std::vector<float>> mainGrid(ROWS, std::vector<float>(COLS, 0));
    double stopCriterion = 1.0;
    double localMax;
    int step = 0;

    // Create blocks of the grid based on the cut coordinates
    std::vector<BlockOfGrid> blocks;
    for (int i = 0; i <= NUM_X_CUTS; ++i) {
        for (int j = 0; j <= NUM_Y_CUTS; ++j) {
            blocks.emplace_back(X_CORDS[i], X_CORDS[i+1], Y_CORDS[j], Y_CORDS[j+1]);
        }
    }

    for(int i=25;i<35;i++){
        for(int j=25;j<35;j++){
            mainGrid[i][j] = 50;
        }
    }

    // Run the Game of Life for 20 generations
    //for (int i = 0; i < 100; ++i){
    while (stopCriterion > EPS) {  // Fixed stopping condition
        stopCriterion = 0.0;
        std::cout << "Time step " << step << ":\n";
        showGrid(mainGrid);

        // Compute the next state for each block
        #pragma omp parallel for schedule(runtime) reduction(max: stopCriterion)
        for (size_t i = 0; i < blocks.size(); ++i) {
            blocks[i].computeNextStateAll(mainGrid);
            stopCriterion = blocks[i].maxTempDiff;
        }
        //std::cout << "stop criterion is " << stopCriterion << std::endl;

        // Update the global grid
        #pragma omp parallel for schedule(runtime)
        for (size_t i = 0; i < blocks.size(); ++i) {
            blocks[i].updateGlobalGrid(mainGrid);
        }

        ++step;
    }
    return 0;
}
