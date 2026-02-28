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

        // Random moves
        void EasyMode();

        // Block/Win logic with some random
        void MediumMode();

        // Nested class to encapsulate complex Hard Mode logic (Minimax)
        class HardMode
        {
        private:
            TicTacToe *game;
            char AI;
            char PLAYER;
            bool isWinner(const std::array<char, 9> &board, const char &player) const;
            bool isBoardFull(const std::array<char, 9> &board) const;

            // Minimax algorithm
            // Returns a score: +10 (AI wins), -10 (Player wins), 0 (Tie)
            int minimax(std::array<char, 9> &board, bool isMaximizing);
            int findBestMove();

        public:
            HardMode(TicTacToe *API, char ai, char human) : game(API), AI(ai), PLAYER(human) {}
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
