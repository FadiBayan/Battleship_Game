# Battleship Game Project

## Overview
This project is an advanced implementation of the classic board game **Battleship** in C, featuring strategic gameplay elements. It includes a single-player mode with an advanced bot capable of simulating competitive gameplay against the user. The project was developed as a collaborative effort, showcasing teamwork and problem-solving skills.

## Features
- **Single-player Mode**: Play against an AI bot with strategic decision-making.
- **Advanced Bot Logic**: Implements probability-based attack strategies to enhance gameplay difficulty.
- **Interactive Gameplay**: Intuitive UI tools to make the game user-friendly.
- **Modular Design**: Organized with multiple modules to ensure scalability and maintainability.

## Project Structure
```plaintext
BattleShip_Project/
├── src/                # Source files
│   ├── Attacks.c       # Handles attack logic
│   ├── BinomialHeap.c  # Manages data structures for probability calculations
│   ├── Bot.c           # AI bot logic
│   ├── CalcProbs.c     # Probability-based decision-making
│   ├── coordslib.c     # Coordinate handling
│   ├── D_LinkedList.c  # Data structure implementation
│   ├── defs.c          # Definitions and global configurations
│   ├── Driver.c        # Main game driver
│   ├── InputLib.c      # Input handling functions
│   ├── Player.c        # Player logic and interactions
│   ├── ShipPlacement.c # Ship placement logic
│   ├── ShortcutFuncs.c # Helper functions
│   ├── UITools.c       # UI tools for user interaction
├── include/            # Header files
├── bin/                # Output directory for the compiled executable
├── Makefile            # Build automation script
└── README.md           # Project documentation (this file)
```

## Prerequisites
- GCC Compiler (MinGW or similar on Windows, GCC on Linux/Mac)
- `make` utility (optional for build automation)

## Installation
1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd BattleShip_Project
   ```
2. Build the project:
   ```bash
   make
   ```
3. Run the game:
   ```bash
   ./bin/main
   ```

## Gameplay Instructions
1. Place your ships on the board following the prompts.
2. Take turns attacking coordinates on the bot's board.
3. The game ends when all ships on one side are destroyed.
4. Enjoy the strategic challenge!

## Build Details
The `Makefile` automates the build process:
- **Compile and Link**:
  ```makefile
  $(OUTPUT): $(SRCs)
      gcc -I$(INC) -o $@ $^
  ```
- **Clean Up**:
  ```makefile
  clean:
      del /Q $(OUTPUT)
