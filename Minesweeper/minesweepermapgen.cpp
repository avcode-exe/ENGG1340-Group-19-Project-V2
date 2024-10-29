#include "minewseepergen.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

/**
 * @brief Generate a minesweeper game board
 * @param size Size of the game board
 * @param mines Number of mines
 */
void minefieldgen(int size, int mines, char cord[][MSIZE]);

/**
 * @brief Generate a random number for coordinates
 * @param size Size of the game board
 */
int randcord(int size);

/**
 * @brief Check if the selected set of coordinates contains a mine
 * @param x x-coordinate
 * @param y y-coordinate
 * @param size Size of the game board
 */
bool checkismine(int x, int y, int size, char cord[][MSIZE]);

/**
 * @brief Count the number of mines
 * @param x x-coordinate
 * @param y y-coordinate
 */
int countmine(int x, int y, int size, char cord[][MSIZE]);

/**
 * @brief Generate a random number for coordinates
 * @param size Size of the game board
 */
int randcord(int size) {
    return rand() % size;
}

/**
 * @brief Check if the selected set of coordinates contains a mine
 * @param x x-coordinate
 * @param y y-coordinate
 * @param size Size of the game board
 */
bool checkismine(int x, int y, int size, char cord[][MSIZE]) {
    if (x < 0 || x >= size || y < 0 || y >= size) {
        return false;
	}
    return cord[x][y] == 'X';
}

/**
 * @brief Count the number of mines
 * @param x x-coordinate
 * @param y y-coordinate
 * @param size Size of the game board
 * @param cord 2D array representing the game board
 */
int countmine(int x, int y, int size, char cord[][MSIZE]) {
    int mines = 0;

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) {
				continue;
			}
            if (checkismine(x + i, y + j, size, cord)) {
                mines += 1;
			}
        }
    }

    return mines;
}

/**
 * @brief Generate a minesweeper game board
 * @param size Size of the game board
 * @param mines Number of mines
 * @param cord 2D array representing the game board
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

void minewsweepergenmain() {
    srand(time(NULL));  // Initialize rand()
    char cord[MSIZE][MSIZE];

    minefieldgen(MSIZE, MINES, cord);
}