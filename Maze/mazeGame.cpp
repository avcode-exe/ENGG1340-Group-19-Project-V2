#include <iostream>
#include <fstream>
#include <string>
#include <ncurses.h>
#include <vector>

using namespace std;

void displayMap(vector <string>& mazemap,int screenSizeX, int screenSizeY, int linepointer, int colpointer, int playerPosY, int playerPosX);


int main() {
ifstream mazefile{ "maze.txt" };

if (!mazefile) {
cerr << "Maze map not found!" << endl;
return 1;
}

//Store mazefile (since fstream only allows sequencial access)
vector <string> mazemap;
string mazeStrip; // A line of maze
while(getline(mazefile, mazeStrip))
{
    mazemap.push_back(mazeStrip);
}

#if 0 // To be completed
struct MazeScreen{
    vector <string> mazemap;
    int screenSizeY;
    int screenSizeX;
    int linepointer;
    int playerPosY;
    int playerPosX;
}
MazeScreen ms {}
#endif
int playerPosX = 1;
int playerPosY = 0;
int screenSizeY;
int screenSizeX;
// Normal startup procedue for ncurse library
initscr();
cbreak();
noecho();


//addch('e' | A_BOLD | A_UNDERLINE);

char usrInput {};
int linepointer = 0;
int colpointer = 0; // Not used, will delete later

while (usrInput != 'x')
{   
    move(0,0); //Ensure cursor is at the top-left corner
    getmaxyx(stdscr, screenSizeY, screenSizeX);
    if (screenSizeX < 56) {
	clear();
	printw("Screen width of 56 required\n");
	printw("Current width: %d\n", screenSizeX);
	refresh();
	continue;
    }
    int i = 0;
    switch (usrInput)
    {
        case 'w':
        // if 'P' is not already on the upper half of screen (AND it is not at the beginning)
        if (playerPosY - linepointer < screenSizeY/2 && linepointer > 0 ){
            linepointer--; // Move viewpoint up
        }
        if (playerPosY > 0 && mazemap[playerPosY-1][playerPosX] != '#')
            playerPosY--;
        break;
        case 's':
        // if 'P' is not already on the lower half of screen (AND the last line of maze is not displayed)
        if (playerPosY > screenSizeY/2 + linepointer && linepointer + screenSizeY < mazemap.size())
            linepointer++; // Move viewpoint down
        if (mazemap[playerPosY+1][playerPosX] != '#')
            playerPosY++;
        break;
        case 'a':
        if (mazemap[playerPosY][playerPosX-1] != '#')
            playerPosX--;
        break;
        case 'd':
        if (mazemap[playerPosY][playerPosX+1] != '#')
            playerPosX++;
        break;
    }
    displayMap(mazemap, screenSizeX, screenSizeY, linepointer, colpointer, playerPosY, playerPosX);
    refresh();
    if (playerPosY + 1 >= mazemap.size())
        break;
    usrInput = getch();
}
endwin();
}
void displayMap(vector <string>& mazemap,int screenSizeX, int screenSizeY, int linepointer, int colpointer, int playerPosY, int playerPosX)
{
    move(0,0);
    int i = 0;
    for (string mazeStrip : mazemap)
    {
        if ( i < (screenSizeY + linepointer) && i >= linepointer) {
            if ( i == playerPosY ) {
                mazeStrip[playerPosX] = 'P';
            }
                printw((mazeStrip + "\n").c_str());
            
        }
        i++;
    }
    refresh();
}
