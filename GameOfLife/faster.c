//
// Name: Julian Segert Ziting Shen
// Date: 11/09/2015
//
// This program simulates the game of life, receiving an input file about
// the number of rows, columns, iterations and the initial alive cells, and
// then update and print out the board for certain times. 
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

// prototypes
int *initializeBoard(FILE* filename, int rows, int cols);
int *updateBoard(int rows, int cols, int *board);
void updateCell(int *board, int *new, int *updated, int rows, int cols, 
               int i, int j);
int correctIndex(int i, int j, int rows, int cols);
void printBoard(int rows, int cols, int *board);

// This is the main program of the simulation of the game of life.
int main(int argc, char *argv[]) {
  int rows, cols, iterations; // # of rows, columns, iterations
  int i, ret; // loop counter and return value of read-in
  FILE *infile; // input file
  int *board; // 2d array used to simulate the board of the game
  struct timeval start_time, end_time; // keep track of the execution time
  double time; // execution time

  if(argc != 3) {
    printf("usage: ./gol infile.txt [1 to print the board, 0 not]");
    exit(1);
  }

  infile = fopen(argv[1], "r");
  if (infile == NULL) {
    printf("Error:file open %s\n", argv[1]);
    exit(1);
  }

  //parse file contents
  ret = fscanf(infile, "%d %d %d", &rows, &cols, &iterations);
  if (ret != 3){
    printf("File could not be read");
    exit(1);
  }

  board = initializeBoard(infile, rows, cols);

  gettimeofday(&start_time, NULL);
  for(i = 0; i < iterations; i++) {
    board = updateBoard(rows, cols, board);
    if(atoi(argv[2])) {
      usleep(200000);
      system("clear");
      printBoard(rows, cols, board);
    }
  }
  gettimeofday(&end_time, NULL);
  
  time = end_time.tv_sec  - start_time.tv_sec 
         + (end_time.tv_usec - start_time.tv_usec)/ (double) 1000000;

  printf("total time for %d iterations of %dx%d is %lf secs \n", 
         iterations, rows, cols, time);

  free(board);
  return 0;
}

/* Sets up the initial board.
Takes a pointer to the file being read and num rows and cols
File MUST be read up to the the number of starting 1s

Returns a pointer to the board as an int array
*/
int *initializeBoard(FILE* infile, int rows, int cols){
  int i, j, x, y, pairs;
  int *board;
  fscanf(infile, "%d", &pairs);
  board = (int *) malloc(sizeof(int)*rows*cols);
  for(i = 0; i < rows; i++) {
    for(j = 0; j < cols; j++) {
      board[i*cols + j] = 0;
    }
  }

  for(i = 0; i < pairs; i++) {
    fscanf(infile, "%d %d", &x, &y);
    board[x*cols + y] = 1;
  }
  fclose(infile);
  return board;
}

/* This function updates each cell of the board in each iteration according 
to the 8 surrounding neighbor cells, and returns an updated board. 

Takes a pointer to the old board with number of rows and cols
Returns a pointer to the new board as an int array
*/
int *updateBoard(int rows, int cols, int *board) {
  int *new; // a board to keep the results of updating
  int *updated; // a board to keep if the cells have been updated
  int i, j; // loop counters
  int current; // the index of the current cell

  new = (int *) malloc(sizeof(int)*rows*cols);
  updated = (int *) malloc(sizeof(int)*rows*cols);

  for(i = 0; i < rows; i++) {
    for(j = 0; j < cols; j++) {
      new[i*cols + j] = 0;
      updated[i*cols + j] = 0;
    }
  }

  for(i = 0; i < rows; i++) {
    for(j = 0; j < cols; j++) {
      current = i*cols + j;
      if(board[current]) {
        updateCell(board, new, updated, rows, cols, 
                  i - 1, j - 1); // up-left
        updateCell(board, new, updated, rows, cols, 
                  i - 1, j); // up
        updateCell(board, new, updated, rows, cols, 
                  i - 1, j + 1); // up-right
        updateCell(board, new, updated, rows, cols, 
                  i, j - 1); // left
        updateCell(board, new, updated, rows, cols, 
                  i, j); // current
        updateCell(board, new, updated, rows, cols, 
                  i, j + 1); // right
        updateCell(board, new, updated, rows, cols, 
                  i + 1, j - 1); // down-left
        updateCell(board, new, updated, rows, cols, 
                  i + 1, j); // down
        updateCell(board, new, updated, rows, cols, 
                  i + 1, j + 1); // down-right
      }
    }
  }
  free(board);
  free(updated);
  return new;
}

/* This function updates the status of a cell according to the 8 surrounding
 cells.

 Takes a pointer to the original board, a pointer to the new board, a
 pointer to the board used to check if the cells are updated, the number of
 rows and columns, and the coordinates of the cell which will be updated.
 Returns nothing but update the new board.*/
void updateCell(int *board, int *new, int *updated, int rows, int cols, 
                int i, int j) {
  int sum; // calculate # of the living cells among the surrounding ones
  int current; // the index of the current cell

  current = correctIndex(i, j, rows, cols);
  if (updated[current]) return;

  sum = 0;
  sum =   board[correctIndex(i - 1, j - 1, rows, cols)] // up-left
        + board[correctIndex(i - 1, j, rows, cols)] // up
        + board[correctIndex(i - 1, j + 1, rows, cols)] // up-right
        + board[correctIndex(i, j - 1, rows, cols)] // left
        + board[correctIndex(i, j + 1, rows, cols)] // right
        + board[correctIndex(i + 1, j - 1, rows, cols)] // down-left
        + board[correctIndex(i + 1, j, rows, cols)] // down
        + board[correctIndex(i + 1, j + 1, rows, cols)]; // down-right
  if((sum < 2) || (sum > 3)) {
    new[current] = 0;
  } else if(sum == 3) {
    new[current] = 1;
  } else {
    new[current] = board[current];
  }
  updated[current] = 1;
}

/* This function outputs the correct index of the surrounding cells according
 to the requested position and the size of the board. If a neighbor is on the 
 other side of the torus, this function wraps around and returns the correct
 index.

 Takes the number of rows and columns and row and col indices of interest
 Returns index of the cell in the board array*/
int correctIndex(int i, int j, int rows, int cols) {
  if(i < 0) i += rows;
  if(i >= rows) i -= rows;
  if(j < 0) j += cols;
  if(j >= cols) j -= cols;
  return i*cols + j;
}

/* This function prints out the board.
Takes the number of rows and cols and a pointer to the board
Returns nothing but prints to console
*/
void printBoard(int rows, int cols, int *board) {
  int i, j;
  for(i = 0; i < rows; i++) {
    for(j = 0; j < cols; j++) {
      if (board[i*cols +j] == 0) 
        printf("- ");
      else
        printf("@ ");
    }
    printf("\n");
  }
}
