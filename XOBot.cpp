#include "XOBot.h"
#include "Utils.h"
#include "TicTacToe.h"
#include <print>
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
    auto number = 0ull;
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

    //* Finds a winning or blocking move.
    //* Returns the index of the critical position, or -1 if none found.
    auto findWinningSpot = [&](char targetIcon) -> unsigned long long int
    {
        for (const auto &line : wins_stage)
        {
            auto countTarget = 0;
            auto countEmpty = 0;
            auto TargetIndex = -1;

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

            //* Found a line with 2 target icons and 1 empty slot
            if (countTarget == 2 && countEmpty == 1)
            {
                return TargetIndex;
            }
        }

        // No critical move
        return -1;
    };

    auto change = std::rand() % 100;

    // 50% Chance to play smartly
    if (change > 50)
    {
        auto winMove = findWinningSpot(AI);
        if (winMove != -1)
        {
            // Win
            game->placeMove(winMove, AI);
            std::println("AI Medium Mode attacks at {}", winMove + 1);
            return;
        }

        auto blockMove = findWinningSpot(PLAYER);
        if (blockMove != -1)
        {
            // Block
            game->placeMove(blockMove, AI);
            std::println("AI Medium Mode attacks at {}", winMove + 1);
            return;
        }
    }

    //* No strategic move found -> fallback to random
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

//** The Minimax Recursive Function
//* isMaximizing = true  -> AI's turn (try to get highest score)
//* isMaximizing = false -> Player's turn (assume Player plays optimally to get lowest score)
/*
    TODO: Implement Alpha-Beta pruning to optimize Minimax performance
*/
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
unsigned long long int XO::XOBot::HardMode::findBestMove()
{
    std::array<char, 9ull> TestBoard = game->getMap();
    auto bestScore = -1000;
    auto move = -1;

    std::println("--- AI Hard mode Thinking ---");

    for (auto i{0uz}; i < TestBoard.size(); i++)
    {
        if (TestBoard.at(i) == ' ')
        {
            TestBoard.at(i) = AI;
            int score = minimax(TestBoard, false);
            TestBoard.at(i) = ' ';

            //* Output evaluation score for each possible move (debug)
            std::println(std::clog, "Slot {} : Score = {}", i + 1, score);

            if (score > bestScore)
            {
                bestScore = score;
                move = i;
            }
        }
    }

    std::println(">>> AI Chose Slot: {} (Best Score: {})", move + 1, bestScore);
    std::println("-------------------");

    return move;
}
void XO::XOBot::HardMode::Hard_Move()
{
    auto AI_Move = findBestMove();
    game->placeMove(AI_Move, game->getComputerIcon());
}