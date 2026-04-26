# Tic Tac Toe AI (C++)

A terminal-based Tic Tac Toe game implemented in modern C++ with multiple AI difficulty levels, including a Minimax-powered hard mode.

---

## Features

* 3 Difficulty Levels

  * Easy → Random moves
  * Medium → Basic strategy (win/block + randomness)
  * Hard → Minimax AI (optimal play)

* AI System

  * Smart move evaluation
  * Recursive Minimax algorithm
  * Designed for future Alpha-Beta pruning

* Terminal UI

  * Clean board rendering
  * Colored output using ANSI escape codes

* Score Tracking

  * Player wins
  * AI wins
  * Ties

---

## Project Structure

```
.
├── main.cpp
├── TicTacToe.h / .cpp     # Core game engine
├── Player.h / .cpp        # Human input handling
├── XOBot.h / .cpp         # AI logic (Easy / Medium / Hard)
├── Utils.h                # Terminal utilities (colors)
```

---

## Build & Run

### Compile

```bash
g++ *.cpp -o gamehub -std=c++23 -Wall -Wextra -lstdc++exp
```

### Run

```bash
./gamehub
```

---

## AI Overview

### Easy Mode

* Completely random moves
* No strategy

### Medium Mode

* Attempts to win if possible
* Blocks opponent
* Otherwise plays randomly

### Hard Mode (Minimax)

* Explores all possible board states
* Assigns scores:

  * +10 → AI wins
  * -10 → Player wins
  * 0 → Tie
* Always selects the optimal move

---

## Example Gameplay

```
Player (X): 0  |  Computer (O): 0 | Ties: 0
==== XO XO XO====
  X  |  O  |     
_____|_____|_____
     |  X  |     
_____|_____|_____
  O  |     |     
==== XO XO XO====
Enter number (1-9) to place your 'X':
```

---

## Future Improvements

- [x] Alpha-Beta pruning (performance optimization)

~~- [ ] GUI version using SFML~~
- [ ] Test function $1/3$
  - [ ] Easy mode
  - [ ] Medium mode
  - [x] Hard mode
- [x] Execution time measurement with `<chrono>` (HardMode `findBestMove`)
  - Standalone benchmark: `execution-time.cpp`.
  - Two `HardMode` bots (X vs O) play a full game; every `findBestMove()` call is timed with `std::chrono::steady_clock` (`startTime` / `endTime` / `elapsedTime` in nanoseconds).
  - The match is repeated 20 times — perfect-play vs perfect-play is deterministic, so the move sequence is identical across games and each move position gets 20 timing samples.
  - Reports per-move-position avg / min / max plus an overall average across all 180 calls.
  - Build: `g++ execution-time.cpp XOBot.cpp TicTacToe.cpp Player.cpp -o execution-time -std=c++23 -Wall -Wextra -lstdc++exp`

---

## Author

* Developed by: [Pattaradanai Amazutsumi]
* Discord: [haszan._]
* Email: [natoodxd@gmail.com] or [2511311215@tni.ac.th]
* GitHub: [@hanzanwow]
* Field: Computer & AI Engineering

---

## Project BackGround
Created during my **1st year, 2nd semester** as a Computer Engineer student,as a project submission for the CPE-113 course.
<br>
This project demostrates my understanding of basic AI algorithms and OOP.

---

## What I Learned
- Implementing Minimax Algorithm from scratch
- Object-Oriented Desing in Modern C++/23
- Game state management and AI decision-making
- Lambda function
- Smart pointers
- Basic API

---

## License

This project is released into the public domain under The Unlicense.  
See the LICENSE file for details.
