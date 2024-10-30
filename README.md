# ENGG1340-Group-19-Project-V2

This repository is the extended version of the following repository: https://github.com/avcode-exe/ENGG1340-Group-19-Project

# Old repository info:

HKU ENGG1340/COMP2113 (2023 - 2024) Group 19 Project

## Groupmates

- Liu Hong Yuan Tom (3036262135)
- Wong Yiu Wing Wilson (3036262032)
- Ho Ho Cheung Sky (3036136934)
- Chan Ho Yin David (3036035075)
- Chan Pak Yin Perry (3036140284)

## About Our Project

Our project consists of a maze game and a minesweeper game.

To begin with, the player has to complete the maze while avoiding monsters nearby.
However, if the player gets hit by the monster, the player will be sent to a game of minesweeper.
If the player loses during the minesweeper game, a health point will be deducted and the player returns to the maze game.

The goal is to reach the exit of the maze before the player loses all health points.

# New repository info:

## Compilation and execution instructions

1. If you don't have `make` or `g++` installed:
	* `sudo apt install build-essential gdb`
	* `sudo apt install make`

2. **Install dependencies:**
	* `sudo apt install libncurses5-dev libncursesw5-dev`

3. **Clone the repository:**
	* `git clone https://github.com/avcode-exe/ENGG1340-Group-19-Project-V2.git`

4. **Compile and run the game:**
	* `cd ENGG1340-Group-19-Project-V2`
	* `make run` or `make` + `./mazeGame`

## List of Features

- A maze generated using recursive randomized Prim's algorithm
	- A frontier is randomly selected from the list of potential frontiers using `<random>`. The algorithm then removes the wall between the original coordinates and the selected frontier and thus carve a path.
	- STL vectors are used to initialize and store frontiers and their possible directions. It is also used for the `potentialFrontiers` array, a list of frontiers the algorithm can choose from randomly.
	- Dynamic pointers are used to instantiate objects of struct Cell for the Depth-First Search (DFS) algorithm, which finds the path from a starting position to the goal position in the maze.
	- The maze generated is also saved to a text file using `<fstream>`. The content of the maze in the text file is then loaded to `mazeGame.cpp` and printed on the screen.
	- Different source code and header files are compiled separately in a Makefile.

- There are monsters moving in various locations of the maze and checkpoints generated in random locations
	- MT19937 is used for the random generation of a random device, which is then used to shuffle the list of potential monster positions.
	- STL vectors and pairs are used to store a list of potential monster positions, with the first value in the pair container being the x-coordinate and the second value being the y-coordinate.
	- Monsters are represented with the character 'M' and are saved to the text file along with the maze.
	- Different source code and header files are compiled separately in a Makefile.

- A game of minesweeper commences when the player is hit by the monster
	- `rand()` from `<cstdlib>` is used to generate random coordinates for mines.
	- STL vectors are used to store a list of coordinates that have been visited by the player.
	- The minesweeper game board is saved in a text file using `<fstream>`, which is then loaded to `minesweeper.cpp` and printed on the screen.
	- Different source code and header files are compiled separately in a Makefile.

## List of non-standard C/C++ libraries

- ncurses

## Gameplay Tutorial

### Starting the Game

1. **Run the Game**: After compiling the game using `make run`, you will be prompted to choose whether to generate a new maze or use the previously saved maze:
   - Enter `y` to generate a new maze.
   - Enter `n` to use the previously saved maze.

2. **Initial Setup**: You will start at the initial position of the maze with a certain number of health points (HP).

### Navigating the Maze

- **Movement**: Use the following keys to navigate through the maze:
  - `W`: Move up
  - `A`: Move left
  - `S`: Move down
  - `D`: Move right

- **Objective**: The goal is to reach the exit of the maze without losing all your health points.

- **Monsters**: Monsters are represented by the character `M` and move dynamically within the maze. Avoid them to prevent losing health points.

### Minesweeper Game

If you get hit by a monster, you will be sent to a game of minesweeper:

1. **Movement**: Use the following keys to navigate through the minesweeper board:
   - `W`: Move up
   - `A`: Move left
   - `S`: Move down
   - `D`: Move right

2. **Revealing Tiles**: Press the `spacebar` to reveal a tile:
   - If a mine (denoted by `X`) is revealed, you lose the minesweeper game and return to the maze with one health point deducted.
   - If a number is revealed, it indicates how many mines are nearby within a 3 by 3 range.
   - If an empty cell is revealed, it indicates no mines are nearby.

3. **Flag Mode**: Press `F` to toggle flag mode. In flag mode, press the `spacebar` to place a flag on a tile, marking it as a potential mine.

### Winning and Losing

- **Winning**: Reach the exit of the maze to win the game.
- **Losing**: If you lose all your health points, the game is over.

### Saving and Resuming the Game

- **Save Game**: The game state, including the player's position, health points, and maze configuration will be saved to a test file when exiting the game via `x` or `X`.
- **Resume Game**: When starting the game, choose `n` to resume from the last saved state.

### Additional Tips

- **Health Points**: Keep an eye on your health points displayed on the right side of the screen.
- **Avoid Monsters**: Plan your moves to avoid monsters and minimize health loss.

Have fun and good luck navigating the maze and playing minesweeper!

## License

This project is licensed under the Apache-2.0 License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Thanks to the **HKU ENGG1340/COMP2113** course instructors and TAs for their guidance.
- Special thanks to **all group members** for their contributions and hard work.