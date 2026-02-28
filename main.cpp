#include <iostream>
#include "TicTacToe.h"
#include "Utils.h"
#include <ctime>

// Run code -> Terminal -> g++ *.cpp -o gamehub -std=c++23 -Wall -Wextra -> Enter
// ./gamehub
int main()
{
    std::srand(std::time(0));
    XO::TicTacToe xo;
    xo.runGame();
    return 0;
}