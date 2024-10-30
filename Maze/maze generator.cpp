/**
 * @file maze generator.cpp
 * @brief This file contains the implementation of a maze generator using recursive randomized Prim's Algorithm.
 */

#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <fstream>

using namespace std;

#define SIZE 55 // Update the size of the maze here
#define WALL 1
#define PATH 0

/**
 * @class Maze
 * @brief Represents a maze and provides methods for generating and printing the maze.
 */
class Maze {
public:
    int startX; /**< The x-coordinate of the starting point of the maze */
    int startY; /**< The y-coordinate of the starting point of the maze */
    int maze[SIZE][SIZE]; /**< The maze grid */
    vector<vector<int>> directions = { {0, -2}, {2, 0}, {0, 2}, {-2, 0} }; /**< The possible directions to move in the maze */
    vector<vector<int>> potentialFrontier; /**< The list of potential frontier cells */

    /**
     * @brief Constructs a Maze object and initializes all cells in the maze to be walls.
     */
    Maze() : startX(1), startY(1) {
        for (int y = 0; y < SIZE; y++) {
            for (int x = 0; x < SIZE; x++) {
                maze[y][x] = WALL;
            }
        }
    }

    /**
     * @brief Checks if a cell is a duplicate in the list of potential frontiers.
     * @param x The x-coordinate of the cell
     * @param y The y-coordinate of the cell
     * @return True if the cell is not a duplicate, false otherwise
     */
    bool checkDuplicate(int x, int y) {
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
    void generateMaze(int x, int y) {
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
            int randomIndex = rand() % potentialFrontier.size();
            vector<int> randomFrontier = potentialFrontier[randomIndex];
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
     * @brief Prints the maze.
     */
    void printMaze() {
        for (int y = 0; y < SIZE; y++) {
            for (int x = 0; x < SIZE; x++) {
                if (x == 1 && y == 0) {
                    cout << " ";
                } else if (x == SIZE - 2 && y == SIZE - 1) {
                    cout << " ";
                } else {
                    cout << (maze[y][x] == WALL ? "▓" : " ");
                }
            }
            cout << endl;
        }
    }

    /**
     * @brief Saves the maze to a file.
     */
    void saveMaze() {
        ofstream file(".gamConfig/maze.txt");
        if (file.is_open()) {
            for (int y = 0; y < SIZE; y++) {
                for (int x = 0; x < SIZE; x++) {
                    if (x == 1 && y == 0) {
                        file << " ";
                    } else if (x == SIZE - 2 && y == SIZE - 1) {
                        file << " ";
                    } else {
                        file << (maze[y][x] == WALL ? "#" : " ");
                    }
                }
                file << "\n";
            }
            file.close();
        } else {
            cout << "Unable to open file";
        }
    }
};

int main() {
    // Initialize random number generator
    srand((unsigned) time(NULL));

    // Create a Maze class
    Maze maze;

    // Generating a maze
    cout << "Generating maze..." << endl;
    maze.generateMaze(maze.startX, maze.startY);
    cout << "Maze generated!" << endl;

    // Save maze
    maze.saveMaze();
    return 0;
}
