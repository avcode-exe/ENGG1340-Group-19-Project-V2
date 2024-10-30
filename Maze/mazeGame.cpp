#include <iostream>
#include <fstream>
#include <string>
#include <ncurses.h>
#include <vector>

using namespace std;

/**
 * @brief Displays the maze map on the screen with the player's position.
 *
 * This function takes the maze map and displays it on the screen within the specified screen size.
 * It also marks the player's position on the map with the character 'P'.
 *
 * @param mazemap A vector of strings representing the maze map.
 * @param screenSizeX The width of the screen in characters.
 * @param screenSizeY The height of the screen in characters.
 * @param linepointer The starting line index to display from the maze map.
 * @param playerPosY The Y-coordinate (row) of the player's position in the maze.
 * @param playerPosX The X-coordinate (column) of the player's position in the maze.
 */
void displayMap(vector<string>& mazemap, int screenSizeX, int screenSizeY, int linepointer, int playerPosY, int playerPosX) {
    move(0, 0);
    int i = 0;
    for (const string& mazeStrip : mazemap) {
        if (i < (screenSizeY + linepointer) && i >= linepointer) {
            string displayStrip = mazeStrip;
            if (i == playerPosY) {
                displayStrip[playerPosX] = 'P';
            }
            printw("%s\n", displayStrip.c_str());
        }
        i++;
    }
    refresh();
}

/**
 * @brief Entry point of the maze game application.
 * 
 * This function initializes the ncurses library, reads the maze configuration
 * from a file, and handles user input to navigate through the maze. The player
 * can move up, down, left, and right using the 'w', 's', 'a', and 'd' keys
 * respectively. The game continues until the user presses 'x' or the player
 * reaches the end of the maze.
 * 
 * @return int Returns 1 if the maze configuration file is not found, otherwise returns 0.
 */
int main() {
    ifstream mazefile{".gameConfig/maze.txt"};

    if (!mazefile) {
        cerr << "Maze map not found!" << endl;
        return 1;
    }

    // Store mazefile (since fstream only allows sequential access)
    vector<string> mazemap;
    string mazeStrip; // A line of maze
    while (getline(mazefile, mazeStrip)) {
        mazemap.push_back(mazeStrip);
    }

    int playerPosX = 1;
    int playerPosY = 0;
    int screenSizeY;
    int screenSizeX;
    // Normal startup procedure for ncurse library
    initscr();
    cbreak();
    noecho();

    char usrInput{};
    int linepointer = 0;

    while (usrInput != 'x') {
        move(0, 0); // Ensure cursor is at the top-left corner
        getmaxyx(stdscr, screenSizeY, screenSizeX);
        if (screenSizeX < 56) {
            clear();
            printw("Screen width of 56 required\n");
            printw("Current width: %d\n", screenSizeX);
            refresh();
            continue;
        }

        switch (usrInput) {
            case 'w':
                // if 'P' is not already on the upper half of screen (AND it is not at the beginning)
                if (playerPosY - linepointer < screenSizeY / 2 && linepointer > 0) {
                    linepointer--; // Move viewpoint up
                }
                if (playerPosY > 0 && mazemap[playerPosY - 1][playerPosX] != '#') {
                    playerPosY--;
                }
                break;
            case 's':
                // if 'P' is not already on the lower half of screen (AND the last line of maze is not displayed)
                if (playerPosY > screenSizeY / 2 + linepointer && linepointer + screenSizeY < mazemap.size()) {
                    linepointer++; // Move viewpoint down
                }
                if (mazemap[playerPosY + 1][playerPosX] != '#') {
                    playerPosY++;
                }
                break;
            case 'a':
                if (mazemap[playerPosY][playerPosX - 1] != '#') {
                    playerPosX--;
                }
                break;
            case 'd':
                if (mazemap[playerPosY][playerPosX + 1] != '#') {
                    playerPosX++;
                }
                break;
        }
        displayMap(mazemap, screenSizeX, screenSizeY, linepointer, playerPosY, playerPosX);
        refresh();
        if (playerPosY + 1 >= mazemap.size()) {
            break;
        }
        usrInput = getch();
    }
    endwin();
}
