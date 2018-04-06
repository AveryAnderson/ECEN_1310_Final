#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "maze.h"
#include "mouse.h"

//A mouse object keeps track of the cpoied maze it is traversing and also acts as a LIFO  llist. Appending a pointer to te previous space in its location with each step
struct _mouse {
  spot * location;
  maze * copy;
  spot * tail;//Only used for breadth
  int size;//Only used for breadth
};

//This is my super inneficient point object.  It keeps track of its coordinates, the previously visited spot, and the number of steps taken from the start square.  The pointer divergentNode tells me where I branched off should I reach a dead end, and direction tells me which way I went. it is r,d, l or u depending on direction.
struct _spot {
  int row;
  int col;
  spot * next;
  int pathLength;
  spot * divergingNode;//only used for breadth
  char direction;//only used for breadth
};

//This function creates our mouse object, which is actually kinda a linked list.
mouse * newMouse(maze * mz){
  if(!mz) return NULL;

  //Allocate my data for the mouse
  mouse * M = (mouse *) malloc(sizeof(mouse));
  if(!M) return NULL;

  //Make a blank limited copy of the maze for our "mouse" to run through and eventually copy the entire maze into.
  M->copy = newMaze((getRows(mz)+1)/2,(getCols(mz)+1)/2);
  if(!M->copy){
    free (M);
    return NULL;
  }
  
  //Call on my function to find the two end points, then set the location of the end, and pass the full spot of the start. 
  M->location = endSpot(mz);
  assert(!setEnd(M->copy,M->location->row,M->location->col));
  deleteSpot(M->location);
  
  //I reuse M->location to avoid making extra pointers
  M->location =  startSpot(mz);
  if(!M->location) {
    free(M);
    return NULL;
  }
  //M->location->divergingNode = M->location;

  //After I have my startSpot, I have to copy it and its surrounding walls into my blank maze copy.
  assert(!(setStart(M->copy,M->location->row,M->location->col)));
  assert(!copyWalls(mz,M));
  
  //printf("This is the mouse's initial view of the maze\n");
  //printMaze(M->copy);

  M->tail = NULL;
  M->size = 1;
  return M;
}

//When I am done with a mouse I must free its memory
void deleteMouse(mouse * M){
  if(M){
    if(M->location) free(M->location);
    free(M);
  }
}

//I also need a way to make my spots.
spot * newSpot(void){
  spot * s;
  s = (spot *) malloc(sizeof(spot));
  if (!s) return NULL;
  s->row = -1;
  s->col = -1;
  s->next = NULL;
  s->pathLength = 0;
  s->divergingNode = NULL;
  s->direction = '\0';
  return s;
}

//As well as free my spots
void deleteSpot(spot * s){
  if(s) {
    if(s->divergingNode)
      free(s->divergingNode);
    free(s);
  }
}

//This function returns the spot that indicates the start of the maze. Note, if get cols and get rows return negative numbers, the for loops never run and the function has to return NULL.
spot * startSpot(maze * mz){
  if(!mz) return NULL;

  //first make a spot
  spot * s = newSpot();
  if(!s) return NULL;

  //Then iterate through the maze.  If i find a start location I assign its rows and columns to spot s and return it. Otherwise I delete s and return NULL
  for(int i = 1; i <= getCols(mz); i= i+2)
    for(int j = 1; j <= getRows(mz); j = j+2){
      if(isStart(mz,j,i)){
	s->row = j;
	s->col = i;
	return s;
      }
    }
  deleteSpot(s);
  return NULL;
}

//This is the exact same function as startSpot but now returning the end. See above for details
spot * endSpot(maze * mz){
  if(!mz) return NULL;
  
  spot * s = newSpot();
  if(!s) return NULL;
  
  for(int i = 1; i <= getCols(mz); i= i+2)
    for(int j = 1; j <= getRows(mz); j = j+2){
      if(isEnd(mz,j,i)){
	s->row = j;
	s->col = i;
	return s;
      }
    }
  deleteSpot(s);
  return NULL;
}

//This function looks into the maze, checks its walls, and checks the spots location in reference to other ones already located in the mouse linked list, and returns one if the inteded direction is both a valid direction and unique.
//A point to the right has a cal value 2 higher than our initial spot
int hasRight(maze * mz, spot * s, mouse * M){
  if(!mz || !s || !M) return 0;
  
  //First pull the wanted row and column from the our spot
  int newRow = s->row; int newCol = s->col;

  //Second assert that that is a maze-space
  if(!isMazeSpace(mz,newRow,newCol)) return 0;
  
  
  //Third assert that the space in the direction we want is not a wall
  if(isWall(mz,newRow,newCol+1)) return 0;
     
  //The only remaining reason for inability is that we have this space before. This can happen two ways, its still in our queue, or it was removed becuase it was a dead end.

  //The data value stored in a the maze at a deadend is a unique negative int.  So if we see it we know not to go there
  //if(getE(M->copy,newRow,newCol+2)==DEADEND) return 0;
  //With the addition of the breadsearch code, this is now made redundant by the following line

  //For my breadthSearch I need to make sure that the point has not been visited before
  if(getE(M->copy,newRow,newCol+2) != 0 && getE(M->copy,newRow, newCol+2) != END){
     return 0;
  }

  //Check to see if its in the queue
  spot * queued = M->location;
  assert(queued);
  //Then iterate through the queue, checking each point.
  //NOTE:*****This is the location that my breadth search cannot pass***** In order to pass this block I copied other functions to the bottom of the page removing this section
  while (queued->next){

    if(queued->row == newRow && queued->col == newCol+2)
      return 0;
    queued = queued->next;
  }

  if(queued->row == newRow && queued->col == newCol+2){
      return 0;
  }
  return 1;
}


//The others all operate under the same steps only changing the direction of movement.  Moving down equates to increasing our row
int hasDown(maze * mz, spot * s, mouse * M){
  if(!mz || !s || !M) return 0;
  //First pull the wanted row and column from the new spot
  int newRow = s->row; int newCol = s->col;

  //Second assert that that is a maze-space
  if(!isMazeSpace(mz,newRow,newCol)) return 0;
  
  //Third assert that the space in the direction we want is not a wall
  if(isWall(mz,newRow+1,newCol)) return 0;
     
  //The only remaining reason for inability is that we have this space before. This can happen two ways, its still in our queue, or it was removed becuase it was a dead end.
  // if(getE(M->copy,newRow+2,newCol)==DEADEND) return 0;
  //With the addition of the breadsearch code, this is now made redundant by the following line

  //For my breadthSearch I need to make sure that the point has not been visited before
  if(getE(M->copy,newRow+2,newCol) != 0  && getE(M->copy,newRow+2, newCol) != END)
    return 0;

  //Check to see if its in the queue
  spot * queued = M->location;
  //Then iterate through the queue, checking each point.
  while (queued->next){
    if(queued->row == newRow+2 && queued->col == newCol)
      return 0;
    queued = queued->next;
  }
  if(queued->row == newRow+2 && queued->col == newCol){
      return 0;
  }
  return 1;
}

//Moving left equates to decreasing our column
int hasLeft(maze * mz, spot * s,mouse * M){
  if(!mz || !s || !M) return 0;
  //First pull the wanted row and column from the new spot
  int newRow = s->row; int newCol = s->col;

  //Second assert that that is a maze-space
  if(!isMazeSpace(mz,newRow,newCol)) return 0;

  //Third assert that the space in the direction we want is not a wall
  if(isWall(mz,newRow,newCol-1)) return 0;
  
  //The only remaining reason for inability is that we have this space before. This can happen two ways, its still in our queue, or it was removed becuase it was a dead end.
  //if(getE(M->copy,newRow,newCol-2)==DEADEND) return 0;
  //With the addition of the breadsearch code, this is now made redundant by the following line

  //For my breadthSearch I need to make sure that the point has not been visited before
  if(getE(M->copy,newRow,newCol-2) != 0 && getE(M->copy,newRow, newCol-2) != END)
  return 0;

  //Check to see if its in the queue
  spot * queued = M->location;
  //Then iterate through the queue, checking each point.
  while (queued->next){
    if(queued->row == newRow && queued->col == newCol-2){
      return 0;
    }
    queued = queued->next;
  }
  if(queued->row == newRow && queued->col == newCol-2){
      return 0;
  }
  
  return 1;
}

//Moving up equates to decreasing our rows
int hasUp(maze * mz, spot * s, mouse * M){
  if(!mz || !s || !M) return 0;
  //First pull the wanted row and column from the new spot
  int newRow = s->row; int newCol = s->col;
  
  //Second assert that that is a maze-space
  if(!isMazeSpace(mz,newRow,newCol)) return 0;
  
  //Third assert that the space in the direction we want is not a wall
  if(isWall(mz,newRow-1,newCol)) return 0;
     
  //The only remaining reason for inability is that we have this space before. This can happen two ways, its still in our queue, or it was removed becuase it was a dead end.
  //if(getE(M->copy,newRow-2,newCol)==DEADEND) return 0;
  //With the addition of the breadsearch code, this is now made redundant by the following line
  
  //For my breadthSearch I need to make sure that the point has not been visited before
  if(getE(M->copy,newRow-2,newCol) != 0 && getE(M->copy,newRow-2, newCol) != END)
    return 0;

  //Check to see if its in the queue
  spot * queued = M->location;
  //Then iterate through the queue, checking each point.
  while (queued->next){
    if(queued->row == newRow-2 && queued->col == newCol)
      return 0;
    queued = queued->next;
  }
  if(queued->row == newRow-2 && queued->col == newCol){
      return 0;
  }
  return 1;
}

//This function trieds to move the mouse right, down, left, and up in that order. It returns 0 if succesful and otherwise it ruturns -1. It doesn't check for a null mouse as that would mess up the implementation of the function later.  This is essentually the put llist function
int moveMouseDepth(maze * mz, mouse * M){
  spot * moved = newSpot();
  spot * location = M->location;

  //Check to see if it can move in any direction, if it can, copy the appropriate rows and cols into moved, make moved point to the old location and make location point to moved. Lastly copy the walls into our maze copy 
  if(hasRight(mz,location,M)){
    moved->row = location->row;
    moved->col = location->col+2;
    moved->next = M->location;
    moved->pathLength = location->pathLength+1; 
    M->location = moved;
    copyWalls(mz,M);
    return 0;
  }
  else if(hasDown(mz,location,M)){
    moved->row = location->row+2;
    moved->col = location->col;
    moved->next = M->location;
    moved->pathLength = location->pathLength+1; 
    M->location = moved;
    copyWalls(mz,M);
    return 0;
  } 
  else if(hasLeft(mz,location,M)){
    moved->row = location->row;
    moved->col = location->col-2;
    moved->next = M->location;
    moved->pathLength = location->pathLength+1; 
    M->location = moved;
    copyWalls(mz,M);
    return 0;
  } 
  else if(hasUp(mz,location,M)){
    moved->row = location->row-2;
    moved->col = location->col;
    moved->next = M->location;
    moved->pathLength = location->pathLength+1; 
    M->location = moved;
    copyWalls(mz,M);
    return 0;
  }

  //If it has no moves, this function cannot work, so it deletes moved and returns -1
  deleteSpot(moved);
  return -1;
}

//I can also treat my mouse as a FIFO, with location pointing to the head of my llist. getFIFO returns the top article of this queue
spot * getFIFO(mouse * M){
  if(!M|| M->location == M->tail) return NULL;
  spot * head = M->location;
  M->location = head->next;
  M->size--;
  return head;
}

/*THIS IS MY OLD ATTEMPT AT PUTFIFO
//I am fairly certain that this function is the one that breaks my breadth search solver.  When run the print statements reveal that though I place the point 1 5 into the FIFO, when it comes time to evaluate it it pulls out 0 0. This is outside of the maze, and the attempted evaluation of the maze crashes the program. I tried to get around having a tail pointer by using a while loop, but the loop is never entered, and I believe the initialized values of 0 0 for my point is where the incorrect information comes from
void putFIFO(spot * location, mouse * M){
  printf("Put in:%d   %d\n",location->row, location->col);

  if(!M->location){
    M->location = location;
  }
  
  else{
  assert(M->location);
  //spot * move = M->location->next;
  
  // while(move){
  // printf("Does This Happen?/n");
  // move = move->next;
  // }
  M->location->next = location;
  }
  printf("Next to be evaluated is: %d  %d\n",M->location->row, M->location->col);
  if(M->location->next) printf("and then%d %d \n",M->location->next->row, M->location->next->row);*/

//This function tries to put information in my FIFO. I think it might be the cause of my errors
void putFIFO(spot * location, mouse * M){
  printf("Put in:%d   %d\n",location->row, location->col);//Test, print out what I try to put in
  
  M->size++;//increase the size of my fifo

  //If I do not have any elements in my FIFO, I can just add that element as the head
  if(!M->location){
    M->location = location;
  }

  //Otherwise, put the information at the end of the tail
  else{
  M->tail->next = location;
  }
  //Move the tail pointer to the last object
  M->tail = location;

  //Test print statements
  printf("Next to be evaluated is: %d  %d\n",M->location->row, M->location->col);
  if(M->location->next) printf("and then%d %d \n",M->location->next->row, M->location->next->row);
}

int moveMouseBreadth(maze * mz, mouse * M){
  //printf("HERE\n");
  //Get the first element in the FIFO
  spot * location = getFIFO(M);
  printf("BREADTH SEARCH ENDS HERE, BUT ONLY ON THE SECOND LOOP\n");
  assert(location);
  printf("%d    %d/n",location->row, location->col);
  
  //Even my test cases cause a segmentation fault, so location is  a null pointer meaning that my last spot was not correctly put in the FIFO

  //If the returned value was the end, we have our solution, and any other path options are inneficient, so loop through them, set their elements to dead ends, and delete the corresponding nodes.
  if(isEnd(mz,location->row,location->col)){
    spot * temp = location->next;
    spot * temp2;
    while(temp){
      setE(mz,temp->row,temp->col,DEADEND);
      temp2 = temp;
      temp = temp->next;
      deleteSpot(temp2);
    }
    deleteSpot(location);
    return 1;
  }
  printf("HERE\n");

  //Make pointers for each location
  spot * right=NULL;
  spot * down=NULL;
  spot * left=NULL;
  spot * up=NULL;
  //Check each direction. if true, initialize pointer and set the details
  if(hasRightB(mz,location,M)){
    right = newSpot();
    right->row = location->row;
    right-> col = location->col+2;
    right->pathLength = location->pathLength + 1;
    right->divergingNode = location->divergingNode;
    right->direction = location->direction;
    printf("HASRIGHT\n");
  }
  if(hasDownB(mz,location,M)){
    down = newSpot();
    down->row = location->row+2;
    down-> col = location->col;
    down->pathLength = location->pathLength + 1;
    down->divergingNode = location->divergingNode;
    down->direction = location->direction;
    printf("HASDOWN\n");
  }
  if(hasLeftB(mz,location,M)){
    left = newSpot();
    left->row = location->row;
    left-> col = location->col-2;
    left->pathLength = location->pathLength + 1;
    left->divergingNode = location->divergingNode;
    left->direction = location->direction;
    printf("HASLEFT\n");
  }
  if(hasUpB(mz,location,M)){
    up = newSpot();
    up->row = location->row-2;
    up-> col = location->col;
    up->pathLength = location->pathLength + 1;
    up->divergingNode = location->divergingNode;
    up->direction = location->direction;
    printf("HASUP\n");

  }

  //Using logic math, if there are two possible directions I can go to, I want to not only go to them, but have a history of where I diverged
  if(((right!=NULL) + (down !=NULL)  + (left!=NULL) + (up!=NULL))>1){
    if(right){
      right->divergingNode = location;
      right->direction = 'r';
      putFIFO(right,M);
    }
    if(down){
      down->divergingNode = location;
      down->direction = 'd';
      putFIFO(down,M);
    }
    if(left){
      left->divergingNode = location;
      left->direction = 'l';
      putFIFO(left,M);
    }
    if(up){
      up->divergingNode = location;
      up->direction = 'u';
      putFIFO(up,M);
    }
    //printf("Here at Node\n");
  }

  //If we don't have multiple options, we might have a couple. If we initialized any of these pointers we will want to add them to our FIFO
  else if(right) putFIFO(right,M);
  else if(down) putFIFO(down,M);
  else if(left) putFIFO(left,M);
  else if(up) putFIFO(up,M);

  //If we have nothing to add, we are at a deadEnd. Since I do not have all of the data for the path saved, I figured it woudl be easiest to jump back to the node and wall off the direction that leads to this deadend. This does have an issue though as if there are multiple nested branches of dead ends only the last branch will be full removed.
  else{
    if(location->direction == 'r'){
      makeWall(mz,location->divergingNode->row,location->divergingNode->col+1);
      setE(mz,location->divergingNode->row,location->divergingNode->col+2,DEADEND);
    }
    if(location->direction == 'd'){
      makeWall(mz,location->divergingNode->row+1,location->divergingNode->col);
      setE(mz,location->divergingNode->row+2,location->divergingNode->col,DEADEND);
    }
    if(location->direction == 'l'){
      makeWall(mz,location->divergingNode->row,location->divergingNode->col-1);
      setE(mz,location->divergingNode->row,location->divergingNode->col-2,DEADEND);
    }
    if(location->direction == 'u'){
      makeWall(mz,location->divergingNode->row+1,location->divergingNode->col);
      setE(mz,location->divergingNode->row-2,location->divergingNode->col,DEADEND);
    }
  }
  //In order for me to keep track of where I've been, I want to change the maze value of each square when I leave it. However the value of the start square should remain the same.
   if(getE(mz,location->row,location->col) != START){
     setE(mz,location->row,location->col,location->pathLength);
   }
   deleteSpot(location);
   
   
   //printf("finished set of steps\n");
   
   //The size of my FIFO should only be zero when I have pulled out my last attempt and found it to be a deadend. This would mean the maze must be empty, or at any rate, I have nothing left to work with.
   if(M->size==0){
    printf("This maze has no solutions\n");
    return -1;
  }
  return 0;
}

//This function will go back a step for the depth search whenever there is a dead end. It returns -1 if the new location is the start.
int stepBack(mouse * M){
  int row = M->location->row;
  int col = M->location ->col;

  //As long as these are not the end points, change them to deadends
  if(getE(M->copy,row,col) != START && getE(M->copy,row,col) != END){
    setE(M->copy,M->location->row,M->location->col, DEADEND);
  }

  //Create a temp pointer to the top item, move the top, delete the memory in temp
  spot * temp = M->location;
  M->location = M->location->next;
  deleteSpot(temp);
  //printf("ddd %d ddd %d\n",startSpot(M->copy)->row,startSpot(M->copy)->col);

  //If I have regressed to the start space, return -1
  if(M->location->row == startSpot(M->copy)->row && M->location->col == startSpot(M->copy)->col){
     return -1;
  }
  
  return 0;
}

//I want to be able to see what parts of the maze the mouse saw, this function allows me to do that by making a copied maze only showing the walls that the mouse sees.
int copyWalls(maze * mz, mouse * M){
  if(!mz || !M) return -1;

  //Check if the square immediately in each direction is a wall, and then make it a wall in the temp maze
  if(isWall(mz,M->location->row+1,M->location->col)){
    makeWall(M->copy,M->location->row+1,M->location->col);
  }
  if(isWall(mz,M->location->row,M->location->col+1)){
    makeWall(M->copy,M->location->row,M->location->col+1);
  }
  if(isWall(mz,M->location->row-1,M->location->col)){
    makeWall(M->copy,M->location->row-1,M->location->col);
  }
  if(isWall(mz,M->location->row,M->location->col-1)){
    makeWall(M->copy,M->location->row,M->location->col-1);
  }
  return 0;
}

//The depth search  function.
int depthSearch(maze * mz,mouse * M){
  //Stepback only returns true if there is no solution.  So when this is a true statement we can say end the program
  int solutionless;
  if(!mz || !M) return -1;

  //Until we find the end, continue going through this loop
  while((!isEnd(mz,M->location->row,M->location->col))){
    //moveMouseDepth will, if possible, move me in the first priority direction I can go.  That is it will go right, down, left, then up.  If it cannot do any of them it has no moves it will return true.
    if(moveMouseDepth(mz,M)){

      //If the move function returns true we need to call step back. The value stepack returns tells us whether or not might still be a solution
      solutionless = stepBack(M);

      //If there is no solution, print the maze as we saw it and return -1
      if(solutionless){
	printf("The mouse did not find its way to an exit\n");
	printf("This is how it saw the maze.");
	printMaze(M->copy);
	return -1;
      }
    }
  }
  //If we made it this far we must have found a solution.  Run printmouse to print the solution we have found
  printMouse(M);
  return 0;
}

int breadthSearch(maze * mz, mouse * M){
  if(!mz || !M) return -1;
  int cnt = 0;
  int solution = 0;
  //Until we reach the end of the maze, or my tester character reaches 10,this loop will keep iterating.
  while(!solution && cnt<10){
    cnt++;
    printf("loop#%d  Move from %d  %d\n",cnt,M->location->row,M->location->col);
    //printf("HERE\n");
    solution = moveMouseBreadth(mz,M);
    printMaze(mz);
  }
  
  return 0;
}


void printMouse(mouse * M){
  printf("This is the mouse's printed path\n");
  spot * queued = M->location->next;
  maze * mz = M->copy;

  //This goes through the Lifo we have created, and copies the pathlength value of each step into our maze copy.  This allows for an easier look at what each step does in the maze
  while (queued->next){
    //printMaze(mz);
    //printf("aa\n\n");
    setE(mz,queued->row,queued->col,queued->pathLength);
    queued = queued->next;
  }
  printMaze(mz);

}




//These functions are exactly the same as their depth search counterparts, except the queue comparison has been removed
int hasRightB(maze * mz, spot * s, mouse * M){
  if(!mz || !s || !M) return 0;
  
  //First pull the wanted row and column from the our spot
  int newRow = s->row; int newCol = s->col;

  //Second assert that that is a maze-space
  if(!isMazeSpace(mz,newRow,newCol)) return 0;
  
  
  //Third assert that the space in the direction we want is not a wall
  if(isWall(mz,newRow,newCol+1)) return 0;
     
  //The only remaining reason for inability is that we have this space before. This can happen two ways, its still in our queue, or it was removed becuase it was a dead end.

  //The data value stored in a the maze at a deadend is a unique negative int.  So if we see it we know not to go there
  //if(getE(M->copy,newRow,newCol+2)==DEADEND) return 0;
  //With the addition of the breadsearch code, this is now made redundant by the following line

  //For my breadthSearch I need to make sure that the point has not been visited before
  int E = getE(mz,newRow, newCol+2);
  if(E  == END){
    return 1;
  }
  if(E != 0 ) 
    return 0;

 
  return 1;
}
int hasDownB(maze * mz, spot * s, mouse * M){
  if(!mz || !s || !M) return 0;
  //First pull the wanted row and column from the new spot
  int newRow = s->row; int newCol = s->col;

  //Second assert that that is a maze-space
  if(!isMazeSpace(mz,newRow,newCol)) return 0;
  
  //Third assert that the space in the direction we want is not a wall
  if(isWall(mz,newRow+1,newCol)) return 0;
     
  //The only remaining reason for inability is that we have this space before. This can happen two ways, its still in our queue, or it was removed becuase it was a dead end.
  // if(getE(M->copy,newRow+2,newCol)==DEADEND) return 0;
  //With the addition of the breadsearch code, this is now made redundant by the following line

  int E = getE(mz,newRow+2, newCol);
  if(E  == END){
    return 1;
  }
  if(E != 0 ) 
    return 0;
  return 1;
}
int hasLeftB(maze * mz, spot * s,mouse * M){
  if(!mz || !s || !M) return 0;
  //First pull the wanted row and column from the new spot
  int newRow = s->row; int newCol = s->col;

  //Second assert that that is a maze-space
  if(!isMazeSpace(mz,newRow,newCol)) return 0;

  //Third assert that the space in the direction we want is not a wall
  if(isWall(mz,newRow,newCol-1)) return 0;
  
  //The only remaining reason for inability is that we have this space before. This can happen two ways, its still in our queue, or it was removed becuase it was a dead end.
  //if(getE(M->copy,newRow,newCol-2)==DEADEND) return 0;
  //With the addition of the breadsearch code, this is now made redundant by the following line

  int E = getE(mz,newRow, newCol-2);
  if(E  == END){
    return 1;
  }
  if(E != 0 ) 
    return 0;
  
  return 1;
}
int hasUpB(maze * mz, spot * s, mouse * M){
  if(!mz || !s || !M) return 0;
  //First pull the wanted row and column from the new spot
  int newRow = s->row; int newCol = s->col;
  
  //Second assert that that is a maze-space
  if(!isMazeSpace(mz,newRow,newCol)) return 0;
  
  //Third assert that the space in the direction we want is not a wall
  if(isWall(mz,newRow-1,newCol)) return 0;
     
  //The only remaining reason for inability is that we have this space before. This can happen two ways, its still in our queue, or it was removed becuase it was a dead end.
  //if(getE(M->copy,newRow-2,newCol)==DEADEND) return 0;
  //With the addition of the breadsearch code, this is now made redundant by the following line
  
  //For my breadthSearch I need to make sure that the point has not been visited before
  int E = getE(mz,newRow-2, newCol);

  if(E  == END){
    return 1;
  }
  if(E != 0 ) 
    return 0;

  return 1;
}



