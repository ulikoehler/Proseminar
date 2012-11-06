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
#include "mtf.hpp"
#include "util.hpp"
using namespace std;
using boost::thread;
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

size_t getFilesizeInBytes(const char* filename) {
    struct stat filestatus;
    stat(filename, &filestatus);
    return filestatus.st_size;
}

void bwtOnFile(const char* infile, const char* outfileBWTOnly, const char* outfileMTF, const char* outfileMTFOnly, unsigned int blocksize) {
    FILE* inFD = fopen(infile, "r");
    FILE* bwtOnlyFD = fopen(outfileBWTOnly, "w");
    FILE* mtfFD = fopen(outfileMTF, "w");
    FILE* mtfOnlyFD = fopen(outfileMTFOnly, "w");
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
        // Calculate MTF only and write to corresponding file (compressed later)
        //
        char* mtfOnlyDst = new char[read];
        moveToFrontEncodeWithAlphabetCopy(buf, read, mtfOnlyDst);
        fwrite(mtfOnlyDst, 1, read, mtfOnlyFD);
        delete[] mtfOnlyDst;
        //
        // Calculate BWT + MTF
        //
        //Do the BWT
        transformer.bwt(buf);
        //Write BWT-only
        fwrite(transformer.L, 1, read, bwtOnlyFD);
        //MTF-Encode 
        moveToFrontEncodeWithAlphabet(transformer.L, read); //Auto-create the alphabet and MTF encode the BWT stuff
        fwrite(transformer.L, 1, read, mtfFD);
    }
    delete[] buf;
    fclose(inFD);
    fclose(bwtOnlyFD);
    fclose(mtfFD);
    fclose(mtfOnlyFD);
}

enum CompressMode {
    HUFFCODE, LZOP
};

/**
 * Thread ftor to compress a single file using a native binary
 */
void compressFile(CompressMode mode, size_t& filesize, const string& input, const string& output) {
    if (mode == HUFFCODE) {
        string cmd = (boost::format("%3% -i %1% -o %2%") % input % output % huffcodePath).str();
        int ret = system(cmd.c_str());
    } else if (mode == LZOP) {
        string cmd = (boost::format("%2% %1%") % input % lzopPath).str();
        int ret = system(cmd.c_str());
    }
    filesize = getFilesizeInBytes(output.c_str());
    if(deleteRawFiles) {
        remove(output.c_str());
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
    string outfileBWTOnly = (boost::format("%3%%1%.%2%.bwt") % infilename % blocksize % outdir).str();
    string outfileBWTMTF = (boost::format("%3%%1%.%2%.bwt.mtf") % infilename % blocksize % outdir).str();
    string outfileMTFOnly = (boost::format("%3%%1%.%2%.mtf") % infilename % blocksize % outdir).str();
    //Execute the BWT and MTF algorithms on the input file to produce several output files
    bwtOnFile(infile.c_str(), outfileBWTOnly.c_str(), outfileBWTMTF.c_str(), outfileMTFOnly.c_str(), blocksize);
    //Execute huffcode on the files
    string btwHuffmanFilename = (boost::format("%3%%1%.%2%.bwt.huff") % infilename % blocksize % outdir).str();
    string btwMtfHuffmanFilename = (boost::format("%3%%1%.%2%.bwt.mtf.huff") % infilename % blocksize % outdir).str();
    string mtfHuffmanFilename = (boost::format("%3%%1%.%2%.mtf.huff") % infilename % blocksize % outdir).str();
    //Start each compressor in a separate thread
    //overhead should be acceptable when compared to performance gain
    const int numThreads = 6;
    ThreadPtr compressThreads[numThreads];
    size_t bwtHuffmanSize = 0;
    size_t bwtMtfHuffmanSize = 0;
    size_t mtfHuffmanSize = 0;
    size_t bwtLZOSize = 0;
    size_t bwtMtfLZOSize = 0;
    size_t mtfLZOSize = 0;
    //Start the huffcode threads
    compressThreads[0] = new thread([&](){compressFile(HUFFCODE, bwtHuffmanSize, outfileBWTOnly, btwHuffmanFilename);});
    compressThreads[1] = new thread([&](){compressFile(HUFFCODE, bwtMtfHuffmanSize, outfileBWTMTF, btwMtfHuffmanFilename);});
    compressThreads[2] = new thread([&](){compressFile(HUFFCODE, mtfHuffmanSize, outfileMTFOnly, mtfHuffmanFilename);});
    //Start the LZO threads
    compressThreads[3] = new thread([&](){compressFile(LZOP, bwtLZOSize, outfileBWTOnly, btwHuffmanFilename);});
    compressThreads[4] = new thread([&](){compressFile(LZOP, bwtMtfLZOSize, outfileBWTMTF, btwMtfHuffmanFilename);});
    compressThreads[5] = new thread([&](){compressFile(LZOP, mtfLZOSize, outfileMTFOnly, mtfHuffmanFilename);});
    //While the other threads are running, get the remaining filesizes 
    //Uncompressed files
    size_t bwtMtfSize = getFilesizeInBytes(outfileBWTMTF.c_str());
    size_t mtfOnlySize = getFilesizeInBytes(outfileMTFOnly.c_str());
     //Join the threads
    for (int i = 0; i < numThreads; i++) {
        compressThreads[i]->join();
        delete compressThreads[i];
    }
    //
    // Evaluate the resulting filesizes
    //
    //R-compatible output
    statsOut
            << blocksize << ',' << "Huffman" << ',' << huffSize << '\n'
            << blocksize << ',' << "MTF" << ',' << mtfOnlySize << '\n'
            << blocksize << ',' << "BWT+MTF" << ',' << bwtMtfSize << '\n'
            << blocksize << ',' << "BWT+Huffman" << ',' << bwtHuffmanSize << '\n'
            << blocksize << ',' << "MTF+Huffman" << ',' << mtfHuffmanSize << '\n'
            << blocksize << ',' << "BWT+MTF+Huffman" << ',' << bwtMtfHuffmanSize
            << blocksize << ',' << "BWT+LZO" << ',' << bwtLZOSize
            << blocksize << ',' << "BWT+MTF+LZO" << ',' << bwtMtfLZOSize
            << blocksize << ',' << "MTF+LZO" << ',' << mtfLZOSize << endl;
    //Remove the raw files if option is enabled
    if (deleteRawFiles) {
        remove(outfileBWTOnly.c_str());
        remove(outfileBWTMTF.c_str());
        remove(outfileMTFOnly.c_str());
    }
}

/*
 * 
 */
int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: bwt <infile> <outdir> <min blocksize | blocksize> [<max blocksize> <blocksize step>]" << endl;
	cout << "Set the KEEP_RAW_FILES environment variable to keep all generated files" << endl;
        return 1;
    }
    //Parse the args
    string infile(argv[1]);
    //Remove path
    string outdir(argv[2]);
    int minBlocksize = atoi(argv[3]);
    //Check if raw files shall be kept
    const char* keepRawFilesEnvValue = getenv("KEEP_RAW_FILES");
    string keepRawFilesValueString = (keepRawFilesEnvValue == NULL ? "" : keepRawFilesEnvValue);
    if (!keepRawFilesValueString.empty()) {
        cout << "Keeping raw output data";
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
    if (argc < 6) { //Only single blocksize
        cout << "Calculating BWT with blocksize " << minBlocksize << endl;
        autoBWT(infile, outdir, minBlocksize, statsOut, compressedOriginalSize);
        return 0;
    }
    //Multiple block sizes
    int maxBlocksize = atoi(argv[4]);
    int blocksizeStep = atoi(argv[5]);
    //Do some postprocessing to make the filenames valid
    if (!boost::ends_with(outdir, "/")) {
        outdir += "/";
    }
    //Execute the BWT
    cout << boost::format("Enabled multi-BWT with min/max %1%/%2%, step %3%") % minBlocksize % maxBlocksize % blocksizeStep << endl;
    for (int i = minBlocksize; i < maxBlocksize; i += blocksizeStep) {
        cout << "Calculating BWT with blocksize " << i << '\n';
        autoBWT(infile, outdir, i, statsOut, compressedOriginalSize);
    }
    //Close the statistics FD
    statsOut.close();
    return 0;
}

