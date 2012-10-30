all:
	g++ -o bwt -march=corei7 -ffast-math -std=c++0x -static main.cpp
