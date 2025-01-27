#include <iostream>
#include <vector>
#include <unistd.h> 

const int ROWS = 6;
const int COLS = 6;

void showGrid(const std::vector<std::vector<int>>& grid){
    //system("clear");
    for (int i=0; i<ROWS; ++i){
        for(int j=0; j<COLS; ++j){
            std::cout << grid[i][j] << " "; 
        }
        std::cout << std::endl;
    }
}

int countNeighbours(const std::vector<std::vector<int>>& grid, int x, int y){
    int count = 0;
    for (int di=-1; di<=1; ++di){
        for (int dj=-1; dj<=1; ++dj){
            int pos_x = x+di;
            int pos_y = y+dj;
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

std::vector<std::vector<int>> updateGrid(std::vector<std::vector<int>> grid){
    int neighbours;
    
    for (int i=0; i<ROWS; ++i){
        for (int j=0; j<COLS; ++j){
            neighbours = countNeighbours(grid, i, j);

            if (grid[i][j] == 0 && (neighbours == 3)){
                grid[i][j] = 1;
            } else {
                grid[i][j] = 0;
            }

            if (grid[i][j] == 1 && (neighbours == 3)){
                grid[i][j] = 1;
            } else {
                grid[i][j] = 0;
            }

        }
    }

    return grid;
}

int main(){
    std::vector<std::vector<int>> test_grid(ROWS, std::vector<int>(COLS, 0));
    for (int i=0; i<ROWS; ++i){
        for(int j=0; j<COLS; ++j){
            test_grid[i][j] = 0;
        }
    }
    test_grid[1][4] = 1;
    test_grid[3][2] = 1;
    test_grid[2][3] = 1;

    for (int i =0; i<=15; ++i)
    {
        showGrid(test_grid);
        updateGrid(test_grid);
        usleep(100000); // Pause for 200 milliseconds
        std::cout << "----------------------------------- at " << i << std::endl;
    }

}
