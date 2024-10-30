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

void displayMap(const vector<string> &mazemap, int screenSizeY, int linepointer, int playerPosY, int playerPosX, const vector<pair<int, int>> &monsterPositions, int playerHP);
bool moveMonsters(vector<string> &mazemap, vector<pair<int, int>> &monsterPositions, pair<int, int> playerPos);
bool isFree(const vector<string> &mazemap, int y, int x);
void moveMonster(vector<string> &mazemap, pair<int, int> &monsterPos, int monsterIndex);
unordered_map<int, pair<int, int>> monsterDirections;
pair<int, int> findNearestCheckpoint(const vector<pair<int, int>> &checkpoints, int playerPosY, int playerPosX);
void storeStatus(int playerPosY, int playerPosX, int playerHP, int linepointer);
void createEmptyFiles();

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

bool isFree(const vector<string> &mazemap, int y, int x) {
    return y >= 0 && y < mazemap.size() && x >= 0 && x < mazemap[0].size() && mazemap[y][x] != '#' && (y != 0 || x != 1);
}

bool moveMonsters(vector<string> &mazemap, vector<pair<int, int>> &monsterPositions, pair<int, int> playerPos) {
    for (int i = 0; i < monsterPositions.size(); ++i) {
        moveMonster(mazemap, monsterPositions[i], i);
        if (monsterPositions[i] == playerPos) {
            return true;
        }
    }
    return false;
}

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

void storeStatus(int playerPosY, int playerPosX, int playerHP, int linepointer) {
    ofstream statusfile(".gameConfig/status.txt");
    if (statusfile.fail()) {
        cerr << "Error opening status file" << endl;
        return;
    }
    statusfile << playerPosY << " " << playerPosX << " " << playerHP << " " << linepointer;
    statusfile.close();
}

void createEmptyFiles() {
    const int dir_err = mkdir(".gameConfig", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    std::ofstream file1(".gameConfig/maze.txt");
    std::ofstream file2(".gameConfig/minefield.txt");
    std::ofstream file3(".gameConfig/status.txt");

    file3.close();

    // write initial status file
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
