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

            //* Minimax algorithm (recursive).
            //  Evaluates board states and returns a score:
            //* +10 (AI win), -10 (Player win), 0 (Tie)
            int minimax(std::array<char, 9ull> &board, bool isMaximizing);
            unsigned long long int findBestMove();

        public:
            HardMode(TicTacToe *API, char aiChar, char humanChar) : game(API), AI(aiChar), PLAYER(humanChar) {}
            ~HardMode() = default;
            void Hard_Move();
        };

    public:
        HardMode hard;
        XOBot(TicTacToe *API) : game(API), PLAYER(API->getPlayerIcon()), AI(API->getComputerIcon()), hard(API, AI, PLAYER) {}
        void Move();
    };
}
#endif
