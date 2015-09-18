/*
----------------------------------------------------
                 Project Two
                   CSC 389
                  Fall 2014
	          Craig Swearingen
	             19 Dec 2014
----------------------------------------------------
*/

----------------------------------------------------

Program Description

This program simulates a unisex bathroom. Males and 
females aren't allowed to be in at the same time.
It can be ran either in automatic mode or manual.
If ran in manual a number(20-100) is passed via the command
line for how many people should be in the line. 
Ex. $./projectTwo 25
If in manual a user can add people to the line, let them
in the bathroom, or let them leave the bathroom

----------------------------------------------------
To compile and run

1. Extract projectTwo compressed file

2. $make

3. $./projectTwo
	or
3. $./projectTwo n
	where n is a number from 20-100

----------------------------------------------------
To delete files made during compilation and recompile

1. $make cleanall

2. $make

3. $./projectTwo
