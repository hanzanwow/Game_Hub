#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <memory>
#include <array>
#include "XOBot.h"
#include "Player.h"

namespace XO
{
    class XOBot;
    class player;
    // game difficulty levels
    enum class Difficulty
    {
        Easy,
        Medium,
        Hard
    };

    class TicTacToe
    {
    private:
        // Smart pointers for Player and Bot management
        // instance of "new & delete"
        std::unique_ptr<player> ptrPlayer;
        std::unique_ptr<XOBot> ptrBot;

        Difficulty mode;
        std::array<char, 9> spaces = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
        const char playerIcon = 'X';
        const char computerIcon = 'O';
        bool running = true;

        int PlayerWins = 0;
        int ComputerWins = 0;
        int Ties = 0;

        Difficulty random_model();
        void drawBoard();
        void resetBoard();
        char checkGameStatus();
        bool PlayAgain();
        void updateScore(char status);
        void displayScore();
        void ShowStatus(char status);

    public:
        /*
        #########==================================================#########
        #########-----Application Programming Interface (API)------#########
        #########==================================================#########
            API for Player and Bot to interact with the game board
        */

        // place a move
        bool placeMove(size_t index, char icon)
        {
            if (index < 9uz && spaces[index] == ' ')
            {
                spaces.at(index) = icon;
                return true;
            }

            return false;
        }

        // Getters game icons
        char getPlayerIcon() const { return playerIcon; }
        char getComputerIcon() const { return computerIcon; }

        // Returns character at board position
        char getPositionAt(size_t index) const { return spaces.at(index); }

        // Return board array
        auto &getMap() const { return spaces; }

        // Returns game mode
        auto &getMode() const { return mode; }

        TicTacToe();
        ~TicTacToe();

        void runGame();
    };
}
#endif
