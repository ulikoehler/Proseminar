#include "Stopwatch.hpp"
#include <cstdio>
#include <snappy.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <lzo/lzo1x.h>
#include <lzo/lzo1x.h>
#include "../ebwt/util.hpp"

using namespace std;

size_t getFilesizeInBytes(const char* filename) {
    struct stat filestatus;
    stat(filename, &filestatus);
    return filestatus.st_size;
}

int main(int argc, char** argv) {
	if (lzo_init() != LZO_E_OK)
	{
	    cout << "LZO init failed! Better be worried!" << endl;
	    return 4;
	}
	if(argc < 3) {
		cout << "This program tests the effect of various blocksizes on LZO and snappy" << endl;
		cout << "Usage: " << argv[0] << " <input file> <blocksize resolution> <max blocksize>" << endl;
		return 1;
	}
	string infile(argv[1]);
	int blocksizeRes = atoi(argv[2]);
	int blocksizeMax = atoi(argv[3]);
	uint32_t originalSize = getFilesizeInBytes(infile.c_str());
	//Initialize the compressionstatistics output
	string compStatsOutFile = "snappy.compression." + getFilenameFromPath(infile) + ".txt";
	ofstream compStatsOut(compStatsOutFile.c_str());
	compStatsOut << "Blocksize,Algorithm,RelSize" << endl;
	//Initialize the compression timer statistics output
	string compTimerStatsOutFile = "snappy.comptime." + getFilenameFromPath(infile) + ".txt";
	ofstream compTimerStatsOut(compTimerStatsOutFile.c_str());
	compTimerStatsOut << "Blocksize,Algorithm,ms" << endl;
	//Initialize the decompression timer statistics output
	string decompTimerStatsOutFile = "snappy.decomptime." + getFilenameFromPath(infile) + ".txt";
	ofstream decompTimerStatsOut(decompTimerStatsOutFile.c_str());
	decompTimerStatsOut << "Blocksize,Algorithm,ms" << endl;
	//Initialize working memory for LZO
	unsigned char * lzoX1Wmem = new unsigned char[LZO1X_1_11_MEM_COMPRESS];
	unsigned char * lzoX999Wmem = new unsigned char[LZO1X_999_MEM_COMPRESS];
	//Iterate ove rall
	for( int blocksize = blocksizeRes; blocksize <= blocksizeMax; blocksize += blocksizeRes) {
		Stopwatch lzo1x11CompressionStopwatch;
		Stopwatch lzo1x999CompressionStopwatch;
		Stopwatch snappyCompressionStopwatch;
		Stopwatch lzo1x11DecompressionStopwatch;
		Stopwatch lzo1x999DecompressionStopwatch;
		Stopwatch snappyDecompressionStopwatch;
		//Initialize the LZO output data (for both LZO1X and LZOX 999)
		unsigned char * outputData = new unsigned char[blocksize + (blocksize / 16 + 64 + 3)]; //Compression output data with some reserve
		unsigned char * decompOutputData = new unsigned char[blocksize + (blocksize / 16 + 64 + 3)]; //Decompression data with some reserve
		//Accumulators for the compressed output sizes
		size_t snappyOutputSize = 0;
		size_t lzo1X11OutputSize = 0;
		size_t lzo1X999OutputSize = 0;
		//Open the input file
		FILE* inFD = fopen(infile.c_str(), "r");
		//Read the file
		char* buf = new char[blocksize];
		while (true) {
			size_t read = fread(buf, 1, blocksize, inFD);
			if(read == 0) {break;} //Stop after last block
			//
			// Snappy - raw-compress to avoid string allocation overhead
			//
			//Compress
			size_t currentCompressedSizeSizeT = 0;
			snappyCompressionStopwatch.start();
			snappy::RawCompress(buf, read, (char*)outputData, &currentCompressedSizeSizeT);
			snappyCompressionStopwatch.stop();
			//Accumulate the compressed length
			snappyOutputSize += currentCompressedSizeSizeT;
			//Decompress
			snappyDecompressionStopwatch.start();
			snappy::RawUncompress((char*)outputData, currentCompressedSizeSizeT, (char*)decompOutputData);
			snappyDecompressionStopwatch.stop();
			//
			// LZO 1X 11
			//
			lzo_uint currentCompressedSize = 0;
			//Compress
			lzo1x11CompressionStopwatch.start();
			lzo1x_1_11_compress((unsigned char*)buf, read, outputData, &currentCompressedSize, lzoX1Wmem );
			lzo1x11CompressionStopwatch.stop();
			//Accumulate the compressed length
			lzo1X11OutputSize += currentCompressedSize;
			//Decompress
			lzo1x11DecompressionStopwatch.start();
			lzo1x_decompress(outputData, currentCompressedSize, decompOutputData, &currentCompressedSize, NULL);
			lzo1x11DecompressionStopwatch.stop();
			//
			// LZO 1X 999
			//
			lzo1x999CompressionStopwatch.start();
			lzo1x_999_compress((unsigned char*)buf, read, outputData, &currentCompressedSize, lzoX999Wmem );
			lzo1x999CompressionStopwatch.stop();
			//Accumulate the compressed length
			lzo1X999OutputSize += currentCompressedSize;
			//Decompress
			lzo1x999DecompressionStopwatch.start();
			lzo1x_decompress( outputData, currentCompressedSize, decompOutputData, &currentCompressedSize, NULL);
			lzo1x999DecompressionStopwatch.stop();
		}
		delete[] buf;
		delete[] outputData;
		fclose(inFD);
		//Write the compression size statistics
		compStatsOut << blocksize << ",Snappy," << (snappyOutputSize / (double)originalSize) << endl;
		compStatsOut << blocksize << ",LZO1X11," << (lzo1X11OutputSize / (double)originalSize) << endl;
		compStatsOut << blocksize << ",LZO1X999," << (lzo1X999OutputSize / (double)originalSize) << endl;
		//Write the compression time statistics
		compTimerStatsOut << blocksize << ",Snappy," << snappyCompressionStopwatch.getMicroseconds()/1000.0 << endl;
		compTimerStatsOut << blocksize << ",LZO1X11," << lzo1x11CompressionStopwatch.getMicroseconds()/1000.0 << endl;
		compTimerStatsOut << blocksize << ",LZO1X999," << lzo1x999CompressionStopwatch.getMicroseconds()/1000.0 << endl;
		//Write the decompression time statistics
		decompTimerStatsOut << blocksize << ",Snappy," << lzo1x11DecompressionStopwatch.getMicroseconds()/1000.0 << endl;
		decompTimerStatsOut << blocksize << ",LZO1X11," << lzo1x11DecompressionStopwatch.getMicroseconds()/1000.0 << endl;
		decompTimerStatsOut << blocksize << ",LZO1X999," << lzo1x999DecompressionStopwatch.getMicroseconds()/1000.0 << endl;
	}
	compStatsOut.close();
	compTimerStatsOut.close();
	decompTimerStatsOut.close();
	return 0;
}
