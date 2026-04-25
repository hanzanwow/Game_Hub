#ifndef XOBOT_H
#define XOBOT_H

#include <array>

namespace XO
{
    class TicTacToe;
    class XOBot
    {
    private:
        TicTacToe *game;
        char PLAYER;
        char AI;

        //* EASY MODE:
        // - Random moves only
        // - No strategy
        void EasyMode();

        //* MEDIUM MODE:
        // - Attempts to win
        // - Block opponent
        // - Otherwise falls back to random (persent change 50%)
        void MediumMode();

        //* HARD MODE AI:
        // - Uses Minimax algorithm
        // - Evaluates all possible board moves
        // - Chooses optimal move
        class HardMode
        {
        private:
            TicTacToe *game;

            char AI;
            char PLAYER;

            bool isWinner(const std::array<char, 9> &board, const char &player) const;
            bool isBoardFull(const std::array<char, 9> &board) const;

            //* Position bonus (terminal-only tie-breaker).
            //  Sum of weights of AI cells minus weights of Player cells.
            //  Each cell's weight = number of winning lines through it: center=4, corners=3, edges=2.
            int positionBonus(const std::array<char, 9> &board) const;

            //* Minimax algorithm (recursive) with Alpha-Beta pruning.
            //  Returns a layered terminal score: outcome dominates, then depth, then position.
            //* AI win:     (10 - depth) * 100 + positionBonus  -> faster wins score higher
            //* Player win: (depth - 10) * 100 + positionBonus  -> slower losses score higher
            //* Tie:        positionBonus                       -> ties broken by central control
            //* alpha = best score the maximizer can guarantee so far
            //* beta  = best score the minimizer can guarantee so far
            int minimax(std::array<char, 9ull> &board, bool isMaximizing, int alpha, int beta, int depth);
            unsigned long long int findBestMove();

        public:
            HardMode(TicTacToe *API, char aiChar, char humanChar) : game(API), AI(aiChar), PLAYER(humanChar) {}
            ~HardMode() = default;
            void Hard_Move();
        };

    public:
        HardMode hard;
        XOBot(TicTacToe *API);
        void Move();
    };
}
#endif
