"""
Monte Carlo Tic-Tac-Toe Player
"""

import random
import poc_ttt_gui
import poc_ttt_provided as provided

# Constants for Monte Carlo simulator
# Change as desired
NTRIALS = 100  # Number of trials to run
MCMATCH = 1.0  # Score for squares played by the machine player
MCOTHER = 1.0  # Score for squares played by the other player

       
def mc_trial(board, player):
    """
    This function start with the given player by making random 
    moves and alternates the player.
    """
    while not board.check_win():
        position=random.choice(board.get_empty_squares())
        board.move(position[0],position[1],player)
        player=provided.switch_player(player)
    
def mc_update_scores(scores, board, player):
    """
    The function should score the completed board and update 
    the scores grid.
    """
    # The player represents which the machine player is.
    if board.check_win()==player:
        for row in xrange(board.get_dim()):
            for col in xrange(board.get_dim()):
                if board.square(row, col)==player:
                    scores[row][col]+=MCMATCH
                elif board.square(row, col)==provided.switch_player(player):
                    scores[row][col]-=MCOTHER
    elif board.check_win()==provided.switch_player(player):
        for row in xrange(board.get_dim()):
            for col in xrange(board.get_dim()):
                if board.square(row, col)==provided.switch_player(player):
                    scores[row][col]+=MCOTHER
                elif board.square(row, col)==player:
                    scores[row][col]-=MCMATCH
    
def get_best_move(board, scores):
    """
    The function finds all of the empty squares with the maximum 
    score and randomly return one of them as a (row, column) tuple.
    """
    # Cannot call this function with a full board.
    max_score=-max(MCMATCH,MCOTHER)*NTRIALS-1
    choices=[]
    for position in board.get_empty_squares():
        if scores[position[0]][position[1]]>max_score:
            max_score=scores[position[0]][position[1]]
            choices=[position]
        elif scores[position[0]][position[1]]==max_score:
            choices.append(position)
    return random.choice(choices)
    
def mc_move(board, player, trials):
    """
    The function returns a move for the machine player.
    """
    scores=[[0.0 for dummy_col in xrange(board.get_dim())] for dummy_row in xrange(board.get_dim())]
    for dummy_trial in xrange(trials):
        trial_board=board.clone()
        mc_trial(trial_board, player)
        mc_update_scores(scores, trial_board, player)
    return get_best_move(board, scores)

# provided.play_game(mc_move, NTRIALS, False)        
# poc_ttt_gui.run_gui(3, provided.PLAYERX, mc_move, NTRIALS, False)
