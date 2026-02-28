#include "TicTacToe.h"
#include "Utils.h"
#include <iostream>
#include <limits>
#include <array>
#include <memory>

XO::TicTacToe::TicTacToe() : mode(random_model()), playerIcon('X'), computerIcon('O'), running(true), PlayerWins(0u), ComputerWins(0u), Ties(0u)
{
    // Create player and bot objects, passing the current game instance
    ptrPlayer = std::make_unique<player>(this);
    ptrBot = std::make_unique<XOBot>(this);
}

XO::TicTacToe::~TicTacToe() { std::cout << "Tic Tac Toe has Delate successfully" << std::endl; }

XO::Difficulty XO::TicTacToe::random_model()
{
    auto random = std::rand() % 3;
    return static_cast<Difficulty>(random);
}

// Main Game Loop
void XO::TicTacToe::runGame()
{
    while (true) // Main loop
    {
        std::system("cls");
        mode = random_model();
        resetBoard();
        running = true;

        auto count = 0u;
        auto status = ' ';

        // match loop
        while (running)
        {
            drawBoard();

            ptrPlayer->Move();
            count++;

            if (count >= 3)
            {
                status = checkGameStatus();
                if (status != ' ')
                {
                    running = false;
                    continue;
                }
            }

            ptrBot->Move();
            count++;

            if (count >= 3)
            {
                status = checkGameStatus();
                if (status != ' ')
                {
                    running = false;
                    continue;
                }
            }
        }
        updateScore(status);
        drawBoard();
        Show_status(status);
        if (!PlayAgain())
            break;
    }
}

void XO::TicTacToe::displayScore()
{
    std::cout << Utils::Color::GREEN << "Player (X): " << Utils::Color::RESET << PlayerWins << Utils::Color::MAGENTA << "  |  Computer (O): " << Utils::Color::RESET << ComputerWins << Utils::Color::YELLOW << " | Ties: " << Utils::Color::RESET << Ties << std::endl;
    std::cout << Utils::Color::CYAN << " | Mode: ";
    switch (mode)
    {
    case Difficulty::Easy:
        std::cout << "Easy";
        break;
    case Difficulty::Medium:
        std::cout << "Medium";
        break;
    case Difficulty::Hard:
        std::cout << "Hard";
        break;
    }
    std::cout << Utils::Color::RESET << std::endl;
}

void XO::TicTacToe::drawBoard()
{
    std::cout << std::endl;
    displayScore();
    std::cout << "==== XO XO XO====";
    std::cout << std::endl;
    std::cout << "     |     |     " << std::endl;
    std::cout << "  " << spaces.at(0) << "  |  " << spaces.at(1) << "  |  " << spaces.at(2) << "  " << std::endl;
    std::cout << "_____|_____|_____" << std::endl;
    std::cout << "     |     |     " << std::endl;
    std::cout << "  " << spaces.at(3) << "  |  " << spaces.at(4) << "  |  " << spaces.at(5) << "  " << std::endl;
    std::cout << "_____|_____|_____" << std::endl;
    std::cout << "     |     |     " << std::endl;
    std::cout << "  " << spaces.at(6) << "  |  " << spaces.at(7) << "  |  " << spaces.at(8) << "  " << std::endl;
    std::cout << "     |     |     " << std::endl;
    std::cout << "==== XO XO XO====";
    std::cout << std::endl;
}

void XO::TicTacToe::resetBoard()
{
    // for (auto& i : spaces)
    //     i = ' ';
    spaces.fill(' ');
}

char XO::TicTacToe::checkGameStatus()
{
    /*
    std::array<std::array<3>,8> wins = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
        {0, 3, 6},
        {1, 4, 7},
        {2, 5, 8},
        {0, 4, 8},
        {2, 4, 6}
    };
    */
    // check rows and columns
    for (auto i = 0u; i < 3u; i++)
    {
        auto rStart = i * 3u;

        // Checks [0,1,2], [3,4,5], [6,7,8]
        if (spaces.at(rStart) != ' ' && spaces.at(rStart) == spaces.at(rStart + 1) && spaces[rStart] == spaces.at(rStart + 2))
            return spaces.at(rStart);

        // Checks [0,3,6], [1,4,7], [2,5,8]
        if (spaces.at(i) != ' ' && spaces.at(i) == spaces.at(i + 3) && spaces.at(i) == spaces.at(i + 6))
            return spaces.at(i);
    }

    // [0,4,8]
    if (spaces.at(0) != ' ' && spaces.at(0) == spaces.at(4) && spaces.at(4) == spaces.at(8))
        return spaces.at(0);

    //[2,4,6]
    if (spaces.at(2) != ' ' && spaces.at(2) == spaces.at(4) && spaces.at(4) == spaces.at(6))
        return spaces.at(2);

    // empty spaces left
    for (const auto &i : spaces)
        if (i == ' ')
            return ' ';
    // Tie
    return 'T';
}

void XO::TicTacToe::updateScore(char status)
{
    if (status == playerIcon)
        PlayerWins++;
    else if (status == computerIcon)
        ComputerWins++;
    else if (status == 'T')
        Ties++;
}

bool XO::TicTacToe::PlayAgain()
{
    char playagain;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer
    do
    {
        std::cout << "Play again?(y/n): ";
        std::cin >> playagain;
        playagain = std::tolower(playagain);

    } while (playagain != 'y' && playagain != 'n');

    return (playagain == 'y');
}

void XO::TicTacToe::Show_status(char status)
{
    if (status == playerIcon)
        std::cout << Utils::Color::GREEN << "YOU WIN!" << Utils::Color::RESET << std::endl;
    else if (status == computerIcon)
        std::cout << Utils::Color::RED << "YOU LOSE!" << Utils::Color::RESET << std::endl;
    else if (status == 'T') // Tie
        std::cout << Utils::Color::YELLOW << "IT'S A TIE!" << Utils::Color::RESET << std::endl;
}