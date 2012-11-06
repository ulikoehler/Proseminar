all:
	g++ -o snappytest -O2 -march=corei7 main.cpp -lsnappy -static -fdata-sections -ffunction-sections -Wl,--gc-sections -s
