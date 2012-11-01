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

void bwtOnFile(const char* infile, const char* outfile, const char* outfileMTF, unsigned int blocksize) {
    FILE* inFD = fopen(infile, "r");
    FILE* outFD = fopen(outfile, "w");
    FILE* mtfFD = fopen(outfileMTF, "w");
    //Initialize the transformer
    BWTTransformer transformer(blocksize);
    char* buf = new char[blocksize];
    while (true) {
        size_t read = fread(buf, 1, blocksize, inFD);
        if (read < blocksize) { //Last block
            if (read == 0) { //No data
                break; //Stop immediately
            }
            //Execute the BWT on the last block
            transformer.resize(read);
            transformer.bwt(buf);
            //Write it
            fwrite(transformer.L, 1, read, outFD);
            //MTF-Encode
            moveToFrontEncodeWithAlphabet(transformer.L, read);
            fwrite(transformer.L, 1, read, mtfFD);
            break;
        }
        //Do the BWT
        transformer.bwt(buf);
        //Write it
        fwrite(transformer.L, 1, blocksize, outFD);
        //MTF-Encode
        moveToFrontEncodeWithAlphabet(transformer.L, blocksize);
        fwrite(transformer.L, 1, blocksize, mtfFD);
    }
    delete buf;
    fclose(inFD);
    fclose(outFD);
    fclose(mtfFD);
}

/**
 * Execute the BWT and choose the output filename automatically
 */
void autoBWT(string& infile, string& outdir, int blocksize, ofstream& statsOut) {
    //IGNORE blocksize == 0
    if(blocksize == 0) {
	return;
    }
    if (!boost::ends_with(outdir, "/")) {
        outdir += "/";
    }
    string infilename = infile.erase(0, infile.find_last_of('/') + 1);
    string outfile = (boost::format("%3%%1%.%2%.bwt") % infilename % blocksize % outdir).str();
    string outfileMTF = (boost::format("%3%%1%.%2%.bwt.mtf") % infilename % blocksize % outdir).str();
    cout << "  Writing to " << outfile << " and " << outfileMTF << endl;
    bwtOnFile(infile.c_str(), outfile.c_str(), outfileMTF.c_str(), blocksize);
    //Execute gzip on the files
    cout << "Compressing " << outfile << " and " << outfileMTF << endl;
    string compressedFile = (boost::format("%3%%1%.%2%.bwt.huff") % infilename % blocksize % outdir).str();
    string mtfCompressedFile = (boost::format("%3%%1%.%2%.bwt.mtf.huff") % infilename % blocksize % outdir).str();
    //Build the compress system calls
    string huffcodePath = "/home/k/koehleru/bin/huffcode";
    //string huffcodePath = "huffcode";
    string compressCmd = (boost::format("%3% -i %1% -o %2%") % outfile % compressedFile % huffcodePath).str();
    cout << "   Compressing: " << compressCmd << endl;
    string compressMTFCmd = (boost::format("%3% -i %1% -o %2%") % outfileMTF % mtfCompressedFile % huffcodePath).str();
    cout << "   Compressing MTF: " << compressCmd << endl;
    system(compressCmd.c_str());
    system(compressMTFCmd.c_str());
    //
    // Evaluate the resulting filesizes
    //
    size_t btwOnlySize = getFilesizeInBytes(outfile.c_str());
    size_t btwMtfSize = getFilesizeInBytes(outfileMTF.c_str());
    size_t btwCompressedSize = getFilesizeInBytes(compressedFile.c_str());
    size_t btwMtfCompressedSize = getFilesizeInBytes(mtfCompressedFile.c_str());
    statsOut << blocksize << ',' << btwOnlySize << ',' << btwCompressedSize << ','
		<< btwMtfSize << ',' << btwMtfCompressedSize << endl;
    //Remove the files
    remove(outfile.c_str());
    remove(outfileMTF.c_str());
    remove(compressedFile.c_str());
    remove(mtfCompressedFile.c_str());
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
    statsOut << "Blocksize,Nur BWT,BTW+Huffman,BWT+MTF,BTW+MTF+Huffman" << endl;
    //Parse the args
    string infile(argv[1]);
    //Remove path
    string outdir(argv[2]);
    int minBlocksize = atoi(argv[3]);
    //Only single or multiple blocksizes
    if (argc < 6) { //Only single blocksize
        cout << "Calculating BWT with blocksize " << minBlocksize << endl;
        autoBWT(infile, outdir, minBlocksize, statsOut);
        return 0;
    }
    //Multiple block sizes
    int maxBlocksize = atoi(argv[4]);
    int blocksizeStep = atoi(argv[5]);
    //Execute the BWT
    cout << boost::format("Enabled multi-BWT with min/max %1%/%2%, step %3%") % minBlocksize % maxBlocksize % blocksizeStep << endl;
    for (int i = minBlocksize; i < maxBlocksize; i += blocksizeStep) {
        cout << "Calculating BWT with blocksize " << i << endl;
        autoBWT(infile, outdir, i, statsOut);
    }
    //Close the statistics FD
    statsOut.close();
    return 0;
}

