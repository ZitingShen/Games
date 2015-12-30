# starting point for makefile for lab 5: you need to implement this
# from scratch
#
# remember that the <TAB> character is necessary before the command in
# target entries: 
#
# target: dependencies
# <TAB> command
#
# you obviously cannot easily see the <TAB> char, but if your makefile 
# isn't working with weird errors, check that you have a tab and not 
# space characters before the command part
#
# you should implement all and clean rules for the commands:
#   make
#   make clean
# also define and use makefile variables 
# use makefiles I've given you as a guide 
#


all: gol faster gol_parallel
	
gol: gol.c 
	gcc -g -Wall -o gol gol.c

faster: faster.c
	gcc -g -Wall -o faster faster.c

gol_parallel: gol_parallel.c
	gcc -g -Wall -o gol_parallel gol_parallel.c

clean:
	$(RM) gol faster gol_parallel

