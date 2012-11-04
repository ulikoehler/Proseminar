/* 
 * File:   mtf.hpp
 * Author: uli
 *
 * Created on 30. Oktober 2012, 02:59
 */

#ifndef MTF_HPP
#define	MTF_HPP
#include <deque>
using namespace std;

deque<char> getAlphabet(const char* str, size_t len) {
    //Set all elements to true that occur in str
    bool occurrence[256];
    for (int i = 0; i < 256; i++) {
        occurrence[i] = false;
    }
    //Iterate over str
    for (int i = 0; i < len; i++) {
        occurrence[(uint8_t) str[i]] = true;
    }
    //Build the alphabet deque
    deque<char> ret;
    for (int i = 0; i < 256; i++) {
        if (occurrence[i]) {
            char val = i;
            ret.push_back(val);
        }
    }
    return ret;
}

string alphabetToString(const deque<char>& alphabet) {
    string ret;
    for (char c : alphabet) {
        ret += c;
    }
    return ret;
}

deque<char> alphabetFromString(const string& s) {
    deque<char> ret;
    for (char c : s) {
        ret.push_back(c);
    }
    return ret;
}

/**
 * In-Place move to front encoder
 * @param str
 * @param len
 */
void moveToFrontEncode(char* str, size_t len, deque<char>& alphabet) {
    for (int i = 0; i < len; i++) {
        const char val = str[i];
        auto it = find(alphabet.begin(), alphabet.end(), val);
        uint8_t index = it - alphabet.begin();
        //Set the index
        str[i] = index; //Alphabet can be at most 256 characters --> no need for multi-byte indices
        //Move the index to the front
        alphabet.erase(it);
        alphabet.push_front(val);
    }
}


/**
 * In-Place move to front encoder
 * @param str The string to read from
 * @param len The number of bytes to read
 */
void moveToFrontEncodeCopy(const char* str, size_t len, char* dest, deque<char>& alphabet) {
    for (int i = 0; i < len; i++) {
        const char val = str[i];
        auto it = find(alphabet.begin(), alphabet.end(), val);
        uint8_t index = it - alphabet.begin();
        //Set the index
        dest[i] = index; //Alphabet can be at most 256 characters --> no need for multi-byte indices
        //Move the index to the front
        alphabet.erase(it);
        alphabet.push_front(val);
    }
}


void moveToFrontEncodeWithAlphabet(char* str, size_t len) {
    deque<char> alphabet = getAlphabet(str, len);
    moveToFrontEncode(str, len, alphabet);
}

void moveToFrontEncodeWithAlphabetCopy(const char* str, size_t len, char* dest) {
    deque<char> alphabet = getAlphabet(str, len);
    moveToFrontEncodeCopy(str, len, dest, alphabet);
}


#endif	/* MTF_HPP */

