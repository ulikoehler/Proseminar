all:
	g++ -pipe -o bwt -O2 -march=corei7 -ffast-math -std=c++0x -static main.cpp -ffunction-sections -fdata-sections -Wl,--gc-sections
	strip bwt
