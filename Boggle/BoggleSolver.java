import edu.princeton.cs.algs4.StdOut;
import edu.princeton.cs.algs4.In;
import edu.princeton.cs.algs4.SET;

public class BoggleSolver {
    private TST<Integer> dic;
    private SET<String> result;

    // Initializes the data structure using the given array of strings as the 
    // dictionary. (You can assume each word in the dictionary contains only 
    // the uppercase letters A through Z.)
    public BoggleSolver(String[] dictionary) {
        dic = new TST<Integer>();
        for (String word: dictionary) {
            int value = 0;
            if (word.length() > 7) value = 11;
            else if (word.length() == 7) value = 5;
            else if (word.length() == 6) value = 3;
            else if (word.length() == 5) value = 2;
            else if (word.length() > 2) value = 1;
            else continue;
            dic.put(word, value);
        }
    }

    // Returns the set of all valid words in the given Boggle board, as an 
    // Iterable.
    public Iterable<String> getAllValidWords(BoggleBoard board) {
        result = new SET<String>();
        for (int i = 0; i < board.rows(); i++) {
            for (int j = 0; j < board.cols(); j++) {
                boolean[] marked = new boolean[board.rows()*board.cols()];
                dfs(board, i, j, marked, "");
            }
        }
        return result;
    }

    private void dfs(BoggleBoard board, int row, int col, boolean[] marked, 
                     String pre) {
        if (marked[row*board.cols() + col]) return;
        marked[row*board.cols() + col] = true;
        String letter = "";
        if (board.getLetter(row, col) == 'Q')
            letter = "QU";
        else letter += board.getLetter(row, col);
        String prev = pre + letter;
        if (!dic.isPrefix(prev)) return;
        if (dic.contains(prev)) result.add(prev);
        if (row != 0) 
            dfs(board, row - 1, col, marked.clone(), prev);
        if (row != board.rows() - 1) 
            dfs(board, row + 1, col, marked.clone(), prev);
        if (col != 0)  
            dfs(board, row, col - 1, marked.clone(), prev);
        if (col != board.cols() - 1) 
            dfs(board, row, col + 1, marked.clone(), prev);       
        if ((row != 0) && (col != 0)) 
            dfs(board, row - 1, col - 1, marked.clone(), prev);
        if ((row != 0) && (col != board.cols() - 1)) 
            dfs(board, row - 1, col + 1, marked.clone(), prev);
        if ((row != board.rows() - 1) && (col != 0)) 
            dfs(board, row + 1, col - 1, marked.clone(), prev);
        if ((row != board.rows() - 1) && (col != board.cols() - 1)) 
            dfs(board, row + 1, col + 1, marked.clone(), prev);
    }

    // Returns the score of the given word if it is in the dictionary, zero 
    // otherwise. (You can assume the word contains only the uppercase 
    // letters A through Z.)
    public int scoreOf(String word) {
        if (!dic.contains(word)) return 0;
        return dic.get(word);
    }

    public static void main(String[] args) {
        In in = new In(args[0]);
        String[] dictionary = in.readAllStrings();
        BoggleSolver solver = new BoggleSolver(dictionary);
        BoggleBoard board = new BoggleBoard(args[1]);
        int score = 0;
        for (String word : solver.getAllValidWords(board)) {
            StdOut.println(word);
            score += solver.scoreOf(word);
        }
        StdOut.println("Score = " + score);
    }
}