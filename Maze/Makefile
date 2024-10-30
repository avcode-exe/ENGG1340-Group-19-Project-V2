TARGET = mazeGame
CPPFILES = mazegenfinal.cpp main.cpp DFS.cpp
HEADERFILES = mazeGame.h DFS.h
CPPFLAGS = -pedantic-errors -Werror -std=c++11 -DDEBUG

all: $(TARGET)
$(TARGET): $(CPPFILES) $(HEADERFILES)
	g++ $(CPPFLAGS) $^ -o $@ -lncurses

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean