#ifndef DFS_H
#define DFS_H

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct Cell {
    int row;
    int col;
    Cell* next;
};

/**
 * Performs a depth-first search (DFS) on a maze to find a path from a given starting position to a goal position.
 *
 * @param maze The maze represented as a 2D vector of characters.
 * @param row The current row position in the maze.
 * @param col The current column position in the maze.
 * @param path A list of cells representing the path from the starting position to the goal position.
 * @param goalRow The row position of the goal in the maze.
 * @param goalCol The column position of the goal in the maze.
 * @return True if a path from the starting position to the goal position is found, false otherwise.
 */
bool dfs(vector<vector<char>>& maze, int row, int col, Cell*& path, int goalRow, int goalCol);

/**
 * @brief Reads a maze from a file and performs a depth-first search (DFS) to find a path from the start to the goal.
 *
 * @return A pointer to the first Cell in the linked list representing the path, or nullptr if no path is found.
 */
Cell* findPath();

/**
 * @brief Deletes a linked list of Cells representing a path in the maze.
 *
 * This function iterates over a linked list of Cells, deleting each Cell as it goes.
 * It uses a while loop to traverse the list. Inside the loop, it saves the pointer to the next Cell,
 * deletes the current Cell, and then moves the path pointer to the next Cell.
 * The loop continues until it has deleted all Cells, i.e., until the path pointer is nullptr.
 *
 * @param path A pointer to the first Cell in the linked list representing the path.
 */
void deletePath(Cell* path);

#endif