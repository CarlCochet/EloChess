# EloChess

Hello there!

This is a little program that DOES NOT calculate performance in a tournament. It calculates the elo evolution of the players
participating to the tournament based on the FIDE formulas. 

All you need is a .pgn file with all the games from the tournament, and the starting elo of the players. 


There is a elochess.exe ../cmake-build-debug, if it does not work it probably means you will have to compile it yourself using the
MakeFile that comes with the software. 


You have 2 options when you run the program:

1. You let the program read the starting elos from the .pgn file. It's easier because it's automatic, but if the elos in the .pgn file
are wrong or badly initialised, you will get bad results at the end.

2. You can edit the elo.elo file that contains some numbers. These were elos used in the CCC2 with 33 engines participating.
In this file you can manually set the elos, but they need to be entered in the order the games are stored in the .pgn. For instance,
in a pgn where the first match is Carlsen (white) vs Caruana (black), you have to put this in the elo.elo file:

2839

2827
...

To sum this up, just put the elos in the order of the apparition of the players in the .pgn file. 
