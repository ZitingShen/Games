/*************************************************************************
 * Name: Ziting Shen
 * Date: 08/18/2015
 *
 * Compilation:  javac-algs4 Board.java
 * Execution: java-algs4 Board
 *
 * Description: Construct a board to solve 8 puzzle using a known int[][].
 *
 *************************************************************************/

import java.util.Queue;
import java.util.LinkedList;

public class Board {
    private char[] board;
    private int N;
    private int hamming = 0;
    private int manhattan = 0;
    
    // construct a board from an N-by-N array of blocks 
    // (where blocks[i][j] = block in row i, column j)
    public Board(int[][] blocks) {
        N = blocks.length;
        board = new char[N*N];
        for (int i = 0; i < N; i++) 
            for (int j = 0; j < N; j++) {
                int index = i*N + j;
                board[index] = (char) blocks[i][j];
                
                if (board[index] != 0) {
                    if (board[index] != index + 1) hamming++;
                    manhattan += Math.abs((board[index] - 1)/N - i) 
                        + Math.abs((board[index] - 1) % N - j);
                }
            }
    }
    
    // board dimension N
    public int dimension() {
        return N;
    }
    
    // number of blocks out of place
    public int hamming() {
        return hamming;
    }
    
    // sum of Manhattan distances between blocks and goal
    public int manhattan() {
        return manhattan;
    }
    
    // is this board the goal board?
    public boolean isGoal() {
        for (int i = 0; i < N*N; i++) 
            if (board[i] != i + 1 && board[i] != 0) return false;
        return true;
    }
    
    // a board that is obtained by exchanging two adjacent blocks in the same row
    public Board twin() {
        if (board[0] != 0 && board[1] != 0) {
            exch(0, 1);
            Board twinBoard = new Board(grid());
            exch(0, 1);
            return twinBoard;
        }
        exch(N, N + 1);
        Board twinBoard = new Board(grid());
        exch(N, N + 1);
        return twinBoard;
    }
    
    private void exch(int i, int j) {
        char temp = board[i];
        board[i] = board[j];
        board[j] = temp;
    }
    
    private int[][] grid() {
        int[][] grid = new int[N][N];
        for (int i = 0; i < N*N; i++) 
            grid[i/N][i % N] = (int) board[i];
        return grid;
    }
    
    // does this board equal y?
    public boolean equals(Object y) {
        if (y == this) return true;
        if (y == null) return false;
        if (y.getClass() != this.getClass()) return false;
        Board that = (Board) y;
        if (that.dimension() != N) return false;
        for (int i = 0; i < N*N; i++) 
            if (that.board[i] != this.board[i]) return false;
        return true;
    }
    
    // all neighboring boards
    public Iterable<Board> neighbors() {
        Queue<Board> queue = new LinkedList<Board>();
        int emptyIndex = 0;
        
        while (board[emptyIndex] != 0) { emptyIndex++; }
        int row = emptyIndex / N;
        int col = emptyIndex % N;
        if (row > 0) {
            exch(emptyIndex, emptyIndex - N);
            queue.offer(new Board(grid()));
            exch(emptyIndex, emptyIndex - N);
        }
        if (row < N - 1) {
            exch(emptyIndex, emptyIndex + N);
            queue.offer(new Board(grid()));
            exch(emptyIndex, emptyIndex + N);
        }
        if (col > 0) {
            exch(emptyIndex, emptyIndex - 1);
            queue.offer(new Board(grid()));
            exch(emptyIndex, emptyIndex - 1);
        }
        if (col < N - 1) {
            exch(emptyIndex, emptyIndex + 1);
            queue.offer(new Board(grid()));
            exch(emptyIndex, emptyIndex + 1);
        }
        return queue;
    }
    
    // string representation of this board
    public String toString() {
        StringBuilder s = new StringBuilder();
        s.append(N);
        
        for (int i = 0; i < N*N; i++) {
            if (i % N == 0) s.append("\n");
            s.append(String.format("%2d ", (int) board[i]));       
        }
        s.append("\n");
        return s.toString();
    }
    
    // unit tests 
    public static void main(String[] args) {
//        Board b = new Board(new int[][]{{1, 2}, {3, 0}});
//        Board c = new Board(new int[][]{{8, 1, 3}, {4, 0, 2}, {7, 6, 5}});
//        Board d = new Board(new int[][]{{1, 2, 3, 4}, {5, 6, 7, 8}, 
//                            {9, 10, 11, 12}, {13, 14, 15, 0}});
//        System.out.println(c.manhattan());
    }
}
