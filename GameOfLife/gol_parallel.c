//
// Name: Julian Segert Ziting Shen
// Date: 11/05/2015
//
//This is a parallelized version of Conway's game of life from lab 6.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>

// prototypes
void initializeBoard(FILE* filename, int rows, int cols, int *board);
void *updateBoard(void *args);
int correctIndex(int i, int j, int rows, int cols);
void printBoard(int rows, int cols, int *board);
void printPartition(int start, int end, int direction, int id, int rows, 
                    int cols);
void parseCmdLine(int argc, char **argv, int *ifPrint, int *direction,
                  int *ifPrintAlloc, int *remnant);

//declare necessary barriers
static pthread_barrier_t updateBarrier;
static pthread_barrier_t printBarrier;

// declaration for argument struct passed to threads
struct update_input {
  int id;
  int rows;
  int cols;
  int *oldboard;
  int *newboard;
  int start;
  int end;
  int direction;
  int iterations;
  int ifPrint;
  FILE* filename;
};
/***************************************************************/
// This is the main program of the simulation of the game of life.
int main(int argc, char *argv[]) {
  int rows, cols, iterations; // # of rows, columns, iterations, 
  int i, ntids, ret; // loop counters, number of tids, 
                        // return value of read-in
  FILE *infile; // input file
  struct timeval start_time, end_time; // keep track of the execution time
  double time; // execution time
  pthread_t *tids;
  struct update_input *tid_args;
  int *oldboard; // 2d array used to simulate the board of the game
  int *newboard;
  int ifPrint, direction, ifPrintAlloc, remnant;


  parseCmdLine(argc, argv, &ifPrint, &direction, &ifPrintAlloc, &remnant);

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

  //malloc boards
  oldboard = (int *) malloc(sizeof(int)*rows*cols);
  newboard = (int *) malloc(sizeof(int)*rows*cols);
  if(!oldboard || !newboard) {
    perror("malloc failed");
    exit(1);
  }


  ntids = atoi(argv[3]);
  if((ntids < 1) || (ntids > 100)) {ntids = 8;}

  if(pthread_barrier_init(&updateBarrier, 0, ntids)) {
    perror("pthread_barrier_int error\n");
    exit(1);
  }

   if(pthread_barrier_init(&printBarrier, 0, ntids)) {
    perror("pthread_barrier_int error\n");
    exit(1);
  }

   gettimeofday(&start_time, NULL); //start

  tids = (pthread_t *) malloc(sizeof(pthread_t)*ntids);
  tid_args = (struct update_input *) malloc(sizeof(struct update_input)*ntids);
  if((!tids) || (!tid_args)) {
    perror("malloc failed");
    exit(1);
  }



  for(i = 0; i < ntids; i++) {
    tid_args[i].id = i;
    tid_args[i].rows = rows;
    tid_args[i].cols = cols;
    tid_args[i].oldboard = oldboard;
    tid_args[i].newboard = newboard;
    tid_args[i].direction = direction;
    tid_args[i].iterations = iterations;
    tid_args[i].ifPrint = ifPrint;
    tid_args[i].filename = infile;
  }



  //partition the board. Update thread params as necessary.
  //this handles the case where num_tids does not evenly divide the board
  if(direction == 0) {
    remnant = rows % ntids;
    for(i = 0; i < remnant; i++) {
      tid_args[i].start = (rows/ntids + 1)*i;
      tid_args[i].end = (rows/ntids + 1)*(i + 1) - 1;
    }
    for(i = remnant; i < ntids; i++) {
      tid_args[i].start = (rows/ntids + 1)*remnant 
                          + (rows/ntids)*(i - remnant);
      tid_args[i].end = (rows/ntids + 1)*remnant
                        + (rows/ntids)*(i - remnant + 1) - 1;
      if(rows - 1 < tid_args[i].end) {tid_args[i].end = rows - 1;}
    }
  } else {
    remnant = cols % ntids;
    for(i = 0; i < remnant; i++) {
      tid_args[i].start = (cols/ntids + 1)*i;
      tid_args[i].end = (cols/ntids + 1)*(i + 1) - 1;
    }
    for(i = remnant; i < ntids; i++) {
      tid_args[i].start = (cols/ntids + 1)*remnant
                          + (cols/ntids)*(i - remnant);
      tid_args[i].end = (cols/ntids + 1)*remnant 
                        + (cols/ntids)*(i - remnant + 1) - 1;
      if(cols - 1 < tid_args[i].end) {tid_args[i].end = cols - 1;}
    }
  }


    for(i = 0; i < ntids; i++) {
      ret = pthread_create(&tids[i], 0, updateBoard, &tid_args[i]);
      if(ret) {
        perror("Error pthread_create\n");
        exit(1);
      }
    }
    

  if(ifPrintAlloc == 1) { 
    for(i = 0; i < ntids; i++) {
      printPartition(tid_args[i].start, tid_args[i].end, 
                     tid_args[i].direction, tid_args[i].id, rows, cols);
    }
  }

  for(i = 0; i < ntids; i++) {
      pthread_join(tids[i], 0);
   }   
  
  gettimeofday(&end_time, NULL);
  time = end_time.tv_sec  - start_time.tv_sec 
         + (end_time.tv_usec - start_time.tv_usec)/ (double) 1000000;

  printf("total time for %d iterations of %dx%d is %lf secs \n", 
         iterations, rows, cols, time);



  free(oldboard);
  free(newboard);
  free(tids);
  free(tid_args);
  return 0;
}



/* parseCmdLine- reads in user input from the command line

Parameters: argc and argv from the command line, and pointers to the ifPrint,
direction, ifPrintAlloc and remnat variables in main.

Returns nothing
*/
void parseCmdLine(int argc, char **argv, int *ifPrint, int *direction,
                  int *ifPrintAlloc, int *remnant){
   if(argc != 6) {
      printf("usage: ./gol infile.txt print[0:1] ntids \
partition[0:1] print_alloc[0:1]\n");
      exit(1);
    }

    *ifPrint = atoi(argv[2]);
    if((*ifPrint != 0) && (*ifPrint != 1)) {
      printf("usage: ./gol infile.txt print[0:1] ntids \
partition[0:1] print_alloc[0:1]\n");
      exit(1);
    }

    *direction = atoi(argv[4]);
    if((*direction != 0) && (*direction != 1)) {
      printf("usage: ./gol infile.txt print[0:1] ntids \
partition[0:1] print_alloc[0:1]\n");
      exit(1);
    }

    *ifPrintAlloc = atoi(argv[5]);
    if((*ifPrintAlloc != 0) && (*ifPrintAlloc != 1)) {
      printf("usage: ./gol infile.txt print[0:1] ntids \
partition[0:1] print_alloc[0:1]\n");
      exit(1);
    }


}





/* Sets up the initial board.
Takes a pointer to the file being read and num rows and cols
File MUST be read up to the the number of starting 1s

Returns a pointer to the board as an int array
*/
void initializeBoard(FILE* infile, int rows, int cols, int *board){
  int i, j, x, y, pairs;
  fscanf(infile, "%d", &pairs);
   if(!board) {
    perror("malloc failed");
    exit(1);
  }
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
}

/* This is the function each thread will run. The first thread is responsible
for initializing and printing the board. Every thread run an even partition
of the board.

Takes a pointer to the corresponding parameter struct.

Returns 0 if successful
*/
void *updateBoard(void *args){
  //int id;
  int rows, cols;
  int *oldboard; 
  int *newboard;
  int id, start, end, direction, iterations, ifPrint;
  
  int i, j, k; // loop counters
  int sum; // calculate # of the living cells among the surrounding ones
  int current; // the index of the current cell
  int rowstart, rowend, colstart, colend;
  FILE* filename;


  id = ((struct update_input *)args)->id;
  rows = ((struct update_input *)args)->rows;
  cols = ((struct update_input *)args)->cols;
  oldboard = ((struct update_input *)args)->oldboard;
  newboard = ((struct update_input *)args)->newboard;
  start = ((struct update_input *)args)->start;
  end = ((struct update_input *)args)->end;
  direction = ((struct update_input *)args)->direction;
  iterations = ((struct update_input *)args)->iterations;
  ifPrint = ((struct update_input *)args)->ifPrint;
  filename = ((struct update_input *)args)->filename;

  if(direction == 0) {
    rowstart = start;
    rowend = end;
    colstart = 0;
    colend = cols - 1;
  } else {
    rowstart = 0;
    rowend = rows - 1;
    colstart = start;
    colend = end;
  }

  if (id == 0){
    initializeBoard(filename, rows, cols, oldboard);
  }

  for (k = 0; k < iterations; k++){

    for(i = rowstart; i <= rowend; i++) {
      for(j = colstart; j <= colend; j++) {
        current = i*cols + j;
        sum = 0; //add up neighboring cells
        sum =   oldboard[correctIndex(i - 1, j - 1, rows, cols)] // up-left
              + oldboard[correctIndex(i - 1, j, rows, cols)] // up
              + oldboard[correctIndex(i - 1, j + 1, rows, cols)] // up-right
              + oldboard[correctIndex(i, j - 1, rows, cols)] // left
              + oldboard[correctIndex(i, j + 1, rows, cols)] // right
              + oldboard[correctIndex(i + 1, j - 1, rows, cols)] // down-left
              + oldboard[correctIndex(i + 1, j, rows, cols)] // down
              + oldboard[correctIndex(i + 1, j + 1, rows, cols)]; // down-right
        if((sum < 2) || (sum > 3)) {
          newboard[current] = 0;
        } else if(sum == 3) {
          newboard[current] = 1;
        } else {
          newboard[current] = oldboard[current];
        }
      }
    }

    //wait for all threads to finish updating
    pthread_barrier_wait(&updateBarrier);

    for(i = rowstart; i <= rowend; i++) {
      for(j = colstart; j <= colend; j++) {
        current = i*cols + j;
        oldboard[current] = newboard[current];
      }
    }

    //wait for threads to finish copying before printing
    pthread_barrier_wait(&printBarrier);

    if (ifPrint && (id == 0)){
      usleep(200000);
      system("clear");
      printBoard(rows, cols, oldboard);
    } 
  }


  return (void *)0;
}

/* This function outputs the correct index of the surrounding cells according
 to the requested position and the size of the board. If a neighbor is on the 
 other side of the torus, this function wraps around and returns the correct
 index.

 Takes the number of rows and columns and row and col indices of interest
 Returns index of the cell in the board array*/
int correctIndex(int i, int j, int rows, int cols) {
  if(i < 0) 
    i += rows;
  if(i >= rows) 
    i -= rows;
  if(j < 0) 
    j += cols;
  if(j >= cols)
    j -= cols;
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

/* This function prints partition information. Called by each thread.
Parameters: start and end of the thread's partition, direction of the partition
tid, and total number of rows and columns.

Returns nothing
*/
void printPartition(int start, int end, int direction, int id, int rows, 
                    int cols){
  if(direction == 0){
    printf("tid%3d:  rows:%4d:%4d (%3d)  cols:%4d:%4d (%3d)\n", id, start, end,
           end - start + 1, 0, cols -1, cols);
    fflush(stdout);
    return;
  }
  printf("tid%3d:  rows:%4d:%4d (%3d)  cols:%4d:%4d (%3d)\n", id, 0, rows - 1,
         rows, start, end, end - start + 1);
  fflush(stdout);
  return;
}
