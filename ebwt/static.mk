all:
	g++ -pipe -o bwt -O2 -march=corei7 -ffast-math -std=c++0x -static main.cpp ../huffman/huffman.c ../lz4/lz4.c -ffunction-sections -fdata-sections -Wl,--gc-sections -lboost_thread -lboost_system -pthread -lboost_program_options -lsnappy -llzo2 -ltcmalloc -s
