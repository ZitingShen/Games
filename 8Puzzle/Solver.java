/*************************************************************************
 * Name: Ziting Shen
 * Date: 08/18/2015
 *
 * Compilation:  javac-algs4 Solver.java
 * Execution: java-algs4 Solver (file name)
 * Dependencies: MinPQ.java  In.java  StdOut.java  Board.java
 *
 * Description: Solve the 8 puzzle by A* algorithm.
 *
 *************************************************************************/
import edu.princeton.cs.algs4.MinPQ;
import edu.princeton.cs.algs4.In;
import edu.princeton.cs.algs4.StdOut;
import java.util.Comparator;
import java.util.Stack;

public class Solver {
    private static final Comparator<Node> HAMMING_PRIORITY 
        = new HammingPriority();
    private static final Comparator<Node> MANHATTAN_PRIORITY 
        = new ManhattanPriority();
    
    private MinPQ<Node> nodes;
    private Node search;
    
    // find a solution to the initial board (using the A* algorithm)
    public Solver(Board initial) {
        if (initial == null) throw new NullPointerException();
        nodes = new MinPQ<Node>(MANHATTAN_PRIORITY);
        nodes.insert(new Node(initial, null, 0, false));
        nodes.insert(new Node(initial.twin(), null, 0, true));
        search = nodes.delMin();
        while (!search.board.isGoal()) {
            for (Board neighbor: search.board.neighbors())
                if (search.prev == null || !neighbor.equals(search.prev.board)) 
                    nodes.insert(new Node(neighbor, search,
                                          search.moves + 1, search.isTwin));
            search = nodes.delMin();
        }
    }
    
    // is the initial board solvable?
    public boolean isSolvable() {
        return !search.isTwin;
    }
    
    // min number of moves to solve initial board; -1 if unsolvable
    public int moves() {
        if (!isSolvable()) return -1;
        return search.moves;
    }
    
    // sequence of boards in a shortest solution; null if unsolvable
    public Iterable<Board> solution()  {
        if (!isSolvable()) return null;
        Stack<Board> traceback = new Stack<Board>();
        Stack<Board> solution = new Stack<Board>();
        Node trace = search;
        while (trace != null) {
            traceback.push(trace.board);
            trace = trace.prev;
        }
        while (!traceback.isEmpty())
            solution.push(traceback.pop());
        return solution;
    }
    
    private class Node {
        private Board board;
        private Node prev;
        private int moves, hammingP, manhattanP;
        private boolean isTwin;
        
        private Node(Board b, Node p, int m, boolean tw) {
            board = b;
            prev = p;
            moves = m;
            hammingP = b.hamming() + m;
            manhattanP = b.manhattan() + m;
            isTwin = tw;
        }   
    }
    
    private static class HammingPriority implements Comparator<Node> {
        public int compare(Node v, Node w) {
            if (v.hammingP > w.hammingP) return 1;
            if (v.hammingP < w.hammingP) return -1;
            if (v.manhattanP > w.manhattanP) return 1;
            if (v.manhattanP < w.manhattanP) return -1;
            return 0;
        }
    }
            
    private static class ManhattanPriority implements Comparator<Node> {
        public int compare(Node v, Node w) {
            if (v.manhattanP > w.manhattanP) return 1;
            if (v.manhattanP < w.manhattanP) return -1;
            if (v.hammingP > w.hammingP) return 1;
            if (v.hammingP < w.hammingP) return -1;
            return 0;
        }
    }
    
    // solve a slider puzzle
    public static void main(String[] args) {
        
        // create initial board from file
        In in = new In(args[0]);
        int N = in.readInt();
        int[][] blocks = new int[N][N];
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
            blocks[i][j] = in.readInt();
        Board initial = new Board(blocks);
        
        // solve the puzzle
        Solver solver = new Solver(initial);
        
        // print solution to standard output
        if (!solver.isSolvable())
            StdOut.println("No solution possible");
        else {
            StdOut.println("Minimum number of moves = " + solver.moves());
            for (Board board : solver.solution())
                StdOut.println(board);
        }
    }
}
