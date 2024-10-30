#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <fstream>
#include "mazeGame.h"
#include "DFS.h"

using namespace std;

/**
 * @brief Constructs a Maze object and initializes the maze grid.
 * 
 * This constructor initializes the maze grid to be completely filled with walls.
 * It also sets the starting coordinates of the maze to (1, 1).
 */
Maze::Maze() : startX(1), startY(1) {
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            maze[y][x] = WALL;
        }
    }
}

/**
 * @brief Checks if a given coordinate (x, y) is a duplicate in the potentialFrontier list.
 * 
 * This function iterates through the potentialFrontier list and checks if any of the 
 * elements have the same x and y coordinates as the provided (x, y). If a match is found, 
 * it returns false indicating that the coordinate is a duplicate. If no match is found, 
 * it returns true indicating that the coordinate is unique.
 * 
 * @param x The x-coordinate to check.
 * @param y The y-coordinate to check.
 * @return true if the coordinate (x, y) is unique in the potentialFrontier list.
 * @return false if the coordinate (x, y) is a duplicate in the potentialFrontier list.
 */
bool Maze::checkDuplicate(int x, int y) {
    for (const auto& vec : potentialFrontier) {
        if (vec[2] == x && vec[3] == y) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Generates the maze using recursive randomized Prim's Algorithm.
 * @param x The x-coordinate of the current cell
 * @param y The y-coordinate of the current cell
 * @brief Steps:
 * 1. Select (x, y) to be the starting point
 * 2. Find all possible frontiers
 * 3. Randomly select a frontier
 * 4. Carve a path to the frontier
 * 5. Repeat steps 2-4 until there are no more frontiers
 * More info at https://en.wikipedia.org/wiki/Maze_generation_algorithm#Iterative_randomized_Prim's_algorithm_(without_stack,_without_sets)
 */
void Maze::generateMaze(int x, int y) {
    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Selected (x, y) to be the starting point (step 1)
    maze[x][y] = PATH;

    // Find all possible frontiers (step 2)
    for (const auto& direction : directions) {
        int nextX = x + direction[0];
        int nextY = y + direction[1];
        if (nextX > 0 && nextX < SIZE && nextY > 0 && nextY < SIZE && maze[nextX][nextY] == WALL && checkDuplicate(nextX, nextY)) {
            potentialFrontier.push_back({x, y, nextX, nextY});
        }
    }

    // Remove duplicates
    sort(potentialFrontier.begin(), potentialFrontier.end());
    potentialFrontier.erase(unique(potentialFrontier.begin(), potentialFrontier.end()), potentialFrontier.end());

    // Step 3 to 6
    while (!potentialFrontier.empty()) {
        std::uniform_int_distribution<> dis(0, potentialFrontier.size() - 1);
        int randomIndex = dis(gen);
        auto randomFrontier = potentialFrontier[randomIndex];
        potentialFrontier.erase(potentialFrontier.begin() + randomIndex);
        int frontierX = randomFrontier[2];
        int frontierY = randomFrontier[3];
        int wallX = (frontierX + randomFrontier[0]) / 2;
        int wallY = (frontierY + randomFrontier[1]) / 2;
        if (maze[wallX][wallY] == WALL) {
            maze[wallX][wallY] = PATH;
            maze[frontierX][frontierY] = PATH;
            generateMaze(frontierX, frontierY);
        }
    }
}

/**
 * @brief Marks a 5x5 area as a no monster zone in the maze.
 *
 * This function takes the center coordinates (cx, cy) of the 5x5 area as input.
 * It then iterates over a 5x5 grid centered at (cx, cy), and for each cell in this grid,
 * it checks if the cell is within the bounds of the maze. If it is, the cell is marked as a no monster zone.
 *
 * @param cx The x-coordinate of the center of the no monster zone.
 * @param cy The y-coordinate of the center of the no monster zone.
 */
void Maze::markNoMonsterZone(int cx, int cy) {
    for (int dx = -3; dx <= 3; ++dx) {
        for (int dy = -3; dy <= 3; ++dy) {
            int nx = cx + dx;
            int ny = cy + dy;
            if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE) {
                noMonsterZone[ny][nx] = true;
            }
        }
    }
}

/**
 * @brief Places monsters in the maze based on the given density.
 * 
 * This function identifies potential positions for monsters within the maze,
 * ensuring that they are placed on dead-end paths and not in restricted zones.
 * The number of monsters is determined by the density parameter, and their
 * positions are randomly selected from the potential positions.
 * 
 * @param density A float value representing the density of monsters to be placed.
 *                This value should be between 0 and 1, where 0 means no monsters
 *                and 1 means maximum possible monsters based on potential positions.
 * @param path A pointer to the Cell structure representing the path in the maze.
 *             This is used to ensure monsters are not placed on the main path.
 */
void Maze::placeMonsters(float density, Cell* path) {
    std::vector<std::pair<int, int>> potentialMonsterPositions;
    for (int y = 1; y < SIZE - 1; y++) {
        for (int x = 1; x < SIZE - 1; x++) {
            if (maze[y][x] == PATH && isDeadEnd(x, y) && !noMonsterZone[y][x] && !isOnPath(x, y, path) && !isNearOtherMonster(x, y)) {
                potentialMonsterPositions.push_back({x, y});
            }
        }
    }

    int numMonsters = density * potentialMonsterPositions.size();

    // Use std::shuffle with a random number generator
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(potentialMonsterPositions.begin(), potentialMonsterPositions.end(), g);

    for (int i = 0; i < numMonsters; i++) {
        int x = potentialMonsterPositions[i].first;
        int y = potentialMonsterPositions[i].second;
        maze[y][x] = 'M'; // Place monster
    }
}

/**
 * @brief Checks if there is another monster within a 5x5 grid around the given coordinates.
 * 
 * This function iterates through a 5x5 grid centered at the given (x, y) coordinates
 * to determine if there is a monster ('M') in any of the surrounding cells.
 * 
 * @param x The x-coordinate to check around.
 * @param y The y-coordinate to check around.
 * @return true If there is a monster within the 5x5 grid around the given coordinates.
 * @return false If there are no monsters within the 5x5 grid around the given coordinates.
 */
bool Maze::isNearOtherMonster(int x, int y) {
    for (int dx = -5; dx <= 5; dx++) {
        for (int dy = -5; dy <= 5; dy++) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && maze[ny][nx] == 'M') {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Checks if the given cell (x, y) in the maze is a dead end.
 *
 * A cell is considered a dead end if it is a path cell and has only one adjacent path cell.
 *
 * @param x The x-coordinate of the cell to check.
 * @param y The y-coordinate of the cell to check.
 * @return true if the cell is a dead end, false otherwise.
 */
bool Maze::isDeadEnd(int x, int y) {
    if (maze[y][x] == PATH) {
        int paths = 0;
        for (const auto& direction : directions) {
            int dx = direction[0];
            int dy = direction[1];
            if (x + dx >= 0 && x + dx < SIZE && y + dy >= 0 && y + dy < SIZE) {
                if (maze[y + dy][x + dx] == PATH) {
                    paths++;
                }
            }
        }
        // A dead end will only have one adjacent path
        return paths == 1;
    }
    return false;
}

/**
 * @brief Places checkpoints at predefined positions within the maze.
 * 
 * This function attempts to place checkpoints ('C') at six specific positions
 * within the maze. If the intended position is a wall, it will search for the 
 * nearest path cell in the vicinity and place the checkpoint there. Each time 
 * a checkpoint is placed, the surrounding area is marked as a no-monster zone.
 * 
 * The predefined positions for the checkpoints are:
 * - Top-left
 * - Top-right
 * - Middle-left
 * - Middle-right
 * - Bottom-left
 * - Bottom-right
 * 
 * The exact positions are determined by dividing the maze size into quarters 
 * and placing the checkpoints accordingly.
 * 
 * The function uses two helper functions:
 * - isPath: Checks if a given cell is a path and not a wall.
 * - tryPlacingCheckpoint: Attempts to place a checkpoint at a given position 
 *   or the nearest path cell if the position is a wall.
 */
void Maze::placeCheckpoints() {
    // Function to check if a cell is a path and not a wall
    auto isPath = [this](int x, int y) -> bool {
        return maze[y][x] == PATH;
    };

    // Helper function to place a checkpoint if the position is not a wall
    auto tryPlacingCheckpoint = [this, &isPath](int x, int y) {
        if (isPath(x, y)) {
            maze[y][x] = 'C';
            markNoMonsterZone(x, y); // Call this function whenever a checkpoint is placed
        } else {
            // Find the nearest path cell in the vicinity of the intended position
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && isPath(nx, ny)) {
                        maze[ny][nx] = 'C';
                        markNoMonsterZone(nx, ny); // Mark the no-monster zone for this checkpoint
                        return; // Stop after placing one checkpoint
                    }
                }
            }
        }
    };

    // Define the six positions for the checkpoints. Adjust as necessary for exact placement.
    int middleY = SIZE / 2;
    int offsetX = SIZE / 4; // Adjust offset as needed
    int offsetY = SIZE / 4; // Adjust offset as needed

    // Top-left
    tryPlacingCheckpoint(offsetX, offsetY);
    // Top-right
    tryPlacingCheckpoint(SIZE - offsetX - 1, offsetY);
    // Middle-left
    tryPlacingCheckpoint(offsetX, middleY);
    // Middle-right
    tryPlacingCheckpoint(SIZE - offsetX - 1, middleY);
    // Bottom-left
    tryPlacingCheckpoint(offsetX, SIZE - offsetY - 1);
    // Bottom-right
    tryPlacingCheckpoint(SIZE - offsetX - 1, SIZE - offsetY - 1);
}

/**
 * @brief Checks if a given cell (x, y) is on the specified path.
 * 
 * This function iterates through the linked list of cells representing the path
 * and checks if any cell in the path matches the given coordinates (x, y).
 * 
 * @param x The x-coordinate (column) of the cell to check.
 * @param y The y-coordinate (row) of the cell to check.
 * @param path A pointer to the head of the linked list representing the path.
 * @return true If the cell (x, y) is on the path.
 * @return false If the cell (x, y) is not on the path.
 */
bool Maze::isOnPath(int x, int y, Cell* path) {
    while (path != nullptr) {
        if (path->row == y && path->col == x) {
            return true;
        }
        path = path->next;
    }
    return false;
}

/**
 * @brief Prints the maze to the standard output.
 * 
 * This function iterates through the maze grid and prints each cell's content.
 * - 'C' represents a checkpoint and is printed as 'C'.
 * - 'M' represents a monster and is printed as 'M'.
 * - The starting point (1, 0) and the ending point (SIZE-2, SIZE-1) are printed as spaces.
 * - Walls are represented by the character '▓'.
 * - Empty spaces are printed as spaces.
 * 
 * The maze is printed row by row.
 */
void Maze::printMaze() {
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (maze[y][x] == 'C') { // Check for checkpoint and print 'C'
                cout << "C";
            } else if (maze[y][x] == 'M') { // Check for monster and print 'M'
                cout << "M";
            } else if (x == 1 && y == 0 || x == SIZE - 2 && y == SIZE - 1) {
                cout << " ";
            } else {
                cout << (maze[y][x] == WALL ? "▓" : " ");  // "▓"
            }
        }
        cout << endl;
    }
}

/**
 * @brief Saves the current state of the maze to a file.
 *
 * This function writes the maze configuration to a file located at ".gameConfig/maze.txt".
 * The maze is represented as a grid of characters, where:
 * - 'C' represents a checkpoint.
 * - 'M' represents a monster.
 * - '#' represents a wall.
 * - ' ' represents a path.
 *
 * The function handles special cases for the start and end points of the maze.
 * If the file cannot be opened, an error message is printed to the console.
 */
void Maze::saveMaze() {
    std::ofstream file(".gameConfig/maze.txt"); // Corrected file path
    if (file.is_open()) {
        for (int y = 0; y < SIZE; y++) {
            for (int x = 0; x < SIZE; x++) {
                if (maze[y][x] == 'C') { // Check for checkpoint and print 'C'
                    file << "C";
                } else if (maze[y][x] == 'M') { // Check for monster and print 'M'
                    file << "M";
                } else if (x == 1 && y == 0 || x == SIZE - 2 && y == SIZE - 1) {
                    file << " ";
                } else {
                    file << (maze[y][x] == WALL ? "#" : " "); // "#" for wall and " " for path
                }
            }
            file << "\n";
        }
        file.close();
    } else {
        cout << "Unable to open file";
    }
}