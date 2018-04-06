#ifndef MAZE_H_
#define MAZE_H_

typedef struct _maze maze;

//These numbers all denote information found within the squares.  They are all negative so the path of the maze cannot be confused with this general maze information. 
#define START  -1
#define END  -2

//These are not heavily used here asides from the printstatement, but they are needed for the mouse functions
#define DEADEND -3

//This funtion takes in the rows and columns of a desired maze, and creates the corresponding maze matrix.  Note, the dimensions of the maze matrix will not be equal to the dimensions of the maze, rather it will also include the areas for the walls of the maze.
maze * newMaze(int rows, int cols);

//This function frees the memory found within the maze
void deleteMaze(maze * mz);

//I am going to want to be able to find out the number of rows and collumns in the maze.
int getRows(maze * mz);
int getCols(maze * mz);

//This function will place a wall on a open wall space.  If there is already a wall or the target square is not a wall space, it will return -1
int makeWall(maze *mz, int row, int col);

//This function does the opposite of makeWall, changing a wall to an open space.
int removeWall(maze * mz, int row, int col);

//This function reads a text file and makes it into a maze
maze * readMaze(int length, int width, char txt[]);

//This function makes sure that the current square is a maze-square, not a wall square.  This function will be used to make sure the mouse never crosses into an area it shouldn't
int isMazeSpace(maze *mz, int row, int col);

//We need to set the endpoints of the maze in order for it to be run.
int setStart(maze *mz, int row, int col);
int setEnd(maze *mz, int row, int col);

//We are going to want to get the values of spaces sometimes. Similarly we will want to set elements in our copied maze.
int getE(maze const * mz, int row, int col);
void setE(maze const * mz, int row, int col, int val);

//This function will allow us to see the structure of our maze
int printMaze(maze *mz);

//This function returns whether or not a wall space actually contains a wall
int isWall(maze *mz, int row, int col);

//These functions will be used to check whether the mouse has completed the maze or has returned to the start.
int isStart(maze *mz,int row, int col);
int isEnd(maze *mz, int row, int col);

#endif
