/* 
 * File:   snappy.hpp
 * Author: uli
 *
 * Created on 6. November 2012, 19:14
 */

#ifndef SNAPPY_HPP
#define	SNAPPY_HPP

#include "snappy.h"
#include <cstdio>
#include <string>

using namespace std;

/**
 * Compresses a given file using Snappy with a given blocksize and returns the compressed size
 * @param blocksize
 * @param infileName
 * @return 
 */
size_t getSnappyCompressedSize(uint32_t blocksize, const char* infileName) {
    FILE* inFD = fopen(infileName, "r");
    size_t outputSize = 0;
    //Read the file
    char* buf = new char[blocksize];
    while (true) {
        size_t read = fread(buf, 1, blocksize, inFD);
        if (read == 0) {
            break;
        } //Stop after last block
        string output;
        snappy::Compress(buf, read, &output);
        //Accumulate the compressed length
        outputSize += output.size();
    }
    fclose(inFD);
    delete[] buf;
    return outputSize;  
}

#endif	/* SNAPPY_HPP */

