#include <snappy.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include "../ebwt/util.hpp"

using namespace std;

size_t getFilesizeInBytes(const char* filename) {
    struct stat filestatus;
    stat(filename, &filestatus);
    return filestatus.st_size;
}

int main(int argc, char** argv) {
	if(argc < 3) {
		cout << "This program tests the effect of various blocksizes on Snappy" << endl;
		cout << "Usage: " << argv[0] << " <input file> <blocksize resolution>" << endl;
		return 1;
	}
	string infile(argv[1]);
	int blocksizeRes = atoi(argv[2]);
	uint32_t originalSize = getFilesizeInBytes(infile.c_str());
	cout << "Size of " << infile << ": " << originalSize << endl;
	string statsOutFile = "snappy.stats." + getFilenameFromPath(infile) + ".txt";
	ofstream statsOut(statsOutFile.c_str());
	statsOut << "Blocksize,RelSize" << endl;
	//Check
	for( int blocksize = blocksizeRes; blocksize <= originalSize; blocksize += blocksizeRes) {
		FILE* inFD = fopen(infile.c_str(), "r");
		size_t outputSize = 0;
		cout << "Compressing " << infile << " (blocksize " << blocksize << ")" << endl;
		//Read the file
		char* buf = new char[blocksize];
		while (true) {
			size_t read = fread(buf, 1, blocksize, inFD);
			if(read == 0) {break;} //Stop after last block
			string output;
			snappy::Compress(buf, read, &output);
			//Accumulate the compressed length
			outputSize += output.size();
		}
		delete[] buf;
		fclose(inFD);
		//Write the filesize
		statsOut << blocksize << ',' << (outputSize / (double)originalSize) << endl;
	}
	statsOut.close();
	return 0;
}