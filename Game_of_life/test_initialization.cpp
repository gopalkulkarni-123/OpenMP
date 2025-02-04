#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>

int ROWS, COLS, NUM_X_CUTS, NUM_Y_CUTS;
std::vector<int> X_CORDS(0), Y_CORDS(0);

struct Grid {
    std::vector<int> bottomLeft;
    std::vector<int> topLeft;
    std::vector<int> topRight;
    std::vector<int> bottomRight;
    int flag;

    Grid(int xMin, int xMax, int yMin, int yMax) 
        : bottomLeft({xMin, yMin}), topLeft({xMin, yMax}), topRight({xMax, yMax}), bottomRight({xMax, yMin}), flag(2) {}
};

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

std::pair <std::vector<Grid>, std::vector<Grid>> classifyGrid(Grid inputGrid){
    std::vector<Grid> innerGrids;
    std::vector<Grid> boundaryGrids;
    for (int i=0; i<=NUM_X_CUTS; ++i){
        for (int j=0; j<=NUM_Y_CUTS; ++j){
            Grid blockOfGrid(X_CORDS[i], X_CORDS[i+1], Y_CORDS[j], Y_CORDS[j+1]);
            if (X_CORDS[i] == 0 || X_CORDS[i+1] == ROWS || Y_CORDS[j] ==0 || Y_CORDS[j+1] == COLS){
                blockOfGrid.flag = 1;
                boundaryGrids.push_back(blockOfGrid);}
            else{
                blockOfGrid.flag = 0;
                innerGrids.push_back(blockOfGrid);}
        } 
    }

    return {innerGrids, boundaryGrids};
}

int main(int argc, char* argv[]){
    //Set the rows and columns
    initializeGrid(argc, argv);
    Grid mainGrid(0, 0, ROWS, COLS);
    auto [innerGrids, boundaryGrids] = classifyGrid(mainGrid);    
    
    //Display
    std::cout << "X Coordinates: ";
    for (const auto& x : X_CORDS) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    std::cout << "Y Coordinates: ";
    for (const auto& y : Y_CORDS) {
        std::cout << y << " ";
    }
    std::cout << std::endl;

    std::cout << "Inner Grids:" << std::endl;
    for (const auto& grid : innerGrids) {
        std::cout << "Grid corners: "
                  << "BottomLeft(" << grid.bottomLeft[0] << ", " << grid.bottomLeft[1] << "), "
                  << "TopLeft(" << grid.topLeft[0] << ", " << grid.topLeft[1] << "), "
                  << "TopRight(" << grid.topRight[0] << ", " << grid.topRight[1] << "), "
                  << "BottomRight(" << grid.bottomRight[0] << ", " << grid.bottomRight[1] << ") "
                  << "Flag: " << grid.flag << std::endl;
    }

    std::cout << "Boundary Grids:" << std::endl;
    for (const auto& grid : boundaryGrids) {
        std::cout << "Grid corners: "
                  << "BottomLeft(" << grid.bottomLeft[0] << ", " << grid.bottomLeft[1] << "), "
                  << "TopLeft(" << grid.topLeft[0] << ", " << grid.topLeft[1] << "), "
                  << "TopRight(" << grid.topRight[0] << ", " << grid.topRight[1] << "), "
                  << "BottomRight(" << grid.bottomRight[0] << ", " << grid.bottomRight[1] << ") "
                  << "Flag: " << grid.flag << std::endl;
    }
}