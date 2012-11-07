all:
	g++ -o snappytest -O2 -march=corei7 main.cpp -llzo2 -lboost_system -lboost_chrono -lsnappy -lrt -static -fdata-sections -ffunction-sections -Wl,--gc-sections -s
