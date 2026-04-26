//* Benchmark for XO::XOBot::HardMode::findBestMove() during a full AI-vs-AI match.
//* Two HardMode bots (X vs O) play a complete game; every findBestMove() call is timed.
//* The whole game is repeated GAMES times so each move position has GAMES samples to
//* average. The match is deterministic (perfect play vs perfect play), so the move
//* sequence is identical across games — only the timings vary run-to-run.
//*
//* Build: g++ execution-time.cpp XOBot.cpp TicTacToe.cpp Player.cpp -o execution-time -std=c++23 -Wall -Wextra -lstdc++exp
//* Run:   ./execution-time

#include "XOBot.h"
#include "TicTacToe.h"
#include "Utils.h"

#include <array>
#include <chrono>
#include <cstdio>
#include <print>
#include <vector>

#ifdef _WIN32
    #include <io.h>
    #define BENCH_DUP    _dup
    #define BENCH_DUP2   _dup2
    #define BENCH_FILENO _fileno
    #define BENCH_CLOSE  _close
    static constexpr const char *NULL_DEVICE = "nul";
#else
    #include <unistd.h>
    #define BENCH_DUP    dup
    #define BENCH_DUP2   dup2
    #define BENCH_FILENO fileno
    #define BENCH_CLOSE  close
    static constexpr const char *NULL_DEVICE = "/dev/null";
#endif

//* Friend hook declared in XOBot.h; exposes the private bits we need from HardMode.
//* findBestMove() reads board state via game->getMap(), so a real TicTacToe is required.
struct HardModeProbe
{
    XO::XOBot::HardMode hm;

    HardModeProbe(XO::TicTacToe *game, char ai, char human)
        : hm(game, ai, human) {}

    unsigned long long findBestMove() { return hm.findBestMove(); }
    bool isWinner(const std::array<char, 9> &b, char p) const { return hm.isWinner(b, p); }
    bool isBoardFull(const std::array<char, 9> &b) const { return hm.isBoardFull(b); }
};

constexpr int GAMES = 20;

struct MoveRecord
{
    char turn;        //* 'X' or 'O' — which bot moved
    int slot;         //* 0..8
    long long ns;     //* findBestMove() duration for this move (game #1 only)
};

int main()
{
    //* Mute stdout/stderr around findBestMove() so its per-slot debug println doesn't
    //* dominate the timing or pollute the summary. Restore both before reporting.
    std::fflush(stdout);
    std::fflush(stderr);
    int savedOut = BENCH_DUP(BENCH_FILENO(stdout));
    int savedErr = BENCH_DUP(BENCH_FILENO(stderr));
    std::freopen(NULL_DEVICE, "w", stdout);
    std::freopen(NULL_DEVICE, "w", stderr);

    //* per_move_ns[i] = all timings recorded for the i-th move (across all GAMES).
    std::array<std::vector<long long>, 9> per_move_ns;

    //* Captured from game #1 only — perfect play vs perfect play repeats the same
    //* moves every game, so one capture is enough to show the sequence.
    std::vector<MoveRecord> sample_game;

    char winnerChar = ' ';   //* set on first game; same outcome every game

    for (int g = 0; g < GAMES; ++g)
    {
        XO::TicTacToe game;                          //* fresh empty board per game
        HardModeProbe bot_X(&game, 'X', 'O');        //* X is AI from this bot's POV
        HardModeProbe bot_O(&game, 'O', 'X');        //* O is AI from this bot's POV

        char turn = 'X';                             //* X moves first by convention
        int move_idx = 0;

        while (true)
        {
            auto &bot = (turn == 'X') ? bot_X : bot_O;

            auto startTime = std::chrono::steady_clock::now();
            auto slot = bot.findBestMove();
            auto endTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
            long long ns = elapsedTime.count();

            per_move_ns[move_idx].push_back(ns);
            if (g == 0)
                sample_game.push_back({turn, static_cast<int>(slot), ns});

            game.placeMove(static_cast<size_t>(slot), turn);
            ++move_idx;

            auto board = game.getMap();
            if (bot_X.isWinner(board, turn))
            {
                if (g == 0) winnerChar = turn;
                break;
            }
            if (bot_X.isBoardFull(board))
            {
                if (g == 0) winnerChar = '-';        //* tie marker
                break;
            }

            turn = (turn == 'X') ? 'O' : 'X';
        }
    }

    //* Restore real stdout/stderr.
    std::fflush(stdout);
    std::fflush(stderr);
    BENCH_DUP2(savedOut, BENCH_FILENO(stdout));
    BENCH_DUP2(savedErr, BENCH_FILENO(stderr));
    BENCH_CLOSE(savedOut);
    BENCH_CLOSE(savedErr);
    std::clearerr(stdout);
    std::clearerr(stderr);

    using C = Utils::Color;

    std::println("{}=== HardMode vs HardMode benchmark ({} games) ==={}",
                 C::CYAN, GAMES, C::RESET);

    const char *resultStr = (winnerChar == '-')
        ? "tie"
        : ((winnerChar == 'X') ? "X wins" : "O wins");
    std::println("Result: {} ({} moves)", resultStr, sample_game.size());

    std::println("\n{}Sample game move sequence (game #1):{}", C::CYAN, C::RESET);
    for (size_t i = 0; i < sample_game.size(); ++i)
    {
        const auto &m = sample_game[i];
        std::println("  Move {:>2}: {} -> slot {}  ({:>10} ns)",
                     i + 1, m.turn, m.slot + 1, m.ns);
    }

    std::println("\n{}Per-move-position timings (avg / min / max across {} games):{}",
                 C::CYAN, GAMES, C::RESET);
    long long grand_sum = 0;
    long long grand_count = 0;
    for (size_t i = 0; i < per_move_ns.size(); ++i)
    {
        const auto &v = per_move_ns[i];
        if (v.empty()) continue;

        long long sum = 0;
        long long mn = v.front();
        long long mx = v.front();
        for (auto x : v)
        {
            sum += x;
            if (x < mn) mn = x;
            if (x > mx) mx = x;
        }
        long long avg = sum / static_cast<long long>(v.size());
        grand_sum += sum;
        grand_count += static_cast<long long>(v.size());

        std::println("  Move {:>2}: avg {:>10} ns ({:>6.3f} ms) | min {:>10} ns | max {:>10} ns",
                     i + 1, avg, avg / 1'000'000.0, mn, mx);
    }

    long long grand_avg = grand_sum / grand_count;
    std::println("{}-----------------------------------------------------{}", C::CYAN, C::RESET);
    std::println("{}Overall average over {} calls : {:>10} ns ({:.3f} ms){}",
                 C::GREEN, grand_count, grand_avg, grand_avg / 1'000'000.0, C::RESET);

    return 0;
}
