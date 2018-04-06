#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "mouse.h"
#include "maze.h"

void help(){
  printf("\n\nThis program runs a mouse through a maze!\n\n"
	 "./miniMouse -M    \n"
	 "prints out the prebuilt mazes and describes maze format\n\n"
	 "./miniMouse -d (rows) (cols) (file.txt) \n"
	 "Runs a depth search on a length by height dimension maze described\n"
	 "by the text file, see maze for proper formatting\n\n"
	 "./miniMouse -b (rows) (cols) (file.txt)\n "
	 "Runs a breadth search on a length by height dimension maze\n"
	 "Unfortunately this function does not work at this time\n");
}

void printMazes(){
  printf("\n\nThere are four mazes pre-built into this program, but it can\n"
	   "scan any maze requested so long as it fits the following format\n"
	   "The maze starts at a character 'S' and ends at character 'E'   \n"
          "Every remaining  path space should be marked with a p. In the grid\n"
	   "Every other element is a wall space. '0' denotes that no wall is\n"
	   "present, '-' is a floor/ceiling space, and '|' is a vertical wall."
	   "\n\n"
	   "The mazes are each chosen to show off the two algorithms.\n"
	   "Maze one is a simple (3x3) maze called for a depth search as so:\n"
	   "./miniMouse -d 3 3 maze1.txt\n"
	   "This basic maze just demonstrates the fact that our solvers work.\n\n"
	   "Maze 2 is a slightly more complex (4x5) maze called with depth: \n"
	   "./miniMouse -d 4 5 maze2.txt\n"
	   "This maze forces the depth search to go all the way around the \n"
	   "maze before finding a solution, demonstrating its inneficiency\n\n"

	   "Maze 3 is empty but for a start in the middle and an end in the top\n"
	   "./miniMouse -d 5 5 maze3.txt\n"
	   "Once again the depth search is incredibly inneficient when it comes             to solving this maze\n\n"

	   "Maze 4 is the goal maze, as it is an actual (8x8) microMouse maze\n"
	   "./miniMouse -d 6 6 maze4.txt\n"

	   "These mazes can be kinda hard to conceptualize due to the rows and\n"          "columns that only refer to walls.This is what they look like.\n\n");
    printf("Maze One:\n");
    maze* mz = readMaze(3, 3, "maze1.txt");
    printMaze(mz);
    deleteMaze(mz);

    printf("\nMaze Two:\n");
    mz = readMaze(4, 5, "maze2.txt");
    printMaze(mz);
    deleteMaze(mz);

    printf("\nMaze Three:\n");
    mz = readMaze(5, 5, "maze3.txt");
    printMaze(mz);
    deleteMaze(mz);
    
    printf("\nMaze four:\n");
    mz = readMaze(6, 6, "maze4.txt");
    printMaze(mz);
    deleteMaze(mz);

}

int  main(int argc, char ** argv){
  int rows,cols;
  
  //If you put in -h, print the help statement
  if(argc == 2 && !strcmp(argv[1],"-h")){
    help();
    return 0;
  }

  //This request prints out the mazes and their descriptions
  else if(argc == 2 && !strcmp(argv[1],"-M")){
    printMazes();
    return 0;
  }
  
  //First make sure we have the right ammount of inputs, and they are all the right type;.
  else if(argc == 5){

    //Assert that the provided row and column values are integers.
    if(!sscanf(argv[2], "%d",&rows)){
      printf("You didn't put an integer for your rows. Type -h for help\n");
      return -1;
    }
    if(!sscanf(argv[3], "%d",&cols)){
      printf("You didn't put an integer for your cols. Type -h for help\n");
      return -1;
   }

    
    //Then read and print the maze that has been created with those integers and the file name provided.  
    maze * M = readMaze(rows,cols,argv[4]);
    if(!M){
      printf("Something is probably wrong with your file name.\n");
      return -1;
    }
    printf("This is the maze you requested: %s\n", argv[4]);
    
    int a = printMaze(M);
    assert(!a);
    
    //Make the "mouse" object that walks through the maze.
    mouse * mickey = newMouse(M);

    
    //If a depth search was created run a depth search
    if(!strcmp(argv[1],"-d")){
      if(depthSearch(M,mickey)){
	deleteMouse(mickey);
	deleteMaze(M);
	return -1;
      }
    }

    
    //If a breadth search was requested run a breadth
    else if(!strcmp(argv[1], "-b")){
      printf("one\n");
      breadthSearch(M,mickey);
      
      }

    //If its not one of my two searches it is wrong
    else{
      deleteMouse(mickey);
      deleteMaze(M);
      return -1;
    }

    deleteMouse(mickey);
    deleteMaze(M);
    return 0;
  }

  
  //If nothing has happened yet, the command line input must have been wrong.
  else
  printf("\nSomething went wrong with your inputs. Type -h for help\n\n");  
  return -1;
}

  
