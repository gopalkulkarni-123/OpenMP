#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>

int ROWS, COLS, NUM_X_CUTS, NUM_Y_CUTS;
std::vector<int> X_CORDS, Y_CORDS;

struct BlockOfGrid {
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    std::vector<std::vector<int>> cells;
    
    BlockOfGrid(int x_min, int x_max, int y_min, int y_max) 
        : xMin(x_min), xMax(x_max), yMin(y_min), yMax(y_max) {
        cells.resize(xMax - xMin, std::vector<int>(yMax - yMin, 0));
    }

    int countNeighbours(const std::vector<std::vector<int>>& grid, int x, int y){
        int count = 0;
        #pragma omp parallel for collapse(2) reduction(+:count)
        for (int di = -1; di <= 1; ++di){
            for (int dj = -1; dj <= 1; ++dj){
                if (di == 0 && dj == 0) continue;
                int pos_x = x + di;
                int pos_y = y + dj;
                if (pos_x >= 0 && pos_x < ROWS && pos_y >= 0 && pos_y < COLS) {
                    count += grid[pos_x][pos_y];
                }
            }
        }
        return count;
    }

    void computeNextState(const std::vector<std::vector<int>>& grid) {
        std::vector<std::vector<int>> newCells = cells;
        #pragma omp parallel for collapse(2)
        for (int i = xMin; i < xMax; ++i){
            for (int j = yMin; j < yMax; ++j){
                int neighbours = countNeighbours(grid, i, j);
                int local_x = i - xMin;
                int local_y = j - yMin;
                
                if (grid[i][j] == 1 && (neighbours == 2 || neighbours == 3)){
                    newCells[local_x][local_y] = 1;
                }
                else if (grid[i][j] == 0 && neighbours == 3){
                    newCells[local_x][local_y] = 1;
                }
                else{
                    newCells[local_x][local_y] = 0;
                }
            }
        }
        cells = newCells;
    }

    void updateGlobalGrid(std::vector<std::vector<int>>& grid) {
        #pragma omp parallel for collapse(2)
        for (int i = xMin; i < xMax; ++i){
            for (int j = yMin; j < yMax; ++j){
                grid[i][j] = cells[i - xMin][j - yMin];
            }
        }
    }
};

void initializeGrid(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <rows> <cols> <num_x_cuts> <num_y_cuts> <X1> ... <Xn> <Y1> ... <Ym>" << std::endl;
        exit(1);
    }
    ROWS = std::atoi(argv[1]);
    COLS = std::atoi(argv[2]);
    NUM_X_CUTS = std::atoi(argv[3]);
    NUM_Y_CUTS = std::atoi(argv[4]);

    int expectedArgs = 5 + NUM_X_CUTS + NUM_Y_CUTS;
    if (argc != expectedArgs) {
        std::cerr << "Error: Expected " << expectedArgs - 1 << " arguments, but received " << argc - 1 << "." << std::endl;
        exit(1);
    }

    X_CORDS.push_back(0);
    for (int i = 0; i < NUM_X_CUTS; i++) {
        X_CORDS.push_back(std::atoi(argv[i+5]));
    }
    X_CORDS.push_back(ROWS);

    Y_CORDS.push_back(0);
    for (int i = 0; i < NUM_Y_CUTS; i++) {
        Y_CORDS.push_back(std::atoi(argv[i+5+NUM_X_CUTS]));
    }
    Y_CORDS.push_back(COLS);
} 

void printGrid(const std::vector<std::vector<int>>& grid) {
    for (const auto& row : grid) {
        for (int cell : row) {
            std::cout << (cell ? "# " : ". ");
        }
        std::cout << "\n";
    }
    std::cout << "----------------\n";
}

int main(int argc, char* argv[]) {
    initializeGrid(argc, argv);
    std::vector<std::vector<int>> mainGrid(ROWS, std::vector<int>(COLS, 0));
    
    std::vector<BlockOfGrid> blocks;
    for (int i = 0; i <= NUM_X_CUTS; ++i) {
        for (int j = 0; j <= NUM_Y_CUTS; ++j) {
            blocks.emplace_back(X_CORDS[i], X_CORDS[i+1], Y_CORDS[j], Y_CORDS[j+1]);
        }
    }
    mainGrid[2][3] = mainGrid[3][4] = mainGrid[4][2] = mainGrid[4][3] = mainGrid[4][4] = 1;
    
    for (int step = 0; step < 10; ++step) {
        std::cout << "Generation " << step << ":\n";
        printGrid(mainGrid);
        
        #pragma omp parallel for
        for (size_t i = 0; i < blocks.size(); ++i) {
            blocks[i].computeNextState(mainGrid);
        }
        
        #pragma omp parallel for
        for (size_t i = 0; i < blocks.size(); ++i) {
            blocks[i].updateGlobalGrid(mainGrid);
        }
    }
    return 0;
}
