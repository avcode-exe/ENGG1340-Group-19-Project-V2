#include "minewseepergen.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ncurses.h>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/**
 * @brief Load the minesweeper game board from minefield.txt
 */
void loadMinefield(char cord[][MSIZE]) {
    string minefield_stream;

    ifstream fin;
    noskipws(fin);
    fin.open(".gameConfig/minefield.txt");

    if (fin.fail()) {
        cout << "error opening minefield.txt" << endl;
        exit(1);
    };

    char newline;
    for (int i = 0; i < MSIZE; i++) {
        for (int j = 0; j < MSIZE; j++) {
            fin >> cord[i][j];
        }
        fin >> newline;
    };

    fin.close();
}

/**
 * @brief Display the game board
 * @param posX x-coordinate
 * @param posY y-coordinate
 * @param flagMode If the player places a flag or not
 * @param message Error message to be printed
 */
void display(char cord[][MSIZE], int posX, int posY, bool flagMode, string message) {
    move(0, 0);
    for (int i = 0; i < MSIZE; i++) {
        for (int j = 0; j < MSIZE; j++) {
            if (cord[i][j] == 'X') {
                attron(COLOR_PAIR(3));
                addch(cord[i][j]);
                attroff(COLOR_PAIR(3));
            } else if (cord[i][j] == 'F') {
                attron(COLOR_PAIR(4));
                addch(cord[i][j]);
                attroff(COLOR_PAIR(4));
            } else if (i == posY && j == posX) {
                attron(COLOR_PAIR(2));
                addch(cord[i][j]);
                attroff(COLOR_PAIR(2));
            } else {
                attron(COLOR_PAIR(1));
                addch(cord[i][j]);
                attroff(COLOR_PAIR(1));
            }
        };
        addch('\n');
    };
    printw("Current coordinates: %d %d %s", posX, posY,
           flagMode ? "(flag ON)" : "(flag OFF)");
    printw("\n");
    if (message == "")
        printw("Use spacebar to select tile, 'f' to flag");
    else {
        printw("%s", message.c_str());
    }
    refresh();
}

/**
 * @brief Initialize the game board by filling every tile with walls
 */
void initGameBoard(char cord[][MSIZE]) {
    for (int r = 0; r < MSIZE; r++) {
        for (int c = 0; c < MSIZE; c++) {
            cord[r][c] = '#';
        }
    }
}

/**
 * @brief Check if the selected tile has already been revealed
 * @param row Row number/y-coordinate
 * @param col Column number/x-coordinate
 */
bool checkIfRevealed(char cord[][MSIZE], int row, int col) {
    if (cord[row][col] == '#' || cord[row][col] == 'F') {
        return false;
    };
    return true;
}

/**
 * @brief Recursively reveal the selected tile and tiles nearby until they reach empty tiles
 * @param row Row number/y-coordinate
 * @param col Column number/x-coordinate
 * @param visited A list of coordinates that have already been visited by the player
 */
bool revealPos(char cord[][MSIZE], char gameBoard[][MSIZE], int row, int col, vector<vector<int>> &visited) {
    if (std::find(visited.begin(), visited.end(), vector<int>{row, col}) ==
        visited.end()) {
        visited.push_back(vector<int>{row, col});
        gameBoard[row][col] = cord[row][col];
        if (cord[row][col] != ' ') {
            return false;
        }
        for (vector<int> spread : vector<vector<int>>{
			{row - 1, col - 1},
            {row - 1, col},
            {row - 1, col + 1},
            {row, col - 1},
            {row, col + 1},
            {row + 1, col - 1},
            {row + 1, col},
            {row + 1, col + 1}}) {
            if (spread[0] >= 0 && spread[0] < MSIZE && spread[1] >= 0 && spread[1] < MSIZE) {
                revealPos(cord, gameBoard, spread[0], spread[1], visited);
            }
        }
    }
    return false;
}

/**
 * @brief Check if a mine is found in the selected tile
 * @param row Row number/y-coordinate
 * @param col Column number/x-coordinate
 */
bool checkIfMineFound(char cord[][MSIZE], int row, int col) {
    if (cord[row][col] == 'X') {
        return true;
    }
    return false;
}

/**
 * @brief Place a flag on the selected tile
 * @param row Row number/y-coordinate
 * @param col Column number/x-coordinate
 */
void placeFlag(char cord[][MSIZE], int row, int col) {
    if (cord[row][col] == '#') {
        cord[row][col] = 'F';
        return;
    }
    if (cord[row][col] == 'F') {
        cord[row][col] = '#';
        return;
    }
}

/**
 * @brief Check if the player wins the game (i.e. no more tiles are left unrevealed or not flagged)
 */
bool checkIfGameWin(char cord[][MSIZE]) {
    bool gameWin = true;
    for (int r = 0; r < MSIZE; r++) {
        for (int c = 0; c < MSIZE; c++) {
            if (cord[r][c] == '#') {
                gameWin = false;
            }
        }
    };

    return gameWin;
}

int minesweeper() {
    minewsweepergenmain();
    char usrInput{};
    int posX = 0;
    int posY = 0;
    char mf[MSIZE][MSIZE];
    loadMinefield(mf);

    char gameBoard[MSIZE][MSIZE];
    initGameBoard(gameBoard);

    bool gameLose = false;
    string str_in, errorMsg;
    int row_in, col_in;
    bool flagMode;
    bool act;
    while (!checkIfGameWin(gameBoard)) {
        flagMode = false;
        act = false;
        clear();
        refresh();
        while (!act) {
            display(gameBoard, posX, posY, flagMode, errorMsg);
            usrInput = getch();
            switch (usrInput) {
            case 'w':
                if (posY > 0)
                    posY--;
                break;
            case 's':
                if (posY + 1 < MSIZE)
                    posY++;
                break;
            case 'a':
                if (posX > 0)
                    posX--;
                break;
            case 'd':
                if (posX + 1 < MSIZE)
                    posX++;
                break;
            case ' ':
                col_in = posX;
                row_in = posY;
                act = true;
                break;
            case 'f':
                flagMode = !flagMode;
                break;
            }
        }
        if (flagMode) {
            if (checkIfRevealed(gameBoard, row_in, col_in)) {
                errorMsg = "Unable to flag revealed tiles. Please choose another one.";
                continue;
            };
            placeFlag(gameBoard, row_in, col_in);
        } else {
            if (checkIfRevealed(gameBoard, row_in, col_in)) {
                errorMsg = "Tile was already revealed. Please choose another one.";
                continue;
            };
            vector<vector<int>> visited{};
            revealPos(mf, gameBoard, row_in, col_in, visited);

            if (checkIfMineFound(mf, row_in, col_in)) {
                gameLose == true;
                display(gameBoard, posX, posY, flagMode, "GAME OVER!\n You Lose!");
                return -1;
            }
        }
    };

    display(gameBoard, posX, posY, flagMode,
            "Cleared!");
    return 0;
}