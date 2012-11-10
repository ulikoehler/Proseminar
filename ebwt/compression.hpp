/* 
 * File:   compression.hpp
 * Author: uli
 *
 * Created on 9. November 2012, 23:27
 */

#ifndef COMPRESSION_HPP
#define	COMPRESSION_HPP

#include <cstdlib>
#include <iostream>
//libhuffman
#ifndef NO_HUFFMAN
#include "../huffman/huffman.h"
#endif
//LZO
#ifndef NO_LZO
#include <lzo/lzo1x.h>
#endif
//Snappy
#ifndef NO_SNAPPY
#include <snappy.h>
#endif
//LZ4
#ifndef NO_LZ4
#include "../lz4/lz4.h"
#endif
//Deflate
#ifndef NO_DEFLATE
#include <zlib.h>
#endif

#ifndef NO_LZO
unsigned char* lzo1X11Wmem[LZO1X_1_11_MEM_COMPRESS];
#endif


#ifndef NO_HUFFMAN

size_t getHuffmanOutputSize(const char* input, size_t inlen) {
    unsigned char* huffout;
    unsigned int outlen;
    huffman_encode_memory((const unsigned char*) input, inlen, &huffout, &outlen);
    //malloc is used, so we need to use free instead of delete
    //Although this doesn't matter for most compilers, it makes the code more portable
    free(huffout);
    return outlen;
}
#endif


#ifndef NO_LZO

size_t getLZO1X11OutputSize(const char* input, size_t inlen) {
    unsigned char* outputData = new unsigned char[inlen + (inlen / 16 + 64 + 3)]; //With some reserves for worst case
    lzo_uint currentCompressedSize;
    lzo1x_1_11_compress((unsigned char*) input, inlen, outputData, &currentCompressedSize, lzo1X11Wmem);
    delete[] outputData;
    return currentCompressedSize;
}
#endif


#ifndef NO_SNAPPY

size_t getSnappyOutputSize(const char* input, size_t inlen) {
    char* outputData = new char[snappy::MaxCompressedLength(inlen)];
    size_t outputSize;
    snappy::RawCompress(input, inlen, outputData, &outputSize);
    delete[] outputData;
    return outputSize;
}
#endif

#ifndef NO_LZ4

size_t getLZ4OutputSize(const char* input, size_t inlen) {
    char* outputData = new char[LZ4_compressBound(inlen)];
    int lz4CompressedSize = LZ4_compress((char*) input, (char*) outputData, inlen);
    delete[] outputData;
    return lz4CompressedSize;
}
#endif

#ifndef NO_DEFLATE

size_t getDeflateOutputSize(const char* input, size_t inlen) {
    char* out = new char[inlen + 128];
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = (uInt) inlen;
    strm.next_in = (Bytef *) input;
    strm.avail_out = (uInt) inlen + 128;
    strm.next_out = (Bytef *) out; //Allocated after deflateInit() because we then know the maximum size
    deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    //After initing, we can ask zlib to tell us the maximum possible output size
    uInt maxOutputSize = deflateBound(&strm, inlen);
    if (maxOutputSize > inlen + 128) {
        std::cerr << "zlib exceeded estimated size by " << (maxOutputSize - (inlen + 128)) << " bytes " << std::endl;
        delete[] out;
        out = new char[maxOutputSize];
        strm.next_out = (Bytef *) out;
        strm.avail_out = maxOutputSize;
    }

    //Compress
    deflate(&strm, Z_FINISH);
    deflateEnd(&strm);
    int compressedSize = (char *) strm.next_out - out;
    delete[] out;
    return compressedSize;
}
#endif


#endif	/* COMPRESSION_HPP */

