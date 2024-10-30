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

If you don't have `make` or `g++` installed:
* `sudo apt install build-essential gdb`
* `sudo apt install make`

Install depedencies:
* `sudo apt install libncurses5-dev libncursesw5-dev`

Clone the repository:
1. `git clone https://github.com/avcode-exe/ENGG1340-Group-19-Project-V2.git`
2. `cd ENGG1340-Group-19-Project-V2`
3. `make run`

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

After running `make run` to start the game, you will be prompted whether a new maze should be generated or not:
- "y": A new maze will be generated
- "n": The previous saved maze will be used

You will begin at the starting position of the maze.
Navigate in the maze using W, A, S, and D keys. The number of health points remaining is shown on the right with "HP". Try to reach the goal position without losing all health points.
If you get hit by a monster, you will be sent to a game of minesweeper.

In the minesweeper game, you can navigate in the game board using W, A, S, and D keys.
To reveal a tile, press the spacebar. If a mine, denoted with "X", is revealed, you lose the minesweeper game and will return to the maze with one health point deducted. Otherwise, the minesweeper game continues.

For example, the following shows that a mine is found:
```
###
#X#
###
```

If a number is shown instead of a mine, the number represents how many mines are nearby in a 3x3 range with the number at the center of the range.

For example, the following snippet shows that there are 2 mines within this 3x3 range:

```
###
#2#
###
```

If an empty cell is revealed instead of a number, it indicates that there are no mines near this tile.

For example, the following snippet shows that there are no mines within this 3x3 range:

```
###
# #
###
```

Apart from revealing tiles, you can also toggle flag mode using the F key. To place a flag on a tile, make sure that flag mode is on, navigate to a tile and press spacebar. The letter "F" will show up on tiles with a flag placed on them.

Keep in mind that you cannot choose tiles that are already revealed. However, you can choose tiles that have a flag placed on them because they have not been revealed yet.

Have fun!