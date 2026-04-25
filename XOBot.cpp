#include "XOBot.h"
#include "Utils.h"
#include "TicTacToe.h"
#include <print>
#include <iostream>
#include <algorithm>
#include <array>

XO::XOBot::XOBot(TicTacToe *API) : game(API), PLAYER(API->getPlayerIcon()), AI(API->getComputerIcon()), hard(API, AI, PLAYER) {}

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
    while (true)
    {
        auto number = std::rand() % 9;
        if (game->placeMove(number, game->getComputerIcon()))
            break;
    }
}
void XO::XOBot::MediumMode()
{
    const std::array<std::array<size_t, 3>, 8> wins_stage =
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
    auto findWinningSpot = [&](char targetIcon) -> int
    {
        for (const auto &line : wins_stage)
        {
            auto countTarget = 0u;
            auto countEmpty = 0u;
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

    auto winMove = findWinningSpot(AI);
    if (winMove != -1)
    {
        // Win
        game->placeMove(winMove, AI);
        std::println("\n {}""AI Medium Mode attacks at"" {}{}",Utils::Color::BLUE, winMove + 1, Utils::Color::RESET);
        return;
    }

    auto blockMove = findWinningSpot(PLAYER);
    if (blockMove != -1)
    {
        // Block
        game->placeMove(blockMove, AI);
        std::println("\n {}""AI Medium Mode block at"" {}{}",Utils::Color::BLUE, blockMove + 1, Utils::Color::RESET);
        return;
    }

    //* No strategic move found -> fallback to random
    EasyMode();
}

bool XO::XOBot::HardMode::isWinner(const std::array<char, 9> &board, const char &player) const
{
    const std::array<std::array<size_t, 3>, 8> wins_stage =
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

int XO::XOBot::HardMode::positionBonus(const std::array<char, 9> &board) const
{
    //* Weight per cell = number of winning lines passing through it.
    //  Center (4) > Corners (3) > Edges (2). Range bounded well below the depth step (100),
    //  so this bonus only ever breaks ties between equally-deep outcomes.
    static constexpr std::array<int, 9> weights = {3, 2, 3, 2, 4, 2, 3, 2, 3};
    auto bonus = 0;
    for (auto i{0uz}; i < board.size(); i++)
    {
        if (board.at(i) == AI)
            bonus += weights.at(i);
        else if (board.at(i) == PLAYER)
            bonus -= weights.at(i);
    }
    return bonus;
}

//** The Minimax Recursive Function (Alpha-Beta + depth + position-weighted terminal scoring)
//* isMaximizing = true  -> AI's turn (try to get highest score)
//* isMaximizing = false -> Player's turn (assume Player plays optimally to get lowest score)
//* depth = plies from the root; the *100 step keeps outcome+depth dominant over positionBonus.
//* Pruning: stop exploring a branch once beta <= alpha; selection still matches plain Minimax.
int XO::XOBot::HardMode::minimax(std::array<char, 9> &board, bool isMaximizing, int alpha, int beta, int depth)
{
    if (isWinner(board, AI))
        return (10 - depth) * 100 + positionBonus(board);

    if (isWinner(board, PLAYER))
        return (depth - 10) * 100 + positionBonus(board);

    if (isBoardFull(board))
        return positionBonus(board);

    if (isMaximizing)
    {
        auto bestScore = -10000;
        for (auto i{0uz}; i < board.size(); i++)
        {
            if (board.at(i) == ' ')
            {
                board.at(i) = AI;                                                                  // Try to Make move
                bestScore = std::max(bestScore, minimax(board, false, alpha, beta, depth + 1));    // Recurse
                board.at(i) = ' ';                                                                 // Undo

                alpha = std::max(alpha, bestScore);
                if (beta <= alpha)
                    break; // Beta cutoff: minimizer would avoid this branch
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
                bestScore = std::min(bestScore, minimax(board, true, alpha, beta, depth + 1));
                board.at(i) = ' ';

                beta = std::min(beta, bestScore);
                if (beta <= alpha)
                    break; // Alpha cutoff: maximizer would avoid this branch
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
            int score = minimax(TestBoard, false, -10000, 10000, 1);
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