all:
	g++ -pipe -g -o bwt -O2 -march=corei7 -ffast-math -std=c++0x -static main.cpp -lboost_system -lboost_thread -pthread
