#include "mazeGame.h"
#include "minesweeper.h"
#include <climits>
#include <future>
#include <ncurses.h>
#include <string>
#include <thread>
#include <unordered_map>

void displayMap(const vector<string> &mazemap, int screenSizeY, int linepointer,
                int playerPosY, int playerPosX,
                const vector<pair<int, int>> &monsterPositions, int playerHP);
bool moveMonsters(vector<string> &mazemap,
                  vector<pair<int, int>> &monsterPositions,
                  pair<int, int> playerPos);
bool isFree(const vector<string> &mazemap, int y, int x);
void moveMonster(vector<string> &mazemap, pair<int, int> &monsterPos,
                 int monsterIndex);
unordered_map<int, pair<int, int>> monsterDirections;
pair<int, int> findNearestCheckpoint(const vector<pair<int, int>> &checkpoints,
                                     int playerPosY, int playerPosX);
void storeStatus(int playerPosY, int playerPosX, int playerHP, int linepointer);

int main() {
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
    // Ensure to read using the same format (with spaces as separators)
    statusFile >> playerPosY >> playerPosX >> playerHP >> linepointer;
    statusFile.close();
  }
  ifstream mazefile{".gameConfig/maze.txt"};

  if (!mazefile) {
    cerr << "Maze map not found!" << endl;
    return 1;
  }

  srand(static_cast<unsigned>(time(nullptr)));

  // Store mazefile (since fstream only allows sequencial access)
  vector<string> mazemap;
  string mazeStrip; // A line of maze
  while (getline(mazefile, mazeStrip)) {
    mazemap.push_back(mazeStrip);
  }

  int screenSizeY;
  int screenSizeX;
  // Normal startup procedue for ncurse libraryT
  initscr();
  cbreak();
  noecho();
  start_color();
  // init_color(6, 255, 165, 0);
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  init_pair(3, COLOR_WHITE, COLOR_RED);
  init_pair(4, COLOR_BLACK, COLOR_WHITE);
  init_pair(6, COLOR_CYAN, COLOR_BLACK);
  init_pair(7, COLOR_RED, COLOR_BLACK);
  init_pair(8, COLOR_GREEN, COLOR_BLACK);

  char usrInput{};

  vector<pair<int, int>> monsterPositions;

  // Find initial positions of monsters and store them after reading the maze
  for (int y = 0; y < mazemap.size(); ++y) {
    for (int x = 0; x < mazemap[y].size(); ++x) {
      if (mazemap[y][x] == 'M') {
        monsterPositions.emplace_back(y, x);
      }
    }
  }

  // Initialize the direction for each monster
  for (int i = 0; i < monsterPositions.size(); ++i) {
    int y = monsterPositions[i].first;
    int x = monsterPositions[i].second;

    // Check if the upper or lower block is a wall
    if (mazemap[max(0, y - 1)][x] == '#' ||
        mazemap[min((int)mazemap.size() - 1, y + 1)][x] == '#') {
      // Initial direction is left or right (randomly chosen)
      monsterDirections[i] = (rand() % 2 == 0)
                                 ? make_pair(0, -1)
                                 : make_pair(0, 1); // Left or right
    } else {
      // Initial direction is up or down (randomly chosen)
      monsterDirections[i] =
          (rand() % 2 == 0) ? make_pair(-1, 0) : make_pair(1, 0); // Up or down
    }
  }

  vector<pair<int, int>> checkpointPositions;

  // Find initial positions of checkpoints and store them after reading the maze
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
      if (!msPause && moveMonsters(mazemap, monsterPositions,
                                   make_pair(playerPosY, playerPosX))) {
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
        std::this_thread::sleep_for(
            std::chrono::seconds(1)); // Pause for 1 second

        if (playerHP <= 0) {
          refresh();
          clear(); // Clear the screen to display the game over message cleanly
          getmaxyx(stdscr, screenSizeY,
                   screenSizeX); // Get the current screen size

          int startX = (screenSizeX - 10) /
                       2; // Calculate starting X to center the message
          int startY = screenSizeY / 2; // Center Y position

          mvprintw(startY, startX,
                   "Game over!"); // Display "Game over!" at the center
          refresh();              // Refresh the screen to show the message
          this_thread::sleep_for(
              std::chrono::seconds(2)); // Pause for 2 seconds
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

  // Enable non-blocking input
  nodelay(stdscr, TRUE);

  do {
    while (msPause) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    move(0, 0);
    getmaxyx(stdscr, screenSizeY, screenSizeX);
    // Check if the screen size is less than the required size
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
      // if 'P' is not already on the upper half of screen (AND it is not at the
      // beginning)
      if (playerPosY - linepointer < screenSizeY / 2 && linepointer > 0) {
        linepointer--; // Move viewpoint up
      }
      if (playerPosY > 0 && mazemap[playerPosY - 1][playerPosX] != '#') {
        playerPosY--;
      }
      break;
    case 's':
      // if 'P' is not already on the lower half of screen (AND the last line of
      // maze is not displayed)
      if (playerPosY > screenSizeY / 2 + linepointer &&
          linepointer + screenSizeY < mazemap.size()) {
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
    // Display the map with the player and monsters
    displayMap(mazemap, screenSizeY, linepointer, playerPosY, playerPosX,
               monsterPositions, playerHP);
    refresh();
    if (playerPosY + 1 >= mazemap.size()) {
      win = true;
      break;
    }
    usrInput = getch();
    if (usrInput == ERR) {
      continue; // No input from the user, continue the loop
    }
  } while (usrInput != 'x' && usrInput != 'X' && gameRunning);
  gameRunning = false;
  if (win) {
    getmaxyx(stdscr, screenSizeY,
             screenSizeX); // Get the current screen size
    clear();
    int startX =
        (screenSizeX - 10) / 2;   // Calculate starting X to center the message
    int startY = screenSizeY / 2; // Center Y position

    mvprintw(startY, startX,
             "You Win!"); // Display "Game over!" at the center
    refresh();            // Refresh the screen to show the message
    this_thread::sleep_for(std::chrono::seconds(2)); // Pause for 2 seconds
  }
  if (usrInput == 'x' || usrInput == 'X') {
    storeStatus(playerPosY, playerPosX, playerHP, linepointer);
  }

  // Make sure to join the thread before exiting the program
  monsterThread.join();

  endwin();
  return 0;
}

// Check if the specified position is free (not a wall or out of bounds)
bool isFree(const vector<string> &mazemap, int y, int x) {
  return y >= 0 && y < mazemap.size() && x >= 0 && x < mazemap[0].size() &&
         mazemap[y][x] != '#' && (y != 0 || x != 1);
}

bool moveMonsters(vector<string> &mazemap,
                  vector<pair<int, int>> &monsterPositions,
                  pair<int, int> playerPos) {
  for (int i = 0; i < monsterPositions.size(); ++i) {
    moveMonster(mazemap, monsterPositions[i], i);
    if (monsterPositions[i] == playerPos) {
      return true; // Player touched by monster
    }
  }
  return false;
}

// Change moveMonster to accept an additional parameter: monsterIndex
void moveMonster(vector<string> &mazemap, pair<int, int> &monsterPos,
                 int monsterIndex) {
  pair<int, int> &direction = monsterDirections[monsterIndex];
  pair<int, int> &pos = monsterPos;

  int newY = pos.first + direction.first;
  int newX = pos.second + direction.second;

  // Check if the next position is not free or out of bounds
  if (!isFree(mazemap, newY, newX)) {
    // If the monster hits a wall, check if it should move horizontally
    if (direction.first != 0) { // Currently moving vertically
      // Check if the left or right is available for horizontal movement
      if (isFree(mazemap, pos.first, pos.second - 1)) {
        direction = make_pair(0, -1); // Move left
      } else if (isFree(mazemap, pos.first, pos.second + 1)) {
        direction = make_pair(0, 1); // Move right
      } else {
        // No horizontal move possible, reverse the vertical direction
        direction.first = -direction.first;
      }
    } else {
      // No horizontal move possible or already moving horizontally
      // Reverse the horizontal direction
      direction.second = -direction.second;
    }

    // Calculate new position after changing direction
    newY = pos.first + direction.first;
    newX = pos.second + direction.second;
  }

  // Move the monster if the next position is free
  if (isFree(mazemap, newY, newX)) {
    swap(mazemap[pos.first][pos.second], mazemap[newY][newX]);
    pos.first = newY;
    pos.second = newX;
  }
}

void displayMap(const vector<string> &originalMazemap, int screenSizeY,
                int linepointer, int playerPosY, int playerPosX,
                const vector<pair<int, int>> &monsterPositions, int playerHP) {
  vector<string> mazemap =
      originalMazemap; // Create a copy of the original maze

  // Place the player on the map
  if (playerPosY >= linepointer && playerPosY < linepointer + screenSizeY) {
    mazemap[playerPosY][playerPosX] = 'P';
  }

  // Place monsters on the map
  for (const auto &monsterPos : monsterPositions) {
    if (monsterPos.first >= linepointer &&
        monsterPos.first < linepointer + screenSizeY) {
      mazemap[monsterPos.first][monsterPos.second] = 'M';
    }
  }

  // Display the map with the player and monsters
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
  // Display player HP on the right side of the map
  mvprintw(screenSizeY / 2, mazemap.size() + 60, "HP: %d", playerHP);
  refresh();
}

// Assuming the checkpoints vector contains pairs of y and x coordinates
// and playerPosY and playerPosX are the current player positions
pair<int, int> findNearestCheckpoint(const vector<pair<int, int>> &checkpoints,
                                     int playerPosY, int playerPosX) {
  pair<int, int> nearestCheckpoint;
  int minDistance = INT_MAX;

  for (const auto &checkpoint : checkpoints) {
    int distance = abs(checkpoint.first - playerPosY) +
                   abs(checkpoint.second - playerPosX);
    if (distance < minDistance) {
      minDistance = distance;
      nearestCheckpoint = checkpoint;
    }
  }

  return nearestCheckpoint;
}

void storeStatus(int playerPosY, int playerPosX, int playerHP,
                 int linepointer) {
  ofstream statusfile(".gameConfig/status.txt");
  if (statusfile.fail()) {
    cerr << "Error opening status file" << endl;
    return;
  }
  statusfile << playerPosY << " " << playerPosX << " " << playerHP << " "
             << linepointer;
  statusfile.close();
}
