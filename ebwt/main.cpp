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
#include <boost/thread/thread.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <functional>
#include <boost/program_options.hpp>
#include "mtf.hpp"
#include "util.hpp"
#include "compression.hpp"

using namespace std;
using boost::thread;
namespace po = boost::program_options;
//Heapsort implementation from:
//http://www.algorithmist.com/index.php/Heap_sort.c

//Configs
const string huffcodePath = "/home/k/koehleru/bin/huffcode";
const string lzopPath = "/home/k/koehleru/bin/lzop";
bool deleteRawFiles = true;

typedef boost::thread* ThreadPtr;

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

struct SizeDataResult {

    SizeDataResult() {
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
        //Misc
        origFilesize = 0;
        rleSize = 0;
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
    //Misc
    size_t origFilesize;
    size_t rleSize;

    void write(std::ostream & out) {
        out
                //Only compression
                << blocksize << ',' << "CompressionOnly,Huffman" << ',' << huffSize / (double) origFilesize << '\n'
                << blocksize << ',' << "CompressionOnly,LZO" << ',' << lzoSize / (double) origFilesize << '\n'
                << blocksize << ',' << "CompressionOnly,Snappy" << ',' << snappySize / (double) origFilesize << '\n'
                << blocksize << ',' << "CompressionOnly,LZ4" << ',' << lz4Size / (double) origFilesize << '\n'
                //No compression
                << blocksize << ',' << "EncodingOnly,RLE" << ',' << rleOnlySize / (double) origFilesize << '\n'
                << blocksize << ',' << "EncodingOnly,BWT+RLE" << ',' << bwtRleSize / (double) origFilesize << '\n'
                //Huffman
                << blocksize << ',' << "Huffman,BWT+Huffman" << ',' << bwtHuffmanSize / (double) origFilesize << '\n'
                << blocksize << ',' << "Huffman,MTF+Huffman" << ',' << mtfHuffmanSize / (double) origFilesize << '\n'
                << blocksize << ',' << "Huffman,BWT+MTF+Huffman" << ',' << bwtMtfHuffmanSize / (double) origFilesize << '\n'
                << blocksize << ',' << "Huffman,RLE+Huffman" << ',' << rleHuffmanSize / (double) origFilesize << '\n'
                << blocksize << ',' << "Huffman,BWT+RLE+Huffman" << ',' << bwtRleHuffmanSize / (double) origFilesize << '\n'
                //LZO
                << blocksize << ',' << "LZO,BWT+LZO" << ',' << bwtLZOSize / (double) origFilesize << '\n'
                << blocksize << ',' << "LZO,BWT+MTF+LZO" << ',' << bwtMtfLZOSize / (double) origFilesize << '\n'
                << blocksize << ',' << "LZO,MTF+LZO" << ',' << mtfLZOSize / (double) origFilesize << '\n'
                << blocksize << ',' << "LZO,RLE+LZO" << ',' << rleLZOSize / (double) origFilesize << '\n'
                << blocksize << ',' << "LZO,BWT+RLE+LZO" << ',' << bwtRleLZOSize / (double) origFilesize << '\n'
                //Snappy
                << blocksize << ',' << "Snappy,BWT+Snappy" << ',' << bwtSnappySize / (double) origFilesize << '\n'
                << blocksize << ',' << "Snappy,BWT+MTF+Snappy" << ',' << bwtMtfSnappySize / (double) origFilesize << '\n'
                << blocksize << ',' << "Snappy,MTF+Snappy" << ',' << mtfSnappySize / (double) origFilesize << '\n'
                << blocksize << ',' << "Snappy,RLE+Snappy" << ',' << rleSnappySize / (double) origFilesize << '\n'
                << blocksize << ',' << "Snappy,BWT+RLE+Snappy" << ',' << bwtRleSnappySize / (double) origFilesize << '\n'
                //LZ4
                << blocksize << ',' << "LZ4,BWT+LZ4" << ',' << bwtSnappySize / (double) origFilesize << '\n'
                << blocksize << ',' << "LZ4,BWT+MTF+LZ4" << ',' << bwtMtfSnappySize / (double) origFilesize << '\n'
                << blocksize << ',' << "LZ4,MTF+LZ4" << ',' << mtfSnappySize / (double) origFilesize << '\n'
                << blocksize << ',' << "LZ4,RLE+LZ4" << ',' << rleSnappySize / (double) origFilesize << '\n'
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
        SizeDataResult& info) {
    FILE* inFD = fopen(infile, "r");
    //Initialize the transformer
    BWTTransformer transformer(blocksize);
    char* buf = new char[blocksize];
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
        info->lzoSize += getLZO1X11OutputSize(buf, read);
        info->lz4Size += getLZ4OutputSize(buf, read);
        info->snappySize += getSnappyOutputSize(buf, read);
        info->huffSize += getHuffmanOutputSize(buf, read);
        //
        // Calculate MTF on the raw input
        //
        char* mtfBuffer = new char[read];
        moveToFrontEncodeAutoAlphabetCopy(buf, read, mtfBuffer);
        //Compress and calculate the sizes
        info->mtfLZOSize += getLZO1X11OutputSize(mtfBuffer, read);
        info->mtfLZ4Size += getLZ4OutputSize(mtfBuffer, read);
        info->mtfSnappySize += getSnappyOutputSize(mtfBuffer, read);
        info->mtfHuffmanSize += getHuffmanOutputSize(mtfBuffer, read);
        delete[] mtfBuffer;
        //
        // Calculate RLE on the raw input
        //
        char* rleBuffer = new char[read * 2]; //Maximum RLE size if no runs are found
        size_t bwtRleSize = doRLE(buf, read, rleBuffer);
        //Compress and calculate the sizes
        info->rleSize += bwtRleSize;
        info->rleLZOSize += getLZO1X11OutputSize(rleBuffer, bwtRleSize);
        info->rleLZ4Size += getLZ4OutputSize(rleBuffer, bwtRleSize);
        info->rleSnappySize += getSnappyOutputSize(rleBuffer, bwtRleSize);
        info->rleHuffmanSize += getHuffmanOutputSize(rleBuffer, bwtRleSize);
        delete[] rleBuffer;
        //
        // Calculate BWT --> transformer.L now contains BWT result (except of index)
        //
        transformer.bwt(buf);
        //Compress and calculate the sizes
        info->bwtLZOSize += getLZO1X11OutputSize(transformer.L, transformer.datasize);
        info->bwtLZ4Size += getLZ4OutputSize(transformer.L, transformer.datasize);
        info->bwtSnappySize += getSnappyOutputSize(transformer.L, transformer.datasize);
        info->bwtHuffmanSize += getHuffmanOutputSize(transformer.L, transformer.datasize);
        //
        // Calculate BWT + MTF
        //
        moveToFrontEncodeAutoAlphabetInPlace(transformer.L, read); //Auto-create the alphabet and MTF encode the BWT stuff
        //Compress and calculate the sizes
        info->bwtMtfLZOSize += getLZO1X11OutputSize(transformer.L, read);
        info->bwtMtfLZ4Size += getLZ4OutputSize(transformer.L, read);
        info->bwtMtfSnappySize += getSnappyOutputSize(transformer.L, read);
        info->bwtHuffmanSize += getHuffmanOutputSize(transformer.L, read);
        //
        // Calculate BWT + RLE
        //
        //Reuse the RLE-buffer from the RLE-only encoding
        size_t bwtRleSize = doRLE(buf, read, rleBuffer);
        //Compress and calculate the sizes
        info->bwtRleSize += bwtRleSize;
        info->bwtRleLZOSize += getLZO1X11OutputSize(rleBuffer, bwtRleSize);
        info->bwtRleLZ4Size += getLZ4OutputSize(rleBuffer, bwtRleSize);
        info->bwtRleSnappySize += getSnappyOutputSize(rleBuffer, bwtRleSize);
        info->bwtRleHuffmanSize += getHuffmanOutputSize(rleBuffer, bwtRleSize);
        delete[] rleBuffer;

    }
    delete[] buf;
    fclose(inFD);
}

enum CompressMode {
#ifndef NO_SNAPPY
    SNAPPY,
#endif
    HUFFCODE, LZOP
};

/**
 * Thread ftor to compress a single file using a native binary
 */
void compressFile(CompressMode mode,
        size_t& filesize,
        const string& inputFilename,
        const string& outputFilename) {
    if (mode == HUFFCODE) {
        string cmd = (boost::format("%3% -i %1% -o %2%") % inputFilename % outputFilename % huffcodePath).str();
        filesize = getFilesizeInBytes(outputFilename.c_str());
        int ret = system(cmd.c_str());
    } else if (mode == LZOP) {
        string cmd = (boost::format("%2% %1%") % inputFilename % lzopPath).str();
        filesize = getFilesizeInBytes(outputFilename.c_str());
        int ret = system(cmd.c_str());
    } else if (mode == SNAPPY) {
        //Snappy algorithm calculates the filesize resulting from compression directly
        //100k has been determined as good avg blksize experimentally
        filesize = getSnappyCompressedSize(100000, inputFilename.c_str());
    }
    //Delete the output file if enabled
    if (deleteRawFiles && (mode != SNAPPY)) { //snappy produces no output file
        remove(outputFilename.c_str());
    }
}

/**
 * Execute the BWT and choose the output filename automatically
 * @param outdir A path to write temporary files to - must end with '/'
 * @param huffSize The size of the original file, huffman-encoded
 */
void autoBWT(string& infile, string& outdir, int blocksize, ofstream& statsOut, size_t huffSize) {
    //IGNORE blocksize == 0
    if (blocksize == 0) {
        return;
    }
    string infilename = infile.erase(0, infile.find_last_of('/') + 1);
    //Calculate the filenames to write the data to
    string bwtFilename = (boost::format("%3%%1%.%2%.bwt") % infilename % blocksize % outdir).str();
    string bwtMTFFilename = (boost::format("%3%%1%.%2%.bwt.mtf") % infilename % blocksize % outdir).str();
    string mtfFilename = (boost::format("%3%%1%.%2%.mtf") % infilename % blocksize % outdir).str();
    string rleFilename = (boost::format("%3%%1%.%2%.rle") % infilename % blocksize % outdir).str();
    string bwtRLEFilename = (boost::format("%3%%1%.%2%.bwt.rle") % infilename % blocksize % outdir).str();
    //Execute the BWT and MTF algorithms on the input file to produce several output files
    bwtOnFile(infile.c_str(), bwtFilename.c_str(), bwtMTFFilename.c_str(), mtfFilename.c_str(),
            bwtRLEFilename.c_str(), rleFilename.c_str(), blocksize);
    //Calculate the output filenames for huffcode
    string btwHuffmanFilename = (boost::format("%3%%1%.%2%.bwt.huff") % infilename % blocksize % outdir).str();
    string btwMtfHuffmanFilename = (boost::format("%3%%1%.%2%.bwt.mtf.huff") % infilename % blocksize % outdir).str();
    string mtfHuffmanFilename = (boost::format("%3%%1%.%2%.mtf.huff") % infilename % blocksize % outdir).str();
    string rleHuffmanFilename = (boost::format("%3%%1%.%2%.rle.huff") % infilename % blocksize % outdir).str();
    string bwtRLEHuffmanFilename = (boost::format("%3%%1%.%2%.bwt.rle.huff") % infilename % blocksize % outdir).str();
    //Calculate the output filenames for LZO
    string bwtLZOFilename = (boost::format("%3%%1%.%2%.bwt.lzo") % infilename % blocksize % outdir).str();
    string bwtMtfLZOFilename = (boost::format("%3%%1%.%2%.bwt.mtf.lzo") % infilename % blocksize % outdir).str();
    string mtfLZOFilename = (boost::format("%3%%1%.%2%.mtf.lzo") % infilename % blocksize % outdir).str();
    string rleLZOFilename = (boost::format("%3%%1%.%2%.rle.lzo") % infilename % blocksize % outdir).str();
    string bwtRLELZOFilename = (boost::format("%3%%1%.%2%.bwt.rle.lzo") % infilename % blocksize % outdir).str();
    //Start each compressor in a separate thread
    //overhead should be acceptable when compared to performance gain
    const int numThreads = 15;
    ThreadPtr compressThreads[numThreads];
    //Initialize variables for the resulting filesizes

    //While the other threads are running, get the remaining filesizes 
    //Uncompressed files
    //Join the threads
    for (int i = 0; i < numThreads; i++) {
        compressThreads[i]->join();
        delete compressThreads[i];
    }
    //
    // Evaluate the resulting filesizes
    //
    //R-compatible output

    //Remove the raw files if option is enabled
    if (deleteRawFiles) {

        remove(bwtFilename.c_str());
        remove(bwtMTFFilename.c_str());
        remove(mtfFilename.c_str());
    }
}

/*
 * 
 */
int main(int argc, char** argv) {
    string infile;
    string outdir;
    uint32_t minBlocksize;
    uint32_t maxBlocksize;
    uint32_t blocksizeStep;
    uint32_t blocksize;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("infile", po::value<string > (&infile), "Input file")
            ("outdir", po::value<string > (&outdir), "Output directory")
            ("blocksize", po::value<uint32_t > (&blocksize), "Calculate only a single blocksize")
            ("min-blocksize", po::value<uint32_t > (&minBlocksize), "The minimum blocksize to calculate")
            ("max-blocksize", po::value<uint32_t > (&maxBlocksize), "The maximum blocksize to calculate")
            ("blocksize-step", po::value<uint32_t > (&blocksizeStep), "The difference between two calculated blocksizes")
            ("keep-files", "Keep all raw files")
            ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    //Check for illegal option combinations
    if (vm.count("help")) {
        cerr << desc << "\n";
        return 1;
    }
    if (vm.count("blocksize") && (vm.count("min-blocksize") || vm.count("max-blocksize") || vm.count("blocksize-step"))) {
        cerr << "--blocksize must not be specified together with one of --min-blocksize, --max-blocksize or --blocksize-step" << endl;
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
    //Process some optional arguments
    if (vm.count("keep-files")) {
        deleteRawFiles = false;
    }
    //Create the statistics output file
    ofstream statsOut((boost::format("ebwt.statistics.%1%.txt") % getFilenameFromPath(infile)).str().c_str());
    statsOut << "Blocksize,Algorithm,Size" << '\n';
    //Huffman-compress the original file and get its size
    string compressedOriginalFilename = (boost::format("%2%%1%.huff") % getFilenameFromPath(infile) % outdir).str();
    string compressOrigCommand = (boost::format("%3% -i %1% -o %2%") % infile % compressedOriginalFilename % huffcodePath).str();
    cout << "Compressing original file: " << compressOrigCommand << '\n';
    int ret = system(compressOrigCommand.c_str());
    size_t compressedOriginalSize = getFilesizeInBytes(compressedOriginalFilename.c_str());
    if (deleteRawFiles) { //Remove it if keep files isn't set
        remove(compressedOriginalFilename.c_str());
    }
    cout << "Compressed original file has size " << compressedOriginalSize << endl;
    //Only single or multiple blocksizes
    if (vm.count("blocksize")) { //Only single blocksize
        cout << "Calculating BWT with blocksize " << minBlocksize << endl;
        autoBWT(infile, outdir, minBlocksize, statsOut, compressedOriginalSize);
        return 0;
    }
    //Do some postprocessing to make the filenames valid
    if (!boost::ends_with(outdir, "/")) {
        outdir += "/";
    }
    //Execute the BWT
    cout << boost::format("Enabled multi-BWT with min/max %1%/%2%, step %3%") % minBlocksize % maxBlocksize % blocksizeStep << endl;
    for (uint32_t i = minBlocksize; i < maxBlocksize; i += blocksizeStep) {
        cout << "Calculating BWT with blocksize " << i << '\n';
        autoBWT(infile, outdir, i, statsOut, compressedOriginalSize);
    }
    //Close the statistics FD
    statsOut.close();
    return 0;
}

