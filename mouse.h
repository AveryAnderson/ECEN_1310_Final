#ifndef MOUSE_H_
#define MOUSE_H_

typedef struct _maze maze;
typedef struct _mouse mouse;
typedef struct _spot spot;

//This function creates our mouse object, which is essentially a lifo
mouse * newMouse(maze * mz);

//This frees all of the data inside the mouse
void deleteMouse(mouse * M);

//This function creates spot objects
spot * newSpot(void);

//This function deletes spots
void deleteSpot(spot * s);

//This function finds the first spot on out list
spot * startSpot(maze * mz);

spot * endSpot(maze * mz);

//The following functions define the ability of the lifo at that moment to move right, down, left and up. They return 1 if there is an accesible space in the described direction.
int hasRight(maze * mz, spot * s, mouse * M);
int hasDown(maze * mz, spot * s, mouse * M);
int hasLeft(maze * mz, spot * s, mouse * M);
int hasUp(maze * mz, spot * s, mouse * M);

int moveMouseDepth(maze * mz, mouse * M);

int moveMouseBreadth(maze * mz, mouse * M);

int stepBack(mouse * M);

int copyWalls(maze * mz, mouse * M);

int depthSearch(maze * mz, mouse * M);

int breadthSearch(maze * mz, mouse * M);

void printMouse(mouse * M);

int hasRightB(maze * mz, spot * s, mouse * M);
int hasDownB(maze * mz, spot * s, mouse * M);
int hasLeftB(maze * mz, spot * s, mouse * M);
int hasUpB(maze * mz, spot * s, mouse * M);


#endif
