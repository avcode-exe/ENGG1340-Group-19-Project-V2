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
 * @brief Loads the minefield configuration from a file into a 2D array.
 *
 * This function reads the minefield configuration from the file located at
 * ".gameConfig/minefield.txt" and populates the provided 2D array `cord` with
 * the minefield data. Each element in the array represents a cell in the minefield.
 *
 * @param cord A 2D array of characters where the minefield configuration will be stored.
 *
 * @note The function assumes that the file ".gameConfig/minefield.txt" exists and is
 *       formatted correctly. If the file cannot be opened, the function will print
 *       an error message and terminate the program.
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
 * @brief Displays the Minesweeper game board and current status.
 *
 * This function renders the Minesweeper game board on the screen using ncurses.
 * It highlights the current position of the cursor, flagged tiles, and mines.
 * Additionally, it displays the current coordinates, flag mode status, and a message.
 *
 * @param cord A 2D array representing the Minesweeper game board.
 * @param posX The current X-coordinate of the cursor.
 * @param posY The current Y-coordinate of the cursor.
 * @param flagMode A boolean indicating whether the flag mode is active.
 * @param message A string containing a message to be displayed.
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
 * @brief Initializes the game board for Minesweeper.
 * 
 * This function sets up the game board by filling each cell with the character '#'.
 * 
 * @param cord A 2D array representing the game board.
 */
void initGameBoard(char cord[][MSIZE]) {
    for (int r = 0; r < MSIZE; r++) {
        for (int c = 0; c < MSIZE; c++) {
            cord[r][c] = '#';
        }
    }
}

/**
 * @brief Checks if a cell in the Minesweeper grid has been revealed.
 * 
 * This function determines whether a specific cell in the Minesweeper grid
 * has been revealed by checking its character value. A cell is considered
 * revealed if it does not contain a '#' (hidden cell) or 'F' (flagged cell).
 * 
 * @param cord The Minesweeper grid represented as a 2D array of characters.
 * @param row The row index of the cell to check.
 * @param col The column index of the cell to check.
 * @return true If the cell is revealed.
 * @return false If the cell is hidden or flagged.
 */
bool checkIfRevealed(char cord[][MSIZE], int row, int col) {
    if (cord[row][col] == '#' || cord[row][col] == 'F') {
        return false;
    };
    return true;
}

/**
 * @brief Reveals the position on the game board and recursively reveals adjacent positions if they are empty.
 * 
 * This function updates the game board by revealing the content of the specified position. If the revealed position
 * is empty (represented by a space character ' '), it recursively reveals all adjacent positions. The function also
 * keeps track of visited positions to avoid infinite recursion.
 * 
 * @param cord The original board containing the actual positions and their values.
 * @param gameBoard The game board that is being revealed to the player.
 * @param row The row index of the position to reveal.
 * @param col The column index of the position to reveal.
 * @param visited A reference to a vector of vectors that keeps track of visited positions to avoid revisiting them.
 * @return true if the position was successfully revealed and it was not a mine, false otherwise.
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
 * @brief Checks if a mine is found at the specified coordinates.
 *
 * This function checks the given 2D array of characters to determine if there is a mine ('X')
 * at the specified row and column.
 *
 * @param cord A 2D array of characters representing the game board.
 * @param row The row index to check.
 * @param col The column index to check.
 * @return true if a mine is found at the specified coordinates, false otherwise.
 */
bool checkIfMineFound(char cord[][MSIZE], int row, int col) {
    if (cord[row][col] == 'X') {
        return true;
    }
    return false;
}

/**
 * @brief Toggles a flag at the specified coordinates on the Minesweeper board.
 *
 * This function places a flag ('F') at the given row and column if the cell is currently unmarked ('#').
 * If the cell already has a flag ('F'), it removes the flag and reverts the cell back to unmarked ('#').
 *
 * @param cord The Minesweeper board represented as a 2D array of characters.
 * @param row The row index of the cell to place or remove the flag.
 * @param col The column index of the cell to place or remove the flag.
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
 * @brief Checks if the game is won by verifying if all cells are revealed.
 *
 * This function iterates through the entire game board and checks if there are any cells
 * that are still hidden (represented by the character '#'). If any hidden cell is found,
 * the game is not won yet.
 *
 * @param cord A 2D array representing the game board.
 * @return true if all cells are revealed and the game is won, false otherwise.
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

/**
 * @brief Main function for the Minesweeper game.
 * 
 * This function initializes the game board and minefield, and handles the main game loop.
 * The player can navigate the game board using 'w', 'a', 's', 'd' keys and can reveal tiles
 * or place flags using the space bar and 'f' key respectively. The game continues until the
 * player either wins by revealing all non-mine tiles or loses by revealing a mine.
 * 
 * @return int Returns 0 if the player wins, and -1 if the player loses.
 */
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

    display(gameBoard, posX, posY, flagMode, "Cleared!");
    return 0;
}