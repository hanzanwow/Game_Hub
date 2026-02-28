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
        const char playerIcon;
        const char computerIcon;
        bool running;

        int PlayerWins;
        int ComputerWins;
        int Ties;

        Difficulty random_model();
        void drawBoard();
        void resetBoard();
        char checkGameStatus();
        bool PlayAgain();
        void updateScore(char status);
        void displayScore();
        void Show_status(char status);

    public:
        /*
        #########==================================================#########
        #########-----Application Programming Interface (API)------#########
        #########==================================================#########
            API for Player and Bot to interact with the game board
        */

        // place a move
        bool placeMove(int index, char icon)
        {
            if (index >= 0 && index < 9 && spaces[index] == ' ')
            {
                spaces[index] = icon;
                return true;
            }
            return false;
        }

        // Getters game icons
        char getPlayerIcon() const { return playerIcon; }
        char getComputerIcon() const { return computerIcon; }

        // Returns character at board position
        char getPositionAt(int index) const { return spaces.at(index); }

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
