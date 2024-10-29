all: minesweepermapgen minesweeper
CPPFLAGS = -pedantic-errors -Werror -std=c++11

minesweepermapgen: minesweepermapgen.o
	g++ $(CPPFLAGS) -o minesweepermapgen minesweepermapgen.o

minesweeper: minesweeper.o
	g++ $(CPPFLAGS) -o minesweeper minesweeper.o

minesweepermapgen.o: minesweepermapgen.cpp ms_config.h
	g++ -c minesweepermapgen.cpp

minesweeper.o: minesweeper.cpp ms_config.h colors.h
	g++ -c minesweeper.cpp

clean:
	rm -f minesweeper minesweepermapgen *.o

run: minesweepermapgen minesweeper
	./minesweepermapgen
	./minesweeper

reroll:
	./minesweepermapgen

.PHONY: clean run reroll
