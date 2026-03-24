#include "TicTacToe.h"
#include "Utils.h"
#include <print>
#include <iostream>
#include <limits>
#include <array>
#include <memory>

XO::TicTacToe::TicTacToe() : mode(random_model())
{
    // Create player and bot objects, passing the current game instance
    ptrPlayer = std::make_unique<player>(this);
    ptrBot = std::make_unique<XOBot>(this);
}

XO::TicTacToe::~TicTacToe() { std::println(std::clog, "Tic Tac Toe has Delate successfully"); }

XO::Difficulty XO::TicTacToe::random_model()
{
    auto random = std::rand() % 3;

    return static_cast<Difficulty>(random);
}

// Main game loop: handles multiple matches until user exits
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

        // Single match loop (runs until win/tie)
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
        ShowStatus(status);

        if (!PlayAgain())
            break;
    }
}

void XO::TicTacToe::displayScore()
{
    std::println("{}Player (X): {}{}  |  {}Computer (O): {}{} | {}Ties: {}{}",
                 Utils::Color::GREEN, Utils::Color::RESET, PlayerWins,
                 Utils::Color::MAGENTA, Utils::Color::RESET, ComputerWins,
                 Utils::Color::YELLOW, Utils::Color::RESET, Ties);

    std::print("{} | Mode: ", Utils::Color::CYAN);
    switch (mode)
    {
    case Difficulty::Easy:
        std::print("Easy");
        break;

    case Difficulty::Medium:
        std::print("Medium");
        break;

    case Difficulty::Hard:
        std::print("Hard");
        break;
    }

    std::println("{}", Utils::Color::RESET);
}

void XO::TicTacToe::drawBoard()
{
    std::println("");

    displayScore();
    std::println("==== XO XO XO====");
    std::println("     |     |     ");
    std::println("  {}  |  {}  |  {}  ", spaces.at(0ull), spaces.at(1ull), spaces.at(2ull));
    std::println("_____|_____|_____");
    std::println("     |     |     ");
    std::println("  {}  |  {}  |  {}  ", spaces.at(3ull), spaces.at(4ull), spaces.at(5ull));
    std::println("_____|_____|_____");
    std::println("     |     |     ");
    std::println("  {}  |  {}  |  {}  ", spaces.at(6ull), spaces.at(7ull), spaces.at(8ull));
    std::println("     |     |     ");
    std::println("==== XO XO XO====");
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
    for (auto i{0uz}; i < 3; i++)
    {
        auto rStart = i * 3;

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

    // Check if there are still empty spaces (game not finished)
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
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

    char playagain;

    do
    {
        std::print("Play again?(y/n): ");
        std::cin >> playagain;

        playagain = std::tolower(playagain);

    } while (playagain != 'y' && playagain != 'n');

    return (playagain == 'y');
}

void XO::TicTacToe::ShowStatus(char status)
{
    if (status == playerIcon)
        std::println("{}YOU WIN!{}", Utils::Color::GREEN, Utils::Color::RESET);

    else if (status == computerIcon)
        std::println("{}YOU LOSE!{}", Utils::Color::RED, Utils::Color::RESET);

    else if (status == 'T') // Tie
        std::println("{}IT'S A TIE!{}", Utils::Color::YELLOW, Utils::Color::RESET);
}