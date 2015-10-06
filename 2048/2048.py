"""
Clone of 2048 game.
"""

import poc_2048_gui
import random

# Directions, DO NOT MODIFY
UP = 1
DOWN = 2
LEFT = 3
RIGHT = 4

# Offsets for computing tile indices in each direction.
# DO NOT MODIFY this dictionary.    
OFFSETS = {UP: (1, 0), 
           DOWN: (-1, 0), 
           LEFT: (0, 1), 
           RIGHT: (0, -1)} 
   
def merge(line):
    """
    Helper function that merges a single row or column in 2048
    """
    rst_line=[]
    for number in line:
        if number:
            rst_line.append(number)
    new_line=[]
    # If the last one is added
    if len(rst_line)>1:
        index=0
        final=True
        while index<len(rst_line)-1:
            if rst_line[index]==rst_line[index+1]:
                new_line.append(rst_line[index]*2)
                if index+1==len(rst_line)-1:
                    final=False
                index+=2
            else:
                new_line.append(rst_line[index])
                index+=1
        if final:
            new_line.append(rst_line[-1])
    elif len(rst_line)==1:
        new_line=rst_line
    new_line+=[0]*(len(line)-len(new_line))
    return new_line

class TwentyFortyEight:
    """
    Class to run the game logic.
    """

    def __init__(self, grid_height, grid_width):
        self.grid_height=grid_height
        self.grid_width=grid_width
        self.reset()
        self.initial_tiles={UP:[[0,col] for col in xrange(grid_width)],
                            DOWN:[[grid_height-1,col] for col in xrange(grid_width)],
                            LEFT:[[row,0] for row in xrange(grid_height)],
                            RIGHT:[[row,grid_width-1] for row in xrange(grid_height)]}
    
    def reset(self):
        """
        Reset the game so the grid is empty.
        """
        self.cells =[[0 for dummy_col in xrange(self.grid_width)] for dummy_row in xrange(self.grid_height)]
    
    def __str__(self):
        """
        Return a string representation of the grid for debugging.
        """
        return ",".join(self.cells)

    def get_grid_height(self):
        """
        Get the height of the board.
        """
        return self.grid_height
    
    def get_grid_width(self):
        """
        Get the width of the board.
        """
        return self.grid_width
                            
    def move(self, direction):
        """
        Move all tiles in the given direction and add
        a new tile if any tiles moved.
        """
        moved=False
        for initial_tile in self.initial_tiles[direction]:
            old_line=[]
            row=initial_tile[0]
            col=initial_tile[1]
            while -1<row<self.grid_height and -1<col<self.grid_width:
                old_line.append(self.cells[row][col])
                row+=OFFSETS[direction][0]
                col+=OFFSETS[direction][1]
            new_line=merge(old_line)
            row=initial_tile[0]
            col=initial_tile[1]
            for number in new_line:
                self.cells[row][col]=number
                row+=OFFSETS[direction][0]
                col+=OFFSETS[direction][1]
            if new_line!=old_line:
                moved=True
        if moved:
            self.new_tile()
        
    def new_tile(self):
        """
        Create a new tile in a randomly selected empty 
        square.  The tile should be 2 90% of the time and
        4 10% of the time.
        """
        empty_squares=[]
        for row in xrange(self.grid_height):
            for col in xrange(self.grid_width):
                if not self.cells[row][col]:
                    empty_squares.append([row,col])
        if empty_squares!=[]:
            choice=random.choice(empty_squares)
            self.cells[choice[0]][choice[1]]=random.choice([2]*9+[4])
        
    def set_tile(self, row, col, value):
        """
        Set the tile at position row, col to have the given value.
        """        
        self.cells[row][col]=value

    def get_tile(self, row, col):
        """
        Return the value of the tile at position row, col.
        """        
        return self.cells[row][col]
 
poc_2048_gui.run_gui(TwentyFortyEight(4, 4))