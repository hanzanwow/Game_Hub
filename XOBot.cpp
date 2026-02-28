#include "XOBot.h"
#include "Utils.h"
#include "TicTacToe.h"
#include <iostream>
#include <algorithm>
#include <array>

void XO::XOBot::Move()
{
    switch (game->getMode())
    {
    case Difficulty::Easy:
        EasyMode();
        break;
    case Difficulty::Medium:
        MediumMode();
        break;
    case Difficulty::Hard:
        hard.Hard_Move();
        break;
    }
}
void XO::XOBot::EasyMode()
{
    auto number = 0;
    while (true)
    {
        number = std::rand() % 9;
        if (game->placeMove(number, game->getComputerIcon()))
            break;
    }
}
void XO::XOBot::MediumMode()
{
    const std::array<std::array<int, 3>, 8> wins_stage =
        {{
            {0, 1, 2}, // Row 1
            {3, 4, 5}, // Row 2
            {6, 7, 8}, // Row 3
            {0, 3, 6}, // Col 4
            {1, 4, 7}, // Col 5
            {2, 5, 8}, // Col 6
            {0, 4, 8}, // Diagonal 1
            {2, 4, 6}  // Diagonal 1
        }};

    // Lambda function to find critical spot(win or block)
    // Returns index of the spot, or -1 if none found.
    auto findWinningSpot = [&](char targetIcon) -> int
    {
        for (const auto &line : wins_stage)
        {
            auto countTarget = 0;
            auto countEmpty = 0;
            auto TargetIndex = -1;

            // Check each position
            for (auto index : line)
            {
                auto now = game->getPositionAt(index);
                if (now == targetIcon)
                    countTarget++;

                else if (now == ' ')
                {
                    countEmpty++;
                    TargetIndex = index;
                }
            }

            // Critical Move
            if (countTarget == 2 && countEmpty == 1)
            {
                return TargetIndex;
            }
        }

        // No critical move
        return -1;
    };

    auto change = std::rand() % 100;
    if (change > 50)
    {
        // 50% Chance to play smartly
        auto winMove = findWinningSpot(AI);
        if (winMove != -1)
        {
            // Win
            game->placeMove(winMove, AI);
            std::cout << "AI Medium Mode attacks at " << winMove + 1 << std::endl;
            return;
        }

        auto blockMove = findWinningSpot(PLAYER);
        if (blockMove != -1)
        {
            // Block
            game->placeMove(blockMove, AI);
            std::cout << "AI Medium Mode blocks at " << blockMove + 1 << std::endl;
            return;
        }
    }

    // play random or no critical move
    EasyMode();
}

bool XO::XOBot::HardMode::isWinner(const std::array<char, 9> &board, const char &player) const
{
    const std::array<std::array<int, 3>, 8> wins_stage =
        {{
            {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // Rows
            {0, 3, 6},
            {1, 4, 7},
            {2, 5, 8}, // Cols
            {0, 4, 8},
            {2, 4, 6} // Diagonals
        }};

    for (const auto &line : wins_stage)
        if ((board.at(line.at(0)) == player) && (board.at(line.at(1)) == player) && (board.at(line.at(2)) == player))
            return true;
    return false;
}

bool XO::XOBot::HardMode::isBoardFull(const std::array<char, 9> &board) const
{
    for (const auto &space : board)
        if (space == ' ')
            return false;
    return true;
}

// The Minimax Recursive Function
// isMaximizing = true  -> AI's turn (try to get highest score)
// isMaximizing = false -> Player's turn (assume Player plays optimally to get lowest score)
int XO::XOBot::HardMode::minimax(std::array<char, 9> &board, bool isMaximizing)
{
    if (isWinner(board, AI))
        return 10;
    if (isWinner(board, PLAYER))
        return -10;
    if (isBoardFull(board))
        return 0;

    if (isMaximizing)
    {
        auto bestScore = -10000;
        for (auto i{0uz}; i < board.size(); i++)
        {
            if (board.at(i) == ' ')
            {
                board.at(i) = AI;                                       // Try to Make move
                bestScore = std::max(bestScore, minimax(board, false)); // Recurse
                board.at(i) = ' ';                                      // Undo
            }
        }
        return bestScore;
    }
    else
    {
        auto bestScore = 10000;
        for (auto i{0uz}; i < board.size(); i++)
        {
            if (board.at(i) == ' ')
            {
                board.at(i) = PLAYER;
                bestScore = std::min(bestScore, minimax(board, true));
                board.at(i) = ' ';
            }
        }
        return bestScore;
    }
}
int XO::XOBot::HardMode::findBestMove()
{
    std::array<char, 9> TestBoard = game->getMap();
    auto bestScore = -1000;
    auto move = -1;

    std::cout << "--- AI Hard mode Thinking ---" << std::endl;

    for (auto i{0uz}; i < TestBoard.size(); i++)
    {
        if (TestBoard.at(i) == ' ')
        {
            TestBoard.at(i) = AI;
            int score = minimax(TestBoard, false);
            TestBoard.at(i) = ' ';

            // Debug output
            std::cout << "Slot " << i + 1 << " : Score = " << score << std::endl;

            if (score > bestScore)
            {
                bestScore = score;
                move = i;
            }
        }
    }

    std::cout << ">>> AI Chose Slot: " << move + 1 << " (Best Score: " << bestScore << ")" << std::endl;
    std::cout << "-------------------" << std::endl;

    return move;
}
void XO::XOBot::HardMode::Hard_Move()
{
    auto AI_Move = findBestMove();
    game->placeMove(AI_Move, game->getComputerIcon());
}