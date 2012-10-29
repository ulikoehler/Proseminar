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
#include <limits>
#include <iostream>
#include <algorithm>
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

void bwt(const char* s, unsigned int datasize) {
    //Double the string to allow in-place rotation building
    // (don't double the NUL terminator)
    char* m = new char[2 * datasize];
    memcpy(m, s, datasize);
    memcpy(m + datasize, s, datasize);
    //Build the sortable permutation wrappers
    Sort* sortArray = new Sort[datasize];
    for (int i = 0; i < datasize; i++) {
        sortArray[i].index = i;
        sortArray[i].m = m;
        sortArray[i].slen = datasize;
    }
    //Sort the Array
    heapSort(sortArray, datasize);
    //Copy the last column to L and find I in the process
    char* L = new char[datasize];
    unsigned int I = 0;
    for (int i = 0; i < datasize; i++) {
        L[i] = (*sortArray[i])[datasize - 1];
        if (sortArray[i] == s) {
            I = i;
        }
    }
    //Find I
    cout << "(L,I) = " << L << "," << I << endl;
}

/*
 * 
 */
int main(int argc, char** argv) {
    string s = "abraca";
    bwt(s.c_str(), s.length());
    return 0;
}

