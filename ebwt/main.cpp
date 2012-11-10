/* 
 * File:   main.cpp
 * Author: uli
 *
 * Created on 29. Oktober 2012, 20:45
 */

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <functional>
#include <boost/program_options.hpp>
#include "mtf.hpp"
#include "util.hpp"
#include "rle.hpp"
#include "compression.hpp"
#include "Stopwatch.hpp"

using namespace std;
namespace po = boost::program_options;
//Heapsort implementation from:
//http://www.algorithmist.com/index.php/Heap_sort.c

struct Sort {
    unsigned int index;
    unsigned int slen;
    const char* m;

    Sort(unsigned int i) {
        this->index = i;
    }

    Sort() {
    }

    const char* operator*() {
        return m + index;
    }

    bool operator==(Sort & other) {
        return memcmp(m + index, *other, slen) == 0;
    }

    //Compare with a raw pointer

    bool operator==(const char* ptr) {
        return memcmp(m + index, ptr, slen) == 0;
    }

    bool operator<(Sort & other) {
        return memcmp(m + index, *other, slen) < 0;
    }

    bool operator<=(Sort & other) {
        return memcmp(m + index, *other, slen) <= 0;
    }

    bool operator>(Sort & other) {
        return memcmp(m + index, *other, slen) > 0;
    }

    bool operator>=(Sort & other) {
        return memcmp(m + index, *other, slen) >= 0;
    }

    string toString() {
        string ret(m + index, slen);
        return ret;
    }
};

template<typename T>
void heapSort(T toSort[], int size) {
    int i;
    T temp;

    for (i = (size / 2); i >= 0; i--) {
        siftDown(toSort, i, size - 1);
    }

    for (i = size - 1; i >= 1; i--) {
        // Swap
        temp = toSort[0];
        toSort[0] = toSort[i];
        toSort[i] = temp;

        siftDown(toSort, 0, i - 1);
    }
}

template<typename T>
void siftDown(T numbers[], int root, int bottom) {
    int maxChild = root * 2 + 1;

    // Find the biggest child
    if (maxChild < bottom) {
        int otherChild = maxChild + 1;
        // Reversed for stability
        maxChild = (numbers[otherChild] > numbers[maxChild]) ? otherChild : maxChild;
    } else {
        // Don't overflow
        if (maxChild > bottom) return;
    }

    // If we have the correct ordering, we are done.
    if (numbers[root] >= numbers[maxChild]) {
        return;
    }

    // Swap
    T temp = numbers[root];
    numbers[root] = numbers[maxChild];
    numbers[maxChild] = temp;

    // Tail queue recursion. Will be compiled as a loop with correct compiler switches.
    siftDown(numbers, maxChild, bottom);
}

struct BWTTransformer {
    char* m;
    char* L;
    unsigned int datasize;
    unsigned int I;
    Sort* sortArray;

    BWTTransformer(unsigned int datasize) : datasize(datasize) {
        m = new char[2 * datasize];
        L = new char[datasize];
        sortArray = new Sort[datasize];
    }

    ~BWTTransformer() {
        delete[] m;
        delete[] L;
        delete[] sortArray;
    }

    //Resize this transformer. Deletes previous data

    void resize(unsigned int newSize) {
        //Delete old data
        delete[] m;
        delete[] L;
        delete[] sortArray;
        //Initialize new data
        this->datasize = newSize;
        m = new char[2 * newSize];
        L = new char[2 * newSize];
        sortArray = new Sort[newSize];
    }

    void bwt(const char* s) {
        //Double the string to allow in-place rotation building
        // (don't double the NUL terminator)
        memcpy(m, s, datasize);
        memcpy(m + datasize, s, datasize);
        //Build the sortable permutation wrappers
        for (int i = 0; i < datasize; i++) {
            sortArray[i].index = i;
            sortArray[i].m = m;
            sortArray[i].slen = datasize;
        }
        //Sort the Array
        heapSort(sortArray, datasize);
        //Copy the last column to L and find I in the process
        for (int i = 0; i < datasize; i++) {
            L[i] = (*sortArray[i])[datasize - 1];
            if (sortArray[i] == s) {
                I = i;
            }
        }
    }
};

struct EBWTStatisticsDataset {

    EBWTStatisticsDataset() {
        //Huffman
        bwtHuffmanSize = 0;
        bwtMtfHuffmanSize = 0;
        mtfHuffmanSize = 0;
        rleHuffmanSize = 0;
        bwtRleHuffmanSize = 0;
        //LZO
        bwtLZOSize = 0;
        bwtMtfLZOSize = 0;
        mtfLZOSize = 0;
        rleLZOSize = 0;
        bwtRleLZOSize = 0;
        //Snappy
        bwtSnappySize = 0;
        bwtMtfSnappySize = 0;
        mtfSnappySize = 0;
        rleSnappySize = 0;
        bwtRleSnappySize = 0;
        //LZ4
        bwtLZ4Size = 0;
        bwtMtfLZ4Size = 0;
        mtfLZ4Size = 0;
        rleLZ4Size = 0;
        bwtRleLZ4Size = 0;
        //None
        rleOnlySize = 0;
        bwtRleSize = 0;
        //Only compression
        huffSize = 0;
        lzoSize = 0;
        snappySize = 0;
        lz4Size = 0;
        deflateSize = 0;
        //Misc
        origFilesize = 0;
        //Timings
        snappyTime = 0.0;
        bwtMtfHuffmanTime = 0.0;
    }

    //Huffman
    size_t bwtHuffmanSize;
    size_t bwtMtfHuffmanSize;
    size_t mtfHuffmanSize;
    size_t rleHuffmanSize;
    size_t bwtRleHuffmanSize;
    //LZO
    size_t bwtLZOSize;
    size_t bwtMtfLZOSize;
    size_t mtfLZOSize;
    size_t rleLZOSize;
    size_t bwtRleLZOSize;
    //Snappy
    size_t bwtSnappySize;
    size_t bwtMtfSnappySize;
    size_t mtfSnappySize;
    size_t rleSnappySize;
    size_t bwtRleSnappySize;
    //LZ4
    size_t bwtLZ4Size;
    size_t bwtMtfLZ4Size;
    size_t mtfLZ4Size;
    size_t rleLZ4Size;
    size_t bwtRleLZ4Size;
    //None -- BWT, MTF and BWT+MTF size left out because these encodings don't change the sizes
    size_t rleOnlySize;
    size_t bwtRleSize;
    //Only compression
    size_t huffSize;
    size_t lzoSize;
    size_t snappySize;
    size_t lz4Size;
    size_t deflateSize;
    //Misc
    size_t origFilesize;
    //
    // Timings
    //
    double snappyTime;
    double bwtMtfHuffmanTime;
    double deflateTime;

    void write(std::ostream & out, unsigned int blocksize) {
        out
                //Only compression
                << blocksize << ',' << "CompressionOnly,Huffman" << ',' << huffSize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "CompressionOnly,LZO" << ',' << lzoSize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "CompressionOnly,Snappy" << ',' << snappySize / (double) origFilesize << "," << snappyTime << "\n"
                << blocksize << ',' << "CompressionOnly,LZ4" << ',' << lz4Size / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "CompressionOnly,Deflate" << ',' << deflateSize / (double) origFilesize << "," << deflateTime << "\n"
                //No compression
                << blocksize << ',' << "EncodingOnly,RLE" << ',' << rleOnlySize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "EncodingOnly,BWT+RLE" << ',' << bwtRleSize / (double) origFilesize << ",0.0\n"
                //Huffman
                << blocksize << ',' << "Huffman,BWT+Huffman" << ',' << bwtHuffmanSize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "Huffman,MTF+Huffman" << ',' << mtfHuffmanSize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "Huffman,BWT+MTF+Huffman" << ',' << bwtMtfHuffmanSize / (double) origFilesize << "," << bwtMtfHuffmanTime << "\n"
                << blocksize << ',' << "Huffman,RLE+Huffman" << ',' << rleHuffmanSize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "Huffman,BWT+RLE+Huffman" << ',' << bwtRleHuffmanSize / (double) origFilesize << ",0.0\n"
                //LZO
                << blocksize << ',' << "LZO,BWT+LZO" << ',' << bwtLZOSize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "LZO,BWT+MTF+LZO" << ',' << bwtMtfLZOSize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "LZO,MTF+LZO" << ',' << mtfLZOSize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "LZO,RLE+LZO" << ',' << rleLZOSize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "LZO,BWT+RLE+LZO" << ',' << bwtRleLZOSize / (double) origFilesize << ",0.0\n"
                //Snappy
                << blocksize << ',' << "Snappy,BWT+Snappy" << ',' << bwtSnappySize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "Snappy,BWT+MTF+Snappy" << ',' << bwtMtfSnappySize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "Snappy,MTF+Snappy" << ',' << mtfSnappySize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "Snappy,RLE+Snappy" << ',' << rleSnappySize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "Snappy,BWT+RLE+Snappy" << ',' << bwtRleSnappySize / (double) origFilesize << ",0.0\n"
                //LZ4
                << blocksize << ',' << "LZ4,BWT+LZ4" << ',' << bwtSnappySize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "LZ4,BWT+MTF+LZ4" << ',' << bwtMtfSnappySize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "LZ4,MTF+LZ4" << ',' << mtfSnappySize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "LZ4,RLE+LZ4" << ',' << rleSnappySize / (double) origFilesize << ",0.0\n"
                << blocksize << ',' << "LZ4,BWT+RLE+LZ4" << ',' << bwtRleSnappySize / (double) origFilesize << endl;
    }
};

size_t getFilesizeInBytes(const char* filename) {
    struct stat filestatus;
    stat(filename, &filestatus);
    return filestatus.st_size;
}

void bwtOnFile(const char* infile,
        unsigned int blocksize,
        EBWTStatisticsDataset& info) {
    FILE* inFD = fopen(infile, "r");
    //Initialize the transformer
    BWTTransformer transformer(blocksize);
    char* buf = new char[blocksize];
    //Initialize the stopwatches to measure how long BWT+MTF+Huff takes
    Stopwatch bwtMtfHuffStopwatch;
    Stopwatch snappyStopwatch;
    Stopwatch deflateStopwatch;
    //Loop over all blocks in the file
    while (true) {
        size_t read = fread(buf, 1, blocksize, inFD);
        if (read < blocksize) { //Last block
            if (read == 0) { //No data read
                break; //Stop immediately
            }
            //Resize the internal transformer variables from the last block
            transformer.resize(read);
        }
        //
        // Calculate the compression results for the uncompressed buffer
        //
        info.lzoSize += getLZO1X11OutputSize(buf, read);
        info.lz4Size += getLZ4OutputSize(buf, read);
        snappyStopwatch.start();

        info.snappySize += getSnappyOutputSize(buf, read);
        snappyStopwatch.stop();
        info.huffSize += getHuffmanOutputSize(buf, read);

        deflateStopwatch.start();
        info.deflateSize += getDeflateOutputSize(buf, read);
        deflateStopwatch.stop();
        //
        // Calculate MTF on the raw input
        //
        char* mtfBuffer = new char[read];
        moveToFrontEncodeAutoAlphabetCopy(buf, read, mtfBuffer);
        //Compress and calculate the sizes
        info.mtfLZOSize += getLZO1X11OutputSize(mtfBuffer, read);
        info.mtfLZ4Size += getLZ4OutputSize(mtfBuffer, read);
        info.mtfSnappySize += getSnappyOutputSize(mtfBuffer, read);
        info.mtfHuffmanSize += getHuffmanOutputSize(mtfBuffer, read);
        delete[] mtfBuffer;
        //
        // Calculate RLE on the raw input
        //
        char* rleBuffer = new char[read * 2]; //Maximum RLE size if no runs are found
        size_t rleSize = doRLE(buf, read, rleBuffer);
        //Compress and calculate the sizes
        info.rleOnlySize += rleSize;
        info.rleLZOSize += getLZO1X11OutputSize(rleBuffer, rleSize);
        info.rleLZ4Size += getLZ4OutputSize(rleBuffer, rleSize);
        info.rleSnappySize += getSnappyOutputSize(rleBuffer, rleSize);
        info.rleHuffmanSize += getHuffmanOutputSize(rleBuffer, rleSize);
        //
        // Calculate BWT --> transformer.L now contains BWT result (except of index)
        //
        bwtMtfHuffStopwatch.start();
        transformer.bwt(buf);
        bwtMtfHuffStopwatch.stop();
        //Compress and calculate the sizes
        info.bwtLZOSize += getLZO1X11OutputSize(transformer.L, transformer.datasize);
        info.bwtLZ4Size += getLZ4OutputSize(transformer.L, transformer.datasize);
        info.bwtSnappySize += getSnappyOutputSize(transformer.L, transformer.datasize);
        info.bwtHuffmanSize += getHuffmanOutputSize(transformer.L, transformer.datasize);
        //
        // Calculate BWT + MTF
        //
        bwtMtfHuffStopwatch.start();
        moveToFrontEncodeAutoAlphabetInPlace(transformer.L, read); //Auto-create the alphabet and MTF encode the BWT stuff
        bwtMtfHuffStopwatch.stop();
        //Compress and calculate the sizes
        info.bwtMtfLZOSize += getLZO1X11OutputSize(transformer.L, read);
        info.bwtMtfLZ4Size += getLZ4OutputSize(transformer.L, read);
        info.bwtMtfSnappySize += getSnappyOutputSize(transformer.L, read);
        bwtMtfHuffStopwatch.start();
        info.bwtMtfHuffmanSize += getHuffmanOutputSize(transformer.L, read);
        bwtMtfHuffStopwatch.stop();
        //
        // Calculate BWT + RLE
        //
        //Reuse the RLE-buffer from the RLE-only encoding
        size_t bwtRleSize = doRLE(buf, read, rleBuffer);
        //Compress and calculate the sizes
        info.bwtRleSize += bwtRleSize;
        info.bwtRleLZOSize += getLZO1X11OutputSize(rleBuffer, bwtRleSize);
        info.bwtRleLZ4Size += getLZ4OutputSize(rleBuffer, bwtRleSize);
        info.bwtRleSnappySize += getSnappyOutputSize(rleBuffer, bwtRleSize);
        info.bwtRleHuffmanSize += getHuffmanOutputSize(rleBuffer, bwtRleSize);
        delete[] rleBuffer;
    }
    //Write the timing information
    info.snappyTime = snappyStopwatch.getMicroseconds() / 1000.0;
    info.bwtMtfHuffmanTime = bwtMtfHuffStopwatch.getMicroseconds() / 1000.0;
    info.deflateTime = deflateStopwatch.getMicroseconds() / 1000.0;
    delete[] buf;
    fclose(inFD);
}

/*
 * 
 */
int main(int argc, char** argv) {
    string infile;
    string outdir;
    uint32_t factor;
    uint32_t offset;
    string envvarName;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("infile", po::value<string > (&infile), "Input file")
            ("outdir", po::value<string > (&outdir), "Output directory")
            ("envvar", po::value<string > (&envvarName)->default_value("SGE_TASK_ID"), "Set this if you want to use an environment variable other than SGE_TASK_ID to determine the blocksize")
            ("factor", po::value<uint32_t > (&factor)->default_value(2500), "The factor to multiply the task ID env variable by to get the current blocksize - default 2500")
            ("offset", po::value<uint32_t > (&offset)->default_value(0), "The blocksize offset")
            ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    //Check for illegal option combinations
    if (vm.count("help")) {
        cerr << desc << "\n";
        return 1;
    }
    if (!vm.count("infile") || !(vm.count("outdir"))) {
        cout << "You need to specify both --input and --outdir!" << endl;
        cerr << desc << "\n";
        return 1;
    }
    if (!boost::ends_with(outdir, "/")) {
        outdir += "/";
    }
    //Calculate the blocksize for the current execution
    unsigned int blocksize = offset + atoi(getenv(envvarName.c_str())) * factor;
    cout << "Calculating eBWT for blocksize: " << blocksize << endl;
    //Create the statistics output file - headers: "Blocksize,Group,Algorithm,Size,Time"
    //   Column description:
    //   Blocksize: The current blocksize
    //   Group: The group the algorithm belongs to
    //   Algorithm: The algorithm
    //   Size: The size (of all blocks) the current blocksize-algorithm combination yields
    //   Time: How long the operation took (cumulatively), in ms, or 0.0 if no value is available
    //Initialize the statistics object
    EBWTStatisticsDataset stats;
    stats.origFilesize = getFilesizeInBytes(infile.c_str());
    bwtOnFile(infile.c_str(), blocksize, stats);
    //Write the statistics to the corresponding file
    ofstream statsOut((boost::format("%3%ebwt.stats-%1%.%2%.txt") % getFilenameFromPath(infile) % blocksize % outdir).str().c_str());
    stats.write(statsOut, blocksize);
    //Close the statistics FD
    statsOut.close();
    return 0;
}
