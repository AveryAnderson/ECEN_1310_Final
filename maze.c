#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "maze.h"

//A maze should have its size, and then a matriz to hold values at each point inside of it.
struct _maze {
  int rows, cols;
  int *data;
};

//In order to create our maze, we will need a maze of length 2*cols-1 and height 2*rows - 1.  This is because we need to include our wall data, and therefore need to put a wall container in between each data point, at each odd indexed square
maze * newMaze(int rows, int cols){
  if(rows<=0 || cols<= 0){
    return NULL;
  }
  //Allocate enough memory for the maze and its resulting matrix
  maze * mz = malloc(sizeof(maze));
  if(!mz) return NULL;

  //Set dimensions. These need to be almost twice as many as the maze spaces as we need to account for each internal wall
  mz->rows = 2*rows - 1;
  mz->cols = 2*cols - 1;
  
  //Allocate space for the maze itself
  mz->data = malloc((mz->rows*mz->cols)*sizeof(int));
  if(!mz->data){
    free(mz);
    return NULL;
  }

  //Initialize every point in the maze to bt a zero, makeing it so no walls, and no endpoints currently exist
  for(int i = 0; i < mz->rows*mz->cols; i++){
    mz->data[i] = 0;
  }
  
  return mz;
}

//Free all of the memory stored in the maze
void deleteMaze(maze *mz){
  if (mz) free(mz);
}

//In case I want to get the rows and columns of a maze outside of this library
int getRows(maze * mz){
  if(mz) return mz->rows;
  else return -1;
}
int getCols(maze * mz){
  if(mz) return mz->cols;
  else return -1;
}

//Just like in the matrix library, we are going to want to get and set elements, I will be using the matrix convention of starting indices at one just for simplicity's sake
int getE(maze const * mz, int row, int col){
  return mz->data[(col-1)*mz->rows + (row-1)];
}

void setE(maze const * mz, int row, int col, int val){
  mz->data[(col-1)*mz->rows + (row-1)] = val;
}

//This is the same function from the matrix library. Can't put data outside of our range
int badArgs(maze const * mz, int row, int col) {
  if (!mz || !mz->data)
    return -1;
  if (row <= 0 || row > mz->rows ||
      col <= 0 || col > mz->cols)
    return -2;
  return 0;
}

//Every odd index denotes a wall, if we want a wall, we must change the int at the corresponding index to one.  If the space is not a wall, the function will return -1, else it returns 0
int makeWall(maze *mz, int row, int col){
  //Mod will tell us if a number is odd.  Either one can be odd, if both are, we are at a node which tells us no pracical information. We also must assert that the requested wall is within the maze.
  if(badArgs(mz,row,col)) return -1;
     
  if(!isMazeSpace(mz,row,col)){
    setE(mz,row,col,1);
    return 0;
  }
  return -1;
}

//If we want to delete a wall for whatever reason, the same operation must be done changing the one out for a zero
int removeWall(maze *mz, int row, int col){
  
  if(badArgs(mz,row,col)) return -1;
     
  if(!isMazeSpace(mz,row,col)){
    setE(mz,row,col,0);
    return 0;
  }
  return -1;
}

//This function will read from a provided text file txt to create a maze
maze * readMaze(int rows, int cols, char txt[]){
  if(!rows || !cols || !txt || rows<0 || cols< 0) return NULL;

  int err;
  //First make a new maze that fits the rows and collumns
  maze * mz = newMaze(rows, cols);
  assert(mz);

  //Second declare and open a pointer to our file
  FILE * fPointer;
  fPointer = fopen(txt,"r");
  char temp;

  //Now iterate through the maze using the rows and cols ints as frameworks. Everytime we encounter one of the special characters, S for start E for end and | and - for walls, we should copy that information into our maze
  for(int i = 1; i <= mz->rows; i++){
    for(int j = 1; j<= mz->cols; j++){
      temp = fgetc(fPointer);
      if(isspace(temp)){j--;}
      if(temp == 'S') {err = setStart(mz,i,j); assert(!err);}
      if(temp == 'E') {err = setEnd(mz,i,j); assert(!err);}
      if(temp == '|' || temp == '-') {err = makeWall(mz,i,j); assert(!err);}
      
    }
  }
  
  fclose(fPointer);
  return mz;
}


//This function makes sure that the current square is a maze-square, not a wall square.  This function will be used to make sure the mouse never crosses into an area it shouldn't
int isMazeSpace(maze *mz, int row, int col){
  if(row > mz->rows || col > mz->cols) return 0;

  //Because every other row and column is a wall, in order to be a maze space both your column and your row index must be odd.
  if(row%2 && col%2) return 1;
  return 0;
}



//When we first make a maze, we need to set a start and end point.  If such points already exist in the maze, this function will remove them. Start will be denoted by a 5. End will be denoted by a -5
int setStart(maze * mz, int row, int col){
  if(badArgs(mz,row,col)) return -1;
  //First we must assert that our wanted location is a mazeSpace, not a wall.
  if(isMazeSpace(mz, row, col)){

    //First make sure there isn't another start in this maze
    for(int i = 1; i <= mz->cols; i= i+2)
      for(int j = 1; j <= mz->rows; j = j+2){
	if(getE(mz,j,i) == START)
	  setE(mz,j,i,0);
      }
    
    //After we make sure there are no other start spaces, we can make our start space
    setE(mz,row,col,START);
  }
  return 0;
}

//Works exactly the same as setStart
int setEnd(maze * mz, int row, int col){
  //if(badArgs(mz,row,col)) return -1;
  //First we must assert that our wanted location is a mazeSpace, not a wall.
  if(isMazeSpace(mz, row, col)){

    //First make sure there isn't another end in this maze
    for(int i = 1; i <= mz->cols; i= i+2)
      for(int j = 1; j <= mz->rows; j = j+2){
	if(getE(mz,j,i) == END)
	  setE(mz,j,i,0);
      }
    
    //After we make sure there are no other end spaces, we can make our end space
    setE(mz,row,col,END);
  }
  return 0;
}

//This function takes all of the data found in the maze, and translates it into printable statements
int printMaze(maze *mz){
  //temp will be a holder variable for printing solutions
  int temp;
  if(!mz) return -1;
  for(int i = 1; i <= mz->rows; i++){
    for(int j = 1; j <= mz->cols; j++){      
      //If its a maze space, print its maze status. S = start E = end P = path and D for deadend.  If a solution for this maze has been calculated, it will be retrieved by the temporary variable, the two nested if statemets are to control the spacing between each entry to keep the mazes lined up correctly
      temp = getE(mz,i,j);
      
      if(isMazeSpace(mz,i,j)){
	if(temp == START) printf("S  ");
	else if(temp == END) printf("E  ");
	else if(temp == DEADEND) printf("D  ");
	else if(temp){
	  if(temp<10)	  printf("%d  ", temp);
	  if(temp>=10)    printf("%d ", temp);}
	else printf("p  ");
	}

      //Now anything that remains is a wall. Walls are denoted by ones, or true statements from getE. If anything still has an odd index, it must be a wall, if it has two even vertexes, its a corner, which is not helpful in any way.
      else if( i%2 && temp) printf("|  ");
      else if( i%2 ) printf("   ");
      else if( j%2 && temp) printf("-  ");
      else if( j%2 ) printf("   ");
      else printf("+  ");
    }
     printf("\n");
     }
  
  return 0;
}

//This function is needed for the mouse to be able to tell where it can and cannot move.  IF there is a wall, or the end of the maze at the point it wants to pass over, this function will tell the mouse so it doesn't go that way.
int isWall(maze * mz, int row, int col){
  if(getE(mz,row,col)  && !isMazeSpace(mz,row,col))
    return 1;
  if(badArgs(mz,row,col))
    return 1;
  return 0;
}

//As simple as their names indicate, these functions return true when the spot they are on is a start space/end space
int isStart(maze * mz,int rows, int cols){
  if(!mz || !rows|| !cols) return 0;
  return getE(mz,rows,cols) == START;
}

int isEnd(maze * mz,int rows, int cols){
  if(!mz || !rows|| !cols) return 0;
  return getE(mz,rows,cols) == END;
}
