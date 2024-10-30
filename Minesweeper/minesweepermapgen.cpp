#include "minewseepergen.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

// void minefieldgen(int size, int mines, char cord[][MSIZE]);
// int randcord(int size);
// bool checkismine(int x, int y, int size, char cord[][MSIZE]);
// int countmine(int x, int y, int size, char cord[][MSIZE]);

/**
 * @brief Generates a random coordinate within the given size.
 * 
 * This function returns a random integer between 0 (inclusive) and the specified size (exclusive).
 * It uses the standard library function rand() to generate the random number.
 * 
 * @param size The upper limit (exclusive) for the random coordinate.
 * @return int A random integer between 0 and size-1.
 */
int randcord(int size) {
    return rand() % size;
}

/**
 * @brief Counts the number of mines surrounding a given cell in a Minesweeper grid.
 *
 * This function checks the eight neighboring cells around the specified cell (x, y)
 * in the Minesweeper grid and counts how many of them contain a mine ('X').
 *
 * @param x The x-coordinate of the cell to check.
 * @param y The y-coordinate of the cell to check.
 * @param size The size of the Minesweeper grid.
 * @param cord A 2D array representing the Minesweeper grid, where 'X' denotes a mine.
 * @return The number of mines surrounding the specified cell.
 */
int countmine(int x, int y, int size, char cord[][MSIZE]) {
    int mines = 0;

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) {
                continue;
            }
            int newX = x + i;
            int newY = y + j;
            if (newX >= 0 && newX < size && newY >= 0 && newY < size && cord[newX][newY] == 'X') {
                mines += 1;
            }
        }
    }

    return mines;
}

/**
 * @brief Generates a minesweeper minefield and writes it to a file.
 *
 * This function initializes a minefield of given size, randomly places a specified number of mines,
 * calculates the number of adjacent mines for each cell, and writes the resulting minefield to a file.
 *
 * @param size The size of the minefield (size x size).
 * @param mines The number of mines to place in the minefield.
 * @param cord A 2D array representing the minefield.
 *
 * The function performs the following steps:
 * 1. Initializes all cells in the minefield to empty (' ').
 * 2. Randomly places the specified number of mines ('X') in the minefield.
 * 3. For each non-mine cell, calculates the number of adjacent mines and sets the cell to that number.
 * 4. Writes the resulting minefield to a file named ".gameConfig/minefield.txt".
 *
 * Note:
 * - The function assumes that the size of the 2D array `cord` is at least `size x size`.
 * - The function uses a helper function `randcord(int)` to generate random coordinates.
 * - The function uses a helper function `countmine(int, int, int, char[][MSIZE])` to count adjacent mines.
 */
void minefieldgen(int size, int mines, char cord[][MSIZE]) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            cord[i][j] = ' ';
        }
    }

    for (int m = 0; m < mines; ++m) {
        int x = randcord(MSIZE);
        int y = randcord(MSIZE);

        if (cord[x][y] == ' ' && x != 0 && y != 0) {
            cord[x][y] = 'X';
        } else {
            --m;
        }
    }

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (cord[i][j] != 'X') {
                int nummine = countmine(i, j, size, cord);
                if (nummine > 0)
                    cord[i][j] = '0' + nummine;
            }
        }
    }

    ofstream fout(".gameConfig/minefield.txt");
    if (fout.fail()) {
        cout << "error opening minefield.txt" << endl;
        exit(1);
    }

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            fout << cord[i][j];
        }
        fout << endl;
    }

    fout.close();
}

/**
 * @brief Main function to generate a Minesweeper game map.
 * 
 * This function initializes the random number generator with the current time,
 * creates a 2D array to represent the Minesweeper grid, and calls the 
 * minefieldgen function to populate the grid with mines.
 */
void minewsweepergenmain() {
    srand(time(NULL));
    char cord[MSIZE][MSIZE];

    minefieldgen(MSIZE, MINES, cord);
}