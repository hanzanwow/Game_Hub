#include "TicTacToe.h"
#include "Player.h"
#include "Utils.h"
#include <iostream>
#include <print>
#include <limits>

void XO::player::Move()
{
    int number;

    while (true)
    {

        std::print("Enter number (1-9) to place your 'X': ");
        std::cin >> number;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::println(std::cerr, "{}Invalid input. Please enter a number.{}", Utils::Color::RED, Utils::Color::RESET);

            continue;
        }

        if (number < 1 || number > 9)
        {
            std::println(std::cerr, "{}Please enter a number between 1-9.{}", Utils::Color::YELLOW, Utils::Color::RESET);
            continue;
        }

        // Adjust input to 0-based index (User enters 1-9, Array uses 0-8)
        auto index = static_cast<size_t>(number - 1);

        // Try placing the move via the game API
        if (game->placeMove(index, game->getPlayerIcon()))
            break;
        else
            std::println(std::cerr, "{}Invalid move or spot taken.{}", Utils::Color::YELLOW, Utils::Color::RESET);
    }
}