#include "TicTacToe.h"
#include <ctime>

// Run code -> Terminal -> g++ *.cpp -o gamehub -std=c++23 -Wall -Wextra -lstdc++exp -> Enter
// ./gamehub
int main()
{
    std::srand(std::time(0));

    XO::TicTacToe xo;
    xo.runGame();

    return 0;
}