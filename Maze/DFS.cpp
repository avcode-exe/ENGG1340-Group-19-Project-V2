#include "DFS.h"

/**
 * Performs a depth-first search (DFS) on a maze to find a path from a given
 * starting position to a goal position.
 *
 * @param maze The maze represented as a 2D vector of characters.
 * @param row The current row position in the maze.
 * @param col The current column position in the maze.
 * @param path A list of cells representing the path from the starting position
 * to the goal position.
 * @param goalRow The row position of the goal in the maze.
 * @param goalCol The column position of the goal in the maze.
 * @return True if a path from the starting position to the goal position is
 * found, false otherwise.
 */
bool dfs(vector<vector<char>> &maze, int row, int col, Cell *&path, int goalRow,
         int goalCol) {
  if (row < 0 || row >= maze.size() || col < 0 || col >= maze[0].size() ||
      maze[row][col] == '#') {
    return false;
  }

  if (row == goalRow && col == goalCol) {
    path = new Cell{row, col, path};
    return true;
  }

  maze[row][col] = '#';

  if (dfs(maze, row - 1, col, path, goalRow, goalCol) ||
      dfs(maze, row + 1, col, path, goalRow, goalCol) ||
      dfs(maze, row, col - 1, path, goalRow, goalCol) ||
      dfs(maze, row, col + 1, path, goalRow, goalCol)) {
    path = new Cell{row, col, path};
    return true;
  }

  return false;
}

/**
 * @brief Deletes a linked list of Cells representing a path in the maze.
 *
 * This function iterates over a linked list of Cells, deleting each Cell as it
 * goes. It uses a while loop to traverse the list. Inside the loop, it saves
 * the pointer to the next Cell, deletes the current Cell, and then moves the
 * path pointer to the next Cell. The loop continues until it has deleted all
 * Cells, i.e., until the path pointer is nullptr.
 *
 * @param path A pointer to the first Cell in the linked list representing the
 * path.
 */
void deletePath(Cell *path) {
  while (path != nullptr) {
    Cell *next = path->next;
    delete path;
    path = next;
  }
}

/**
 * @brief The main function reads a maze from a file, performs a depth-first
 * search (DFS) to find a path from the start to the goal, and prints the maze
 * with the path marked.
 *
 * @return int Returns 0 if the program executed successfully.
 */
Cell *findPath() {
  ifstream file("maze.txt");
  if (!file) {
    Cell *temp = nullptr;
    // cout << "Failed to open the maze file." << endl;
    return temp;
  }

  vector<vector<char>> maze;
  string line;
  while (getline(file, line)) {
    maze.push_back(vector<char>(line.begin(), line.end()));
  }

  int startRow = 0;
  int startCol = 1;
  int goalRow = maze.size() - 1;
  int goalCol = maze[0].size() - 2;

  Cell *path = nullptr;
  vector<vector<char>> mazeCopy = maze;
  dfs(mazeCopy, startRow, startCol, path, goalRow, goalCol);

  return path;
}
