#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
#include <math.h>
#include <iomanip>
#include <fstream>
#include <zlib.h>
#include <string>
#include <chrono>

// Global variables for grid dimensions and cut coordinates
int ROWS, COLS, WIDTH, NUM_X_CUTS, NUM_Y_CUTS, NUM_Z_CUTS;
std::string FILE_NAME;
const double EPS = 1e-3;
std::vector<float> X_CORDS(0), Y_CORDS(0), Z_CORDS(0);

struct BlockOfGrid {
    
    int xMin, xMax, yMin, yMax, zMin, zMax;
    float alpha = 0.75;
    float dx = 1.0;
    float dy = 1.0;
    float dz = 1.0;
    float dt = 0.1;
    float r_x = alpha * dt/(2 * dx * dx);
    float r_y = alpha * dt/(2 * dy * dy);
    float r_z = alpha * dt/(2 * dz * dz);
    float tempDiff = 100.0;
    float maxTempDiff = 100.0;
    std::vector<std::vector<std::vector<float>>> localGrid;
    
    // Constructor with built-in boundary trimming
    BlockOfGrid(int x_min, int x_max, int y_min, int y_max, int z_min, int z_max) 
        : xMin(x_min), 
          xMax(x_max), 
          yMin(y_min), 
          yMax(y_max),
          zMin(z_min),
          zMax(z_max) {

        localGrid.resize(xMax - xMin, std::vector<std::vector<float>>(yMax - yMin, std::vector<float>(zMax - zMin, 0)));
    }

    void computeNextStateAll(const std::vector<std::vector<std::vector<float>>>& grid) {
        //Computes the next state for all the blocks including the boundary blocks. For boundary blocks, it trims the edges
        maxTempDiff = 0.0;
        for (int i = std::max(1, xMin); i < std::min(xMax, ROWS - 1); ++i) {
            for (int j = std::max(1, yMin); j < std::min(yMax, COLS -1); ++j) {
                for (int k = std::max(1, zMin); k < std::min(zMax, WIDTH -1); ++k) {
                    tempDiff = r_x * (grid[i+1][j][k] - 2*grid[i][j][k] + grid[i-1][j][k])
                     + r_y * (grid[i][j+1][k] - 2*grid[i][j][k] + grid[i][j-1][k])
                     + r_z * (grid[i][j][k+1] - 2*grid[i][j][k] + grid[i][j][k-1]);
                    localGrid[i - xMin][j - yMin][k - zMin] = grid[i][j][k] + tempDiff;
                    maxTempDiff = std::max(maxTempDiff, tempDiff);
                }
            }
        }
        //std::cout << "Maximum temperature difference is " << maxTempDiff <<std::endl;
        //std::cout << "temperature difference is " << tempDiff <<std::endl;
        //std::cout << "Maximum temperature difference is " << maxTempDiff <<std::endl;
        computeNextStateEdgeCells();
    }

    void computeNextStateEdgeCells() {
        // Exit early if the block has no edge cells (i.e., it's entirely internal)
        if (xMin > 0 && xMax < ROWS && yMin > 0 && yMax < COLS && zMin > 0 && zMax < WIDTH) {
            return;
        }

        // Set edge cells to 0 only if the block touches a boundary
        if (xMin == 0) {  // Top boundary
            for (int j = yMin; j < yMax; ++j) {
                for (int k = zMin; k < zMax; ++k){
                    localGrid[0][j - yMin][k - zMin] = 0; // Accessing the top row of localGrid
                }
            }
        }

        if (xMax == ROWS) {  // Bottom boundary
            for (int j = yMin; j < yMax; ++j) {
                for (int k = zMin; k < zMax; ++k){
                    localGrid[xMax - xMin - 1][j - yMin][k - zMin] = 0; // Accessing the bottom row of localGrid
                }
            }
        }

        if (yMin == 0) {  // Left boundary
            for (int i = xMin; i < xMax; ++i) {
                for (int k = zMin; k < zMax; ++k){
                    localGrid[i - xMin][0][k - zMin] = 0; // Accessing the left column of localGrid
                }
            }
        }
        
        if (yMax == COLS) {  // Right boundary
            for (int i = xMin; i < xMax; ++i) {
                for (int k = zMin; k < zMax; ++k){
                    localGrid[i - xMin][yMax - yMin - 1][k - zMin] = 0; // Accessing the right column of localGrid
                }
            }
        }

        if (zMin == 0) {  // Left boundary
            for (int i = xMin; i < xMax; ++i) {
                for (int j = yMin; j < yMax; ++j){
                    localGrid[i - xMin][j - yMin][0] = 0; // Accessing the left column of localGrid
                }
            }
        }
        
        if (zMax == WIDTH) {  // Right boundary
            for (int i = xMin; i < xMax; ++i) {
                for (int j = yMin; j < yMax; ++j){
                    localGrid[i - xMin][j - yMin][zMax - zMin - 1] = 0; // Accessing the right column of localGrid
                }
            }
        }

        for(int i=10;i<40;i++){
            for(int j=10;j<40;j++){
    
                //Z plane initialization
                localGrid[i][j][0] = 200;
                localGrid[i][j][WIDTH - 1] = 200;
    
                //Y plane Initialization
                localGrid[i][0][j] = 200;
                localGrid[i][COLS - 1][j] = 200;
    
                //X Plane initialization
                localGrid[0][j][i] = 200;
                localGrid[ROWS - 1][j][i] = 200;
    
            }
        }

    }
          
    // Function to update the global grid with the current block's state
    void updateGlobalGrid(std::vector<std::vector<std::vector<float>>>& grid) {
        //#pragma omp parallel for schedule(runtime)
        for (int i = xMin; i < xMax; ++i) {  // Ensures xMax - 1 is included
            for (int j = yMin; j < yMax; ++j) {
                for (int k = zMin; k < zMax; ++k) {  // Ensures yMax - 1 is included
                    grid[i][j][k] = localGrid[i - xMin][j - yMin][k - zMin];
                }
            }
        }
    }
};

// Function to initialize the grid and read input parameters
void initializeGrid(int argc, char* argv[]) {
    if (argc < 8) { // At least 8 values needed before coordinates
        std::cerr << "Usage: " << argv[0] << " <file_name> <rows> <cols> <width> <num_x_cuts> <num_y_cuts> <num_z_cuts> <X1> ... <Xn> <Y1> ... <Ym> <Z1> ... <Zn>" << std::endl;
        exit(1);
    }

    //File name
    FILE_NAME = argv[1];

    // Read grid dimensions
    ROWS = std::atoi(argv[2]);
    COLS = std::atoi(argv[3]);
    WIDTH = std::atoi(argv[4]);


    // Read number of X and Y coordinates
    NUM_X_CUTS = std::atoi(argv[5]);
    NUM_Y_CUTS = std::atoi(argv[6]);
    NUM_Z_CUTS = std::atoi(argv[7]);

    int expectedArgs = 8 + NUM_X_CUTS + NUM_Y_CUTS + NUM_Z_CUTS;
    if (argc != expectedArgs) {
        std::cerr << "Error: Expected " << expectedArgs - 1 << " arguments, but received " << argc - 1 << "." << std::endl;
        exit(1);
    }

    // Read X coordinates
    X_CORDS.push_back(0);
    for (int i = 0; i < NUM_X_CUTS; i++) {
        X_CORDS.push_back(std::atoi(argv[i + 8]));
    }
    X_CORDS.push_back(ROWS);

    // Read Y coordinates
    Y_CORDS.push_back(0);
    for (int i = 0; i < NUM_Y_CUTS; i++) {
        Y_CORDS.push_back(std::atoi(argv[i + 8 + NUM_X_CUTS]));
    }
    Y_CORDS.push_back(COLS);

    // Read Z coordinates
    Z_CORDS.push_back(0);
    for (int i = 0; i < NUM_Z_CUTS; i++) {
        Z_CORDS.push_back(std::atoi(argv[i + 8 + NUM_X_CUTS + NUM_Y_CUTS]));
    }
    Z_CORDS.push_back(WIDTH);
} 

void saveCompressed(const std::vector<std::vector<std::vector<float>>>& matrix, int timeStep, const std::string& filename) {
    gzFile file = gzopen(filename.c_str(), "ab"); // Open in append mode
    if (!file) return;

    int rows = matrix.size(), cols = matrix[0].size();

    // Write the time step
    gzwrite(file, &timeStep, sizeof(int));

    // Write matrix dimensions
    gzwrite(file, &rows, sizeof(int));
    gzwrite(file, &cols, sizeof(int));

    // Write matrix data
    for (const auto& row : matrix) {
        gzwrite(file, row.data(), cols * sizeof(float));
    }

    gzclose(file);
}

void saveCSVFile(const std::vector<std::vector<std::vector<float>>>& matrix, int timeStep,const std::string& filename){
    std::ofstream file(filename + "_" + std::to_string(timeStep) +".csv");
    file << "X,Y,Z,Temperature" << "\n";

    for (int i = 0; i < ROWS; ++i){
        for (int j = 0; j < COLS; ++j){
            for (int k = 0; k < WIDTH; ++k){
                file << i << "," << j << "," << k << "," << matrix[i][j][k] << "\n";
            }
        }
    }

    file.close();
}

int main(int argc, char* argv[]){
    initializeGrid(argc, argv);
    std::vector<std::vector<std::vector<float>>> mainGrid(ROWS, std::vector<std::vector<float>>(COLS, std::vector<float>(WIDTH, 0)));
    double stopCriterion = 1.0;
    double localMax;
    int step = 0;

    // Create blocks of the grid based on the cut coordinates
    std::vector<BlockOfGrid> blocks;
    for (int i = 0; i <= NUM_X_CUTS; ++i) {
        for (int j = 0; j <= NUM_Y_CUTS; ++j) {
            for (int k = 0; k <= NUM_Z_CUTS; ++k){
                blocks.emplace_back(X_CORDS[i], X_CORDS[i+1], Y_CORDS[j], Y_CORDS[j+1], Z_CORDS[k], Z_CORDS[k+1]);
            }
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    do{
        stopCriterion = 0.0;
        if(step%10 == 0){
            //saveCompressed(mainGrid, step, FILE_NAME);
            saveCSVFile(mainGrid, step, FILE_NAME);
            std::cout << "Delta at " << step << "is" << stopCriterion;
            }

        // Compute the next state for each block
        #pragma omp parallel for schedule(runtime) reduction(max: stopCriterion)
        for (size_t i = 0; i < blocks.size(); ++i) {
            blocks[i].computeNextStateAll(mainGrid);
            stopCriterion = blocks[i].maxTempDiff;
        }

        // Update the global grid
        #pragma omp parallel for schedule(runtime)
        for (size_t i = 0; i < blocks.size(); ++i) {
            blocks[i].updateGlobalGrid(mainGrid);
        }
    
        ++step;
    }while (stopCriterion > EPS || step <= 5);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;
    std::cout << "Delta at convergence" << stopCriterion;

    return 0;
}