all:
	g++ -o snappytest -O2 -march=corei7 main.cpp ../lz4/lz4.c ../lz4/lz4hc.c -llzo2 -lboost_system -lboost_chrono -lsnappy -lrt -static -fdata-sections -ffunction-sections -Wl,--gc-sections -s
