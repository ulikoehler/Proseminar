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
#include "mtf.hpp"
using namespace std;
//Heapsort implementation from:
//http://www.algorithmist.com/index.php/Heap_sort.c

//Configs
const string huffcodePath = "/home/k/koehleru/bin/huffcode";

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

void bwtOnFile(const char* infile, const char* outfileBWTOnly, const char* outfileMTF,  const char* outfileMTFOnly, unsigned int blocksize) {
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

/**
 * Execute the BWT and choose the output filename automatically
 * @param outdir A path to write temporary files to - must end with '/'
 * @param huffSize The size of the original file, huffman-encoded
 */
void autoBWT(string& infile, string& outdir, int blocksize, ofstream& statsOut, size_t huffSize) {
    //IGNORE blocksize == 0
    if(blocksize == 0) {
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
    string btwOnlyCompressedFile = (boost::format("%3%%1%.%2%.bwt.huff") % infilename % blocksize % outdir).str();
    string btwMtfCompressedFile = (boost::format("%3%%1%.%2%.bwt.mtf.huff") % infilename % blocksize % outdir).str();
    string mtfOnlyCompressedFile = (boost::format("%3%%1%.%2%.mtf.huff") % infilename % blocksize % outdir).str();
    //Build the compress system calls
    //string huffcodePath = "huffcode";
    string compressBWTCmd = (boost::format("%3% -i %1% -o %2%") % outfileBWTOnly % btwOnlyCompressedFile % huffcodePath).str();
    string compressBWTMTFCmd = (boost::format("%3% -i %1% -o %2%") % outfileBWTMTF % btwMtfCompressedFile % huffcodePath).str();
    string compressMTFCmd = (boost::format("%3% -i %1% -o %2%") % outfileMTFOnly % mtfOnlyCompressedFile % huffcodePath).str();
    cout << "   Compressing BTW: " << compressBWTCmd
	   << "\n   Compressing BTW+MTF: " << compressBWTMTFCmd
	   << "\n   Compressing MTF: " << compressMTFCmd << '\n';
    
    int ret = system(compressBWTCmd.c_str());
    ret = system(compressBWTMTFCmd.c_str());
    ret = system(compressMTFCmd.c_str());
    //
    // Evaluate the resulting filesizes
    //
    
    //Uncompressed files
    size_t bwtMtfSize = getFilesizeInBytes(outfileBWTMTF.c_str());
    size_t mtfOnlySize = getFilesizeInBytes(outfileMTFOnly.c_str());
    //Compressed files
    size_t bwtCompressedSize = getFilesizeInBytes(btwOnlyCompressedFile.c_str()); //BWT+Huff
    size_t bwtMtfCompressedSize = getFilesizeInBytes(btwMtfCompressedFile.c_str()); //BWT+MTF+Huff
    size_t mtfOnlyCompressedSize = getFilesizeInBytes(mtfOnlyCompressedFile.c_str()); //MTF+Huff
    //R
    statsOut << blocksize <<  "BWTMTF" << bwtMtfSize << '\n'
	<< blocksize << "Huffman" << mtfOnlyCompressedSize << '\n'
	<< blocksize << "MTF" << mtfOnlySize << '\n'
	<< blocksize << "BWT+Huffman" << bwtCompressedSize << '\n'
	<< blocksize << "MTF+Huffman" << mtfOnlyCompressedSize << '\n'
	<< blocksize << "BWT+MTF+Huffman" << bwtMtfCompressedSize << '\n';
    //Remove the files
    remove(outfileBWTOnly.c_str());
    remove(outfileBWTMTF.c_str());
    remove(outfileMTFOnly.c_str());
    
    remove(btwOnlyCompressedFile.c_str());
    remove(btwMtfCompressedFile.c_str());
    remove(mtfOnlyCompressedFile.c_str());
}

/*
 * 
 */
int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: bwt <infile> <outdir> <min blocksize | blocksize> [<max blocksize> <blocksize step>]" << endl;
        return 1;
    }
    //Create the statistics output file
    ofstream statsOut("ebwt.statistics.txt");
    statsOut << "Blocksize,Algorithm,Size" << '\n';
    //Parse the args
    string infile(argv[1]);
    //Remove path
    string outdir(argv[2]);
    int minBlocksize = atoi(argv[3]);
    //Huffman-compress the original file and get its size
    string compressedOriginalFilename = (boost::format("%2%%1%.huff") % infile % outdir).str();
    cout << "Compressing original file to " << compressedOriginalFilename << '\n';
    string compressOrigCommand = (boost::format("%3% -i %1% -o %2%") % infile % compressedOriginalFilename % huffcodePath).str();
    int ret = system(compressOrigCommand.c_str());
    size_t compressedOriginalSize = getFilesizeInBytes(compressedOriginalFilename.c_str());
    remove(compressedOriginalFilename.c_str());
    cout << "Compressed original file has size " << compressedOriginalSize << '\n';
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

