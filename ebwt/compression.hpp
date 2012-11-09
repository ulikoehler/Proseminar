/* 
 * File:   compression.hpp
 * Author: uli
 *
 * Created on 9. November 2012, 23:27
 */

#ifndef COMPRESSION_HPP
#define	COMPRESSION_HPP

//libhuffman
#include "../huffman/huffman.h"
//LZO
#include <lzo/lzo1x.h>
//Snappy
#include <snappy.h>
//LZ4
#include "../lz4/lz4.h"

unsigned char* lzoX1Wmem[LZO1X_1_11_MEM_COMPRESS];

size_t getHuffmanOutputSize(const char* input, size_t inlen) {
    char* huffout;
    unsigned int outlen;
    huffman_encode_memory(input, inlen, &huffout, &outlen);
    //malloc is used, so we need to use free instead of delete
    //Although this doesn't matter for most compilers, it makes the code more portable
    free(huffout);
    return outlen;
}

size_t getLZO1X11OutputSize(const char* input, size_t inlen) {
    unsigned char* outputData = new unsigned char[inlen + (inlen / 16 + 64 + 3)]; //With some reserves for worst case
    lzo_uint currentCompressedSize;
    lzo1x_1_11_compress((unsigned char*) input, read, outputData, &currentCompressedSize, lzoX1Wmem);
    delete[] outputData;
    return currentCompressedSize;
}

size_t getSnappyOutputSize(const char* input, size_t inlen) {
    char* outputData = new char[snappy::MaxCompressedLength(inlen)];
    size_t outputSize;
    snappy::RawCompress(input, inlen, outputData, &outputSize);
    delete[] outputData;
    return outputSize;
}

size_t getLZ4OutputSize(const char* input, size_t inlen) {
    char* outputData = new char[LZ4_compressBound(inlen)];
    int lz4CompressedSize = LZ4_compress((char*)input, (char*)outputData, inlen);
    delete[] outputData;
    return lz4CompressedSize;
}



#endif	/* COMPRESSION_HPP */

