# Define compiler and flags
CXX = g++
CPPFLAGS = -pedantic-errors -Werror -std=c++11 -fPIE
LDFLAGS = -lncurses -lpthread

# Define paths
MAZE_SRC = Maze/main.cpp Maze/DFS.cpp Maze/mazegenfinal.cpp
MS_SRC = Minesweeper/minesweeper.cpp Minesweeper/minesweepermapgen.cpp
INCLUDES = -IMaze -IMinesweeper

# Define the target
TARGET = mazeGame

# Define all source files
SRC = $(MAZE_SRC) $(MS_SRC)

# Define object files from source files
OBJ = $(SRC:.cpp=.o)

# Default target
all: $(TARGET)

# Rule to link the program
$(TARGET): $(OBJ)
	$(CXX) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

# Rule to compile source files into object files
%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

# Run target
run: $(TARGET)
	./$(TARGET)

# Clean target
clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean run
