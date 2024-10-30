#include "mazeGame.h"
#include "minesweeper.h"
#include <climits>
#include <future>
#include <ncurses.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <fstream>
#include <sys/stat.h>

using namespace std;

unordered_map<int, pair<int, int>> monsterDirections;

/**
 * @brief Checks if a given position in the maze is free to move to.
 *
 * This function determines if the specified coordinates (y, x) in the maze
 * are within bounds and not blocked by a wall ('#'). Additionally, it ensures
 * that the position (0, 1) is not considered free.
 *
 * @param mazemap A reference to a vector of strings representing the maze.
 * @param y The y-coordinate (row) to check.
 * @param x The x-coordinate (column) to check.
 * @return true if the position is within bounds, not a wall, and not (0, 1); false otherwise.
 */
bool isFree(const vector<string> &mazemap, int y, int x) {
    return y >= 0 && y < mazemap.size() && x >= 0 && x < mazemap[0].size() && mazemap[y][x] != '#' && (y != 0 || x != 1);
}

/**
 * @brief Moves the monster in the maze based on its current direction.
 * 
 * This function updates the position of a monster in the maze. If the monster
 * encounters an obstacle, it changes its direction to avoid the obstacle.
 * 
 * @param mazemap A reference to the maze represented as a vector of strings.
 * @param monsterPos A reference to the current position of the monster as a pair of integers (row, column).
 * @param monsterIndex The index of the monster in the monsterDirections vector.
 * 
 * The function first calculates the new position of the monster based on its
 * current direction. If the new position is not free (i.e., it is an obstacle),
 * the function attempts to change the monster's direction to either left or right
 * if it was moving vertically, or reverses its direction if it was moving horizontally.
 * After updating the direction, it recalculates the new position. If the new position
 * is free, it swaps the monster's current position with the new position in the maze.
 */
void moveMonster(vector<string> &mazemap, pair<int, int> &monsterPos, int monsterIndex) {
    pair<int, int> &direction = monsterDirections[monsterIndex];
    pair<int, int> &pos = monsterPos;

    int newY = pos.first + direction.first;
    int newX = pos.second + direction.second;

    if (!isFree(mazemap, newY, newX)) {
        if (direction.first != 0) {
            if (isFree(mazemap, pos.first, pos.second - 1)) {
                direction = make_pair(0, -1);
            } else if (isFree(mazemap, pos.first, pos.second + 1)) {
                direction = make_pair(0, 1);
            } else {
                direction.first = -direction.first;
            }
        } else {
            direction.second = -direction.second;
        }
        newY = pos.first + direction.first;
        newX = pos.second + direction.second;
    }

    if (isFree(mazemap, newY, newX)) {
        swap(mazemap[pos.first][pos.second], mazemap[newY][newX]);
        pos.first = newY;
        pos.second = newX;
    }
}

/**
 * @brief Moves all monsters in the maze and checks if any monster has reached the player's position.
 * 
 * This function iterates through the list of monster positions, moves each monster by calling the 
 * moveMonster function, and checks if any monster's new position matches the player's position.
 * 
 * @param mazemap A reference to a vector of strings representing the maze layout.
 * @param monsterPositions A reference to a vector of pairs of integers representing the positions of the monsters.
 * @param playerPos A pair of integers representing the player's position.
 * @return true if any monster reaches the player's position, false otherwise.
 */
bool moveMonsters(vector<string> &mazemap, vector<pair<int, int>> &monsterPositions, pair<int, int> playerPos) {
    for (int i = 0; i < monsterPositions.size(); ++i) {
        moveMonster(mazemap, monsterPositions[i], i);
        if (monsterPositions[i] == playerPos) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Displays the current state of the maze map on the screen.
 *
 * This function takes the original maze map and updates it to include the player's
 * position and the positions of the monsters. It then prints the updated map to the screen.
 *
 * @param originalMazemap A vector of strings representing the original maze map.
 * @param screenSizeY The height of the screen in lines.
 * @param linepointer The current line pointer indicating the top line of the visible screen.
 * @param playerPosY The Y-coordinate of the player's position in the maze.
 * @param playerPosX The X-coordinate of the player's position in the maze.
 * @param monsterPositions A vector of pairs representing the positions of the monsters in the maze.
 * @param playerHP The current health points of the player.
 */
void displayMap(const vector<string> &originalMazemap, int screenSizeY, int linepointer, int playerPosY, int playerPosX, const vector<pair<int, int>> &monsterPositions, int playerHP) {
    vector<string> mazemap = originalMazemap;

    if (playerPosY >= linepointer && playerPosY < linepointer + screenSizeY) {
        mazemap[playerPosY][playerPosX] = 'P';
    }

    for (const auto &monsterPos : monsterPositions) {
        if (monsterPos.first >= linepointer && monsterPos.first < linepointer + screenSizeY) {
            mazemap[monsterPos.first][monsterPos.second] = 'M';
        }
    }

    move(0, 0);
    int i = 0;
    for (string mazeStrip : mazemap) {
        if (i < (screenSizeY + linepointer) && i >= linepointer) {
            for (int j = 0; j < mazeStrip.length(); j++) {
                if (i == playerPosY && j == playerPosX) {
                    attron(COLOR_PAIR(2));
                    printw("P ");
                    attroff(COLOR_PAIR(2));
                } else if (mazeStrip[j] == '#') {
                    printw("##");
                } else if (mazeStrip[j] == 'M') {
                    attron(COLOR_PAIR(8));
                    printw("M ");
                    attroff(COLOR_PAIR(8));
                } else if (mazeStrip[j] == 'C') {
                    attron(COLOR_PAIR(2));
                    printw("C ");
                    attroff(COLOR_PAIR(2));
                } else {
                    printw("  ");
                }
            }
            printw("\n");
        }
        i++;
    }
    mvprintw(screenSizeY / 2, mazemap.size() + 60, "HP: %d", playerHP);
    refresh();
}

/**
 * @brief Finds the nearest checkpoint to the player's current position.
 *
 * This function iterates through a list of checkpoints and calculates the Manhattan distance
 * from the player's current position to each checkpoint. It returns the checkpoint with the
 * smallest distance.
 *
 * @param checkpoints A vector of pairs representing the coordinates (y, x) of the checkpoints.
 * @param playerPosY The y-coordinate of the player's current position.
 * @param playerPosX The x-coordinate of the player's current position.
 * @return A pair of integers representing the coordinates (y, x) of the nearest checkpoint.
 */
pair<int, int> findNearestCheckpoint(const vector<pair<int, int>> &checkpoints, int playerPosY, int playerPosX) {
    pair<int, int> nearestCheckpoint;
    int minDistance = INT_MAX;

    for (const auto &checkpoint : checkpoints) {
        int distance = abs(checkpoint.first - playerPosY) + abs(checkpoint.second - playerPosX);
        if (distance < minDistance) {
            minDistance = distance;
            nearestCheckpoint = checkpoint;
        }
    }

    return nearestCheckpoint;
}

/**
 * @brief Stores the current status of the player to a file.
 *
 * This function writes the player's current position (Y and X coordinates),
 * health points (HP), and a line pointer to a status file located at
 * ".gameConfig/status.txt". If the file cannot be opened, an error message
 * is printed to the standard error output.
 *
 * @param playerPosY The Y coordinate of the player's position.
 * @param playerPosX The X coordinate of the player's position.
 * @param playerHP The current health points of the player.
 * @param linepointer An integer representing the line pointer.
 */
void storeStatus(int playerPosY, int playerPosX, int playerHP, int linepointer) {
    ofstream statusfile(".gameConfig/status.txt");
    if (statusfile.fail()) {
        cerr << "Error opening status file" << endl;
        return;
    }
    statusfile << playerPosY << " " << playerPosX << " " << playerHP << " " << linepointer;
    statusfile.close();
}

/**
 * @brief Creates an empty directory and files for game configuration.
 *
 * This function creates a directory named ".gameConfig" and three empty files within it:
 * - maze.txt
 * - minefield.txt
 * - status.txt
 *
 * After creating the files, it opens the status.txt file and writes initial game status values:
 * - 0 (initial value)
 * - 1 (initial value)
 * - 5 (initial value)
 * - 0 (initial value)
 *
 * If the status.txt file cannot be opened, an error message is printed to the standard error output.
 */
void createEmptyFiles() {
    const int dir_err = mkdir(".gameConfig", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    std::ofstream file1(".gameConfig/maze.txt");
    std::ofstream file2(".gameConfig/minefield.txt");
    std::ofstream file3(".gameConfig/status.txt");

    file3.close();

    ofstream statusfile(".gameConfig/status.txt");
    if (statusfile.fail()) {
        cerr << "Error opening status file" << endl;
        return;
    }
    statusfile << 0 << " " << 1 << " " << 5 << " " << 0;
    statusfile.close();

    file1.close();
    file2.close();
}

/**
 * @brief Main function for the Maze game.
 * 
 * This function initializes the game, checks for existing game configuration,
 * and either starts a new game or resumes a saved game. It handles player input,
 * updates the game state, and manages the game loop. The game involves navigating
 * a maze, avoiding monsters, and reaching the end to win.
 * 
 * @return int Returns 0 on successful execution, 1 if there are errors in loading
 *         necessary game files.
 */
int main() {
    bool folderExists = false;
    struct stat st;
    if (stat(".gameConfig", &st) == 0) {
        if (st.st_mode & S_IFDIR != 0) {
            folderExists = true;
        }
    }
    if (!folderExists) {
        createEmptyFiles();
    }
    char newGame;
    bool gameRunning = true;
    bool win = false;
    int playerPosX;
    int playerPosY;
    int playerHP;
    int linepointer;
    cout << "New game? ('n' for resume game) [y/n]: ";
    cin >> newGame;
    if (newGame == 'y') {
        playerPosX = 1;
        playerPosY = 0;
        playerHP = 5;
        linepointer = 0;
        Maze maze_temp;
        float monsterDensity = 0.5f;
        maze_temp.generateMaze(maze_temp.startX, maze_temp.startY);
        Cell *path = findPath();
        maze_temp.placeCheckpoints();
        maze_temp.placeMonsters(monsterDensity, path);
        maze_temp.saveMaze();
        deletePath(path);
    } else {
        ifstream statusFile(".gameConfig/status.txt");
        if (!statusFile) {
            cerr << "Error: status.txt not found!" << endl;
            return 1;
        }
        statusFile >> playerPosY >> playerPosX >> playerHP >> linepointer;
        statusFile.close();
    }

    ifstream mazefile{".gameConfig/maze.txt"};
    if (!mazefile) {
        cerr << "Maze map not found!" << endl;
        return 1;
    }

    srand(static_cast<unsigned>(time(nullptr)));

    vector<string> mazemap;
    string mazeStrip;
    while (getline(mazefile, mazeStrip)) {
        mazemap.push_back(mazeStrip);
    }

    int screenSizeY;
    int screenSizeX;
    initscr();
    cbreak();
    noecho();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_RED);
    init_pair(4, COLOR_BLACK, COLOR_WHITE);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_RED, COLOR_BLACK);
    init_pair(8, COLOR_GREEN, COLOR_BLACK);

    char usrInput{};

    vector<pair<int, int>> monsterPositions;
    for (int y = 0; y < mazemap.size(); ++y) {
        for (int x = 0; x < mazemap[y].size(); ++x) {
            if (mazemap[y][x] == 'M') {
                monsterPositions.emplace_back(y, x);
            }
        }
    }

    for (int i = 0; i < monsterPositions.size(); ++i) {
        int y = monsterPositions[i].first;
        int x = monsterPositions[i].second;
        if (mazemap[max(0, y - 1)][x] == '#' || mazemap[min((int)mazemap.size() - 1, y + 1)][x] == '#') {
            monsterDirections[i] = (rand() % 2 == 0) ? make_pair(0, -1) : make_pair(0, 1);
        } else {
            monsterDirections[i] = (rand() % 2 == 0) ? make_pair(-1, 0) : make_pair(1, 0);
        }
    }

    vector<pair<int, int>> checkpointPositions;
    for (int y = 0; y < mazemap.size(); ++y) {
        for (int x = 0; x < mazemap[y].size(); ++x) {
            if (mazemap[y][x] == 'C') {
                checkpointPositions.emplace_back(y, x);
            }
        }
    }

    int msR;
    bool msPause = false;
    std::thread monsterThread([&]() {
        while (gameRunning) {
            if (!msPause && moveMonsters(mazemap, monsterPositions, make_pair(playerPosY, playerPosX))) {
                msPause = true;
                clear();
                refresh();
                msR = minesweeper();
                if (msR != 0) {
                    auto nearestCheckpoint = findNearestCheckpoint(
                        checkpointPositions, playerPosY, playerPosX);
                    playerPosY = nearestCheckpoint.first;
                    playerPosX = nearestCheckpoint.second;
                    playerHP--;
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
                if (playerHP <= 0) {
                    refresh();
                    clear();
                    getmaxyx(stdscr, screenSizeY, screenSizeX);
                    int startX = (screenSizeX - 10) / 2;
                    int startY = screenSizeY / 2;
                    mvprintw(startY, startX, "Game over!");
                    refresh();
                    this_thread::sleep_for(std::chrono::seconds(2));
                    gameRunning = false;
                    msPause = false;
                    break;
                }
                msPause = false;
                clear();
                refresh();
                if (msR == 0) {
                    std::this_thread::sleep_for(std::chrono::seconds(6));
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });

    nodelay(stdscr, TRUE);

    do {
        while (msPause) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        move(0, 0);
        getmaxyx(stdscr, screenSizeY, screenSizeX);
        if (screenSizeX < 120) {
            clear();
            printw("Screen width of 120 required\n");
            printw("Current width: %d\n", screenSizeX);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            refresh();
            continue;
        }
        switch (usrInput) {
            case 'w':
                if (playerPosY - linepointer < screenSizeY / 2 && linepointer > 0) {
                    linepointer--;
                }
                if (playerPosY > 0 && mazemap[playerPosY - 1][playerPosX] != '#') {
                    playerPosY--;
                }
                break;
            case 's':
                if (playerPosY > screenSizeY / 2 + linepointer &&
                    linepointer + screenSizeY < mazemap.size()) {
                    linepointer++;
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
        displayMap(mazemap, screenSizeY, linepointer, playerPosY, playerPosX, monsterPositions, playerHP);
        refresh();
        if (playerPosY + 1 >= mazemap.size()) {
            win = true;
            break;
        }
        usrInput = getch();
        if (usrInput == ERR) {
            continue;
        }
    } while (usrInput != 'x' && usrInput != 'X' && gameRunning);
    gameRunning = false;
    if (win) {
        getmaxyx(stdscr, screenSizeY, screenSizeX);
        clear();
        int startX = (screenSizeX - 10) / 2;
        int startY = screenSizeY / 2;
        mvprintw(startY, startX, "You Win!");
        refresh();
        this_thread::sleep_for(std::chrono::seconds(2));
    }
    if (usrInput == 'x' || usrInput == 'X') {
        storeStatus(playerPosY, playerPosX, playerHP, linepointer);
    }

    monsterThread.join();
    endwin();
    return 0;
}
