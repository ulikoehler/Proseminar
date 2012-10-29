/* 
 * File:   main.cpp
 * Author: uli
 *
 * Created on 29. Oktober 2012, 20:45
 */

#include <cstdlib>
#include <string>
#include <iostream>
using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    string s = "abraca";
    unsigned int slen = s.length();
    //Double the string to allow for p
    string mString = s + s;
    const char* m = mString.c_str();
    //Build the permutations
    const char** permutations = new const char*[slen];
    for (int i = 0; i < slen; i++) {
        permutations[i] = m + i;
    }
    //Print
    for (int i = 0; i < slen; i++) {
        string s(permutations[i], slen);
        cout << s;
    }

    return 0;
}

