#ifndef MAZEGAME_H
#define MAZEGAME_H

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
#include "DFS.h"

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
    bool noMonsterZone[SIZE][SIZE] = {false};
    vector<vector<int>> directions = { {0, -2}, {2, 0}, {0, 2}, {-2, 0} }; /**< The possible directions to move in the maze */
    vector<vector<int>> potentialFrontier; /**< The list of potential frontier cells */

    /**
     * @brief Constructs a Maze object and initializes all cells in the maze to be walls.
     */
    Maze();

    /**
     * @brief Checks if a cell is a duplicate in the list of potential frontiers.
     * @param x The x-coordinate of the cell
     * @param y The y-coordinate of the cell
     * @return True if the cell is not a duplicate, false otherwise
     */
    bool checkduplicate(int x, int y);

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
    void generateMaze(int x, int y);

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
    void markNoMonsterZone(int cx, int cy);


    /**
     * @brief Places monsters in dead ends near the entrance of the maze.
     * @param density The density of monsters to be placed (0.0 to 1.0).
     */
    void placeMonsters(float density, Cell* path);

    /**
     * @brief Checks if the cell at the given coordinates is a dead end.
     * @param x The x-coordinate of the cell
     * @param y The y-coordinate of the cell
     * @return True if the cell is a dead end, false otherwise
     */
    bool isDeadEnd(int x, int y);

    /**
     * @brief Places checkpoints in the maze.
     *
     * This function places six checkpoints in the maze at predefined positions.
     * It uses two helper lambda functions: isPath and tryPlacingCheckpoint.
     * isPath checks if a cell is a path and not a wall.
     * tryPlacingCheckpoint attempts to place a checkpoint at a given position if it's not a wall.
     * If the position is a wall, it finds the nearest path cell in the vicinity and places the checkpoint there.
     * After placing a checkpoint, it marks a 5x5 area around the checkpoint as a no-monster zone.
     *
     * The six positions for the checkpoints are defined as follows:
     * - Top-left: (offsetX, offsetY)
     * - Top-right: (SIZE - offsetX - 1, offsetY)
     * - Middle-left: (offsetX, middleY)
     * - Middle-right: (SIZE - offsetX - 1, middleY)
     * - Bottom-left: (offsetX, SIZE - offsetY - 1)
     * - Bottom-right: (SIZE - offsetX - 1, SIZE - offsetY - 1)
     *
     * The offsets and middle coordinates are calculated based on the size of the maze.
     */
    void placeCheckpoints();

    bool isOnPath(int x, int y, Cell* path);

    bool isNearOtherMonster(int x, int y);

    /**
     * @brief Prints the maze.
     */
    void printMaze();

    /**
    * @brief Saves the maze to a file.
    */
    void saveMaze();
};

#endif