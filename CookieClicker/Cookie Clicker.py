"""
Cookie Clicker Simulator
"""

import simpleplot
import math

# Used to increase the timeout, if necessary
import codeskulptor
codeskulptor.set_timeout(20)

import poc_clicker_provided as provided

# Constants
SIM_TIME = 10000000000.0

class ClickerState:
    """
    Simple class to keep track of the game state.
    """
    
    def __init__(self):
        self._total_cookies=0.0
        self._current_cookies=0.0
        self._current_time=0.0
        self._current_cps=1.0
        self._history_list=[(0.0, None, 0.0, 0.0)]
        
    def __str__(self):
        """
        Return human readable state
        """
        return """Time: %s
        Current Cookies: %s
        CPS: % s
        Total cookies: %s""" % (self._current_time,self._current_cookies,self._current_cps,self._total_cookies)
        
    def get_cookies(self):
        """
        Return current number of cookies 
        (not total number of cookies)
        
        Should return a float
        """
        return self._current_cookies
    
    def get_cps(self):
        """
        Get current CPS

        Should return a float
        """
        return self._current_cps
    
    def get_time(self):
        """
        Get current time

        Should return a float
        """
        return self._current_time
    
    def get_history(self):
        """
        Return history list

        History list should be a list of tuples of the form:
        (time, item, cost of item, total cookies)

        For example: (0.0, None, 0.0, 0.0)
        """
        return self._history_list

    def time_until(self, cookies):
        """
        Return time until you have the given number of cookies
        (could be 0 if you already have enough cookies)

        Should return a float with no fractional part
        """
        time=0.0
        if self._current_cookies<cookies:
            time=(cookies-self._current_cookies)/self._current_cps
            time=float(math.ceil(time))
        return time
    
    def wait(self, time):
        """
        Wait for given amount of time and update state

        Should do nothing if time <= 0
        """
        if time>0:
            self._current_time+=time
            self._current_cookies+=time*self._current_cps
            self._total_cookies+=time*self._current_cps
    
    def buy_item(self, item_name, cost, additional_cps):
        """
        Buy an item and update state

        Should do nothing if you cannot afford the item
        """
        if self._current_cookies>=cost:
            self._current_cookies-=cost
            self._current_cps+=additional_cps
            self._history_list.append((self._current_time,item_name,cost,self._total_cookies))
   
    
def simulate_clicker(build_info, duration, strategy):
    """
    Function to run a Cookie Clicker game for the given
    duration with the given strategy.  Returns a ClickerState
    object corresponding to game.
    """
    items=build_info.clone()
    state=ClickerState()
    item=""
    while duration>=state.get_time():
        item=strategy(state.get_cookies(),state.get_cps(),duration-state.get_time(),items)
        if item:
            if state.time_until(items.get_cost(item))<=duration-state.get_time():
                state.wait(state.time_until(items.get_cost(item)))
                state.buy_item(item,items.get_cost(item),items.get_cps(item))
                items.update_item(item)
            else:
                break
        else:
            break
    state.wait(duration-state.get_time())
    return state


def strategy_cursor(cookies, cps, time_left, build_info):
    """
    Always pick Cursor!

    Note that this simplistic strategy does not properly check whether
    it can actually buy a Cursor in the time left.  Your strategy
    functions must do this and return None rather than an item you
    can't buy in the time left.
    """
    return "Cursor"

def strategy_none(cookies, cps, time_left, build_info):
    """
    Always return None

    This is a pointless strategy that you can use to help debug
    your simulate_clicker function.
    """
    return None

def strategy_cheap(cookies, cps, time_left, build_info):
    """
    Always buy the cheapest item.
    """
    cost=0.0
    item=None
    for stuff in build_info.build_items():
        if build_info.get_cost(stuff)<=cookies+time_left*cps:
            if not cost or build_info.get_cost(stuff)<=cost:
                cost=build_info.get_cost(stuff)
                item=stuff
    return item

def strategy_expensive(cookies, cps, time_left, build_info):
    """
    Always buy the most expensive item.
    """
    cost=0.0
    item=None
    for stuff in build_info.build_items():
        if build_info.get_cost(stuff)<=cookies+time_left*cps:
            if build_info.get_cost(stuff)>=cost:
                cost=build_info.get_cost(stuff)
                item=stuff
    return item

def strategy_best(cookies, cps, time_left, build_info):
    """
    Find a way to get the most total cookies.
    """
    min_cost=0.0
    min_item=None
    max_cost=0.0
    max_item=None
    for stuff in build_info.build_items():
        if build_info.get_cost(stuff)<=cookies+time_left*cps:
            if not min_cost or build_info.get_cost(stuff)<=min_cost:
                min_cost=build_info.get_cost(stuff)
                min_item=stuff
            if not max_cost or build_info.get_cost(stuff)>=max_cost:
                max_cost=build_info.get_cost(stuff)
                max_item=stuff
    if min_cost and max_cost/min_cost<=65.0:
        return max_item
    else:
        return min_item
        
def run_strategy(strategy_name, time, strategy):
    """
    Run a simulation with one strategy
    """
    state = simulate_clicker(provided.BuildInfo(), time, strategy)
    print strategy_name, ":", state

    # Plot total cookies over time

    # Uncomment out the lines below to see a plot of total cookies vs. time
    # Be sure to allow popups, if you do want to see it

    #history = state.get_history()
    #history = [(item[0], item[3]) for item in history]
    #simpleplot.plot_lines(strategy_name, 1000, 400, 'Time', 'Total Cookies', [history], True)

def run():
    """
    Run the simulator.
    """    
    run_strategy("Cursor", SIM_TIME, strategy_cursor)

    # Add calls to run_strategy to run additional strategies
    #run_strategy("Cheap", SIM_TIME, strategy_cheap)
    #run_strategy("Expensive", SIM_TIME, strategy_expensive)
    #run_strategy("Best", SIM_TIME, strategy_best)
    
#run()
    

