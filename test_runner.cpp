//* Unit tests for XO::XOBot::HardMode (Minimax + helpers).
//* Build: g++ test.cpp XOBot.cpp -o test_runner -std=c++23 -Wall -Wextra -lstdc++exp
//* Run:   ./test_runner

#include "XOBot.h"
#include "Utils.h"
#include <array>
#include <cstdlib>
#include <print>
#include <string>

//* Friend hook declared in XOBot.h; exposes private HardMode methods to tests.
//* HardMode never dereferences `game` from these helpers, so passing nullptr is safe.
struct HardModeProbe
{
    XO::XOBot::HardMode hm{nullptr, 'O', 'X'};

    bool isWinner(const std::array<char, 9> &b, char p) const { return hm.isWinner(b, p); }
    bool isBoardFull(const std::array<char, 9> &b) const { return hm.isBoardFull(b); }
    int positionBonus(const std::array<char, 9> &b) const { return hm.positionBonus(b); }
    int minimax(std::array<char, 9> &b, bool isMax, int alpha, int beta, int depth)
    {
        return hm.minimax(b, isMax, alpha, beta, depth);
    }
};

namespace
{
    int passed = 0;
    int failed = 0;

    using C = Utils::Color;

    void report(bool cond, const std::string &name, int line)
    {
        if (cond)
        {
            ++passed;
            std::println("  {}PASS{}  {}", C::GREEN, C::RESET, name);
        }
        else
        {
            ++failed;
            std::println("  {}FAIL{}  {} {}(line {}){}",
                         C::RED, C::RESET, name, C::YELLOW, line, C::RESET);
        }
    }

    void section(const char *title)
    {
        std::println("\n{}== {} =={}", C::CYAN, title, C::RESET);
    }
}

#define CHECK(cond) report((cond), #cond, __LINE__)
#define CHECK_EQ(actual, expected)                                            \
    do                                                                        \
    {                                                                         \
        auto _a = (actual);                                                   \
        auto _e = (expected);                                                 \
        report(_a == _e,                                                      \
               std::string(#actual " == " #expected " [got ") +               \
                   std::to_string(_a) + ", want " + std::to_string(_e) + "]", \
               __LINE__);                                                     \
    } while (0)

constexpr char A = 'O'; //* AI
constexpr char P = 'X'; //* Player
constexpr char _ = ' ';

//* The 8 winning lines, mirrored from XOBot.cpp (rows, cols, diagonals).
constexpr std::array<std::array<int, 3>, 8> LINES = {{
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
    {0, 4, 8}, {2, 4, 6},
}};

void test_isWinner(HardModeProbe &t)
{
    section("isWinner: every line wins for AI");
    for (const auto &line : LINES)
    {
        std::array<char, 9> b{_, _, _, _, _, _, _, _, _};
        for (auto i : line)
            b[i] = A;
        CHECK(t.isWinner(b, A));
        CHECK(!t.isWinner(b, P));
    }

    section("isWinner: every line wins for PLAYER");
    for (const auto &line : LINES)
    {
        std::array<char, 9> b{_, _, _, _, _, _, _, _, _};
        for (auto i : line)
            b[i] = P;
        CHECK(t.isWinner(b, P));
        CHECK(!t.isWinner(b, A));
    }

    section("isWinner: negative cases");
    {
        //* Two-in-a-row should not register as win.
        std::array<char, 9> b{A, A, _, _, _, _, _, _, _};
        CHECK(!t.isWinner(b, A));
    }
    {
        //* Line broken by opponent.
        std::array<char, 9> b{A, A, P, _, _, _, _, _, _};
        CHECK(!t.isWinner(b, A));
        CHECK(!t.isWinner(b, P));
    }
    {
        //* Tie position — full board, neither wins.
        std::array<char, 9> b{P, A, P, P, A, A, A, P, P};
        CHECK(!t.isWinner(b, A));
        CHECK(!t.isWinner(b, P));
    }
    {
        //* Diagonal of opponents (extreme): three Ps along anti-diagonal, AI never wins.
        std::array<char, 9> b{_, _, P, _, P, _, P, _, _};
        CHECK(t.isWinner(b, P));
        CHECK(!t.isWinner(b, A));
    }
}

void test_isBoardFull(HardModeProbe &t)
{
    section("isBoardFull");
    {
        std::array<char, 9> empty{_, _, _, _, _, _, _, _, _};
        CHECK(!t.isBoardFull(empty));
    }
    {
        std::array<char, 9> one{A, _, _, _, _, _, _, _, _};
        CHECK(!t.isBoardFull(one));
    }
    {
        //* 8 of 9 filled — single empty.
        std::array<char, 9> almost{A, P, A, P, A, P, P, A, _};
        CHECK(!t.isBoardFull(almost));
    }
    {
        //* Empty cell at index 0 only.
        std::array<char, 9> almost0{_, P, A, P, A, P, P, A, A};
        CHECK(!t.isBoardFull(almost0));
    }
    {
        //* Fully filled — tie composition.
        std::array<char, 9> full{A, P, A, P, A, P, P, A, P};
        CHECK(t.isBoardFull(full));
    }
    {
        //* Fully filled with a winner — still considered "full".
        std::array<char, 9> fullWin{A, A, A, P, P, A, P, A, P};
        CHECK(t.isBoardFull(fullWin));
    }
}

void test_positionBonus(HardModeProbe &t)
{
    section("positionBonus");
    //* Weights from XOBot.cpp: {3,2,3, 2,4,2, 3,2,3}.
    constexpr std::array<int, 9> W = {3, 2, 3, 2, 4, 2, 3, 2, 3};

    {
        std::array<char, 9> empty{_, _, _, _, _, _, _, _, _};
        CHECK_EQ(t.positionBonus(empty), 0);
    }

    //* Each cell, AI alone -> +W[i]
    for (size_t i = 0; i < 9; ++i)
    {
        std::array<char, 9> b{_, _, _, _, _, _, _, _, _};
        b[i] = A;
        CHECK_EQ(t.positionBonus(b), W[i]);
    }

    //* Each cell, PLAYER alone -> -W[i]
    for (size_t i = 0; i < 9; ++i)
    {
        std::array<char, 9> b{_, _, _, _, _, _, _, _, _};
        b[i] = P;
        CHECK_EQ(t.positionBonus(b), -W[i]);
    }

    {
        //* Mirror-symmetric position should net to 0.
        //*  X | _ | O
        //*  _ | _ | _
        //*  O | _ | X
        std::array<char, 9> b{P, _, A, _, _, _, A, _, P};
        CHECK_EQ(t.positionBonus(b), 0);
    }

    {
        //* Extreme: every cell AI -> sum of all weights = 24.
        std::array<char, 9> all_ai{A, A, A, A, A, A, A, A, A};
        CHECK_EQ(t.positionBonus(all_ai), 24);
    }
    {
        //* Extreme: every cell PLAYER -> -24.
        std::array<char, 9> all_p{P, P, P, P, P, P, P, P, P};
        CHECK_EQ(t.positionBonus(all_p), -24);
    }
    {
        //* AI center vs Player corner -> 4 - 3 = 1.
        std::array<char, 9> b{P, _, _, _, A, _, _, _, _};
        CHECK_EQ(t.positionBonus(b), 1);
    }
}

void test_minimax_terminal(HardModeProbe &t)
{
    section("minimax: terminal-leaf scoring");
    {
        //* AI already won at depth=1 -> (10-1)*100 + bonus(8) = 908.
        std::array<char, 9> b{A, A, A, _, _, _, _, _, _};
        CHECK_EQ(t.minimax(b, false, -10000, 10000, 1), 908);
    }
    {
        //* AI already won at depth=5 -> (10-5)*100 + 8 = 508.
        std::array<char, 9> b{A, A, A, _, _, _, _, _, _};
        CHECK_EQ(t.minimax(b, false, -10000, 10000, 5), 508);
    }
    {
        //* PLAYER won at depth=1 -> (1-10)*100 + (-8) = -908.
        std::array<char, 9> b{P, P, P, _, _, _, _, _, _};
        CHECK_EQ(t.minimax(b, true, -10000, 10000, 1), -908);
    }
    {
        //* Tie / full board -> positionBonus only.
        //*  X | O | X
        //*  X | O | O
        //*  O | X | X
        //* AI {1,4,5,6} = 2+4+2+3 = 11; HU {0,2,3,7,8} = 3+3+2+2+3 = 13; bonus = -2.
        std::array<char, 9> b{P, A, P, P, A, A, A, P, P};
        CHECK_EQ(t.minimax(b, true, -10000, 10000, 1), -2);
        CHECK_EQ(t.minimax(b, false, -10000, 10000, 1), -2);
    }
}

void test_minimax_forced(HardModeProbe &t)
{
    section("minimax: one-ply forced outcomes");
    {
        //* AI to move; AI plays index 2 to win.
        //* After: AI {0,1,2}, HU {3,4}; bonus = (3+2+3)-(2+4) = 2.
        //* Score = (10-2)*100 + 2 = 802.
        std::array<char, 9> b{A, A, _, P, P, _, _, _, _};
        CHECK_EQ(t.minimax(b, true, -10000, 10000, 1), 802);
    }
    {
        //* PLAYER to move; PLAYER plays index 2 to win.
        //* After: AI {3,4}, HU {0,1,2}; bonus = (2+4)-(3+2+3) = -2.
        //* Score = (2-10)*100 + (-2) = -802.
        std::array<char, 9> b{P, P, _, A, A, _, _, _, _};
        CHECK_EQ(t.minimax(b, false, -10000, 10000, 1), -802);
    }
    {
        //* AI must win on the diagonal (plays 8): AI {0,4} -> add 8 -> {0,4,8}.
        //* bonus after: AI {0,4,8} = 3+4+3 = 10; HU {} = 0; bonus = 10.
        //* Score = (10-2)*100 + 10 = 810.
        std::array<char, 9> b{A, _, _, _, A, _, _, _, _};
        CHECK_EQ(t.minimax(b, true, -10000, 10000, 1), 810);
    }
}

void test_minimax_block_and_tie(HardModeProbe &t)
{
    section("minimax: AI must block / no immediate loss");
    {
        //* PLAYER threatens row 0; AI must block at index 2.
        //* After perfect play this position is a tie or AI advantage; check `s > -700`
        //* (no forced loss-in-1).
        std::array<char, 9> b{P, P, _, _, A, _, _, _, _};
        int s = t.minimax(b, true, -10000, 10000, 1);
        CHECK(s > -700);
    }
    {
        //* Symmetric: AI threatens row 0; PLAYER must block. Score should not be a forced AI win.
        std::array<char, 9> b{A, A, _, _, P, _, _, _, _};
        int s = t.minimax(b, false, -10000, 10000, 1);
        CHECK(s < 700);
    }
    {
        //* Empty board, AI to move. Tic-tac-toe with perfect play is a tie -> outcome is 0.
        //* Final score is positionBonus only, bounded |s| < 100.
        std::array<char, 9> b{_, _, _, _, _, _, _, _, _};
        int s = t.minimax(b, true, -10000, 10000, 1);
        CHECK(std::abs(s) < 100);
    }
    {
        //* Empty board, PLAYER to move. Same: tie outcome.
        std::array<char, 9> b{_, _, _, _, _, _, _, _, _};
        int s = t.minimax(b, false, -10000, 10000, 1);
        CHECK(std::abs(s) < 100);
    }
}

void test_minimax_alphabeta_consistency(HardModeProbe &t)
{
    section("minimax: alpha-beta produces same result as full window");
    //* For any position, narrowing the alpha-beta window must not change the chosen score
    //* as long as the true value is inside the window. We verify by running a few positions
    //* with the default wide window vs a narrower (but still safe) window.
    struct Case
    {
        std::array<char, 9> b;
        bool isMax;
        int depth;
    };
    Case cases[] = {
        {{A, A, _, P, P, _, _, _, _}, true, 1},
        {{P, P, _, A, A, _, _, _, _}, false, 1},
        {{A, _, _, _, A, _, _, _, _}, true, 1},
        {{_, _, _, _, A, _, _, _, _}, false, 2},
        {{P, _, _, _, A, _, _, _, _}, true, 2},
    };

    for (const auto &c : cases)
    {
        auto b1 = c.b;
        auto b2 = c.b;
        int wide = t.minimax(b1, c.isMax, -10000, 10000, c.depth);
        int tight = t.minimax(b2, c.isMax, -1000, 1000, c.depth);
        CHECK_EQ(tight, wide);
    }
}

int main()
{
    HardModeProbe t;

    test_isWinner(t);
    test_isBoardFull(t);
    test_positionBonus(t);
    test_minimax_terminal(t);
    test_minimax_forced(t);
    test_minimax_block_and_tie(t);
    test_minimax_alphabeta_consistency(t);

    using C = Utils::Color;
    const char *banner = (failed == 0) ? C::GREEN : C::RED;
    std::println("\n{}================================={}", banner, C::RESET);
    std::println("Results: {}{} passed{}, {}{} failed{}.",
                 C::GREEN, passed, C::RESET,
                 (failed == 0 ? C::GREEN : C::RED), failed, C::RESET);
    std::println("{}================================={}", banner, C::RESET);
    return failed == 0 ? 0 : 1;
}
