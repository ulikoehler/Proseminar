#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE
#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <map>
#include <boost/algorithm/string.hpp>
#include "../mtf.hpp"
#include "../util.hpp"
#include "../rle.hpp"

using namespace std;

BOOST_AUTO_TEST_CASE(TestInfilenameProcessing) {
    string infile = "/home/uli/test.txt";
    infile.erase(0, infile.find_last_of('/') + 1);
    BOOST_CHECK_EQUAL("test.txt", infile);
    //Test without slash
    infile = "test.txt";
    infile.erase(0, infile.find_last_of('/') + 1);
    BOOST_CHECK_EQUAL("test.txt", infile);
}

BOOST_AUTO_TEST_CASE(TestGetAlphabet) {
    string test = "ATATATATGCCGAG"; //ATGC
    deque<char> alphabet = getAlphabet(test.c_str(), test.length());
    BOOST_CHECK_EQUAL(4, alphabet.size());
    BOOST_CHECK(alphabet.end() != find(alphabet.begin(), alphabet.end(), 'A'));
    BOOST_CHECK(alphabet.end() != find(alphabet.begin(), alphabet.end(), 'G'));
    BOOST_CHECK(alphabet.end() != find(alphabet.begin(), alphabet.end(), 'C'));
    BOOST_CHECK(alphabet.end() != find(alphabet.begin(), alphabet.end(), 'T'));
    //Negative result expected
    BOOST_CHECK(alphabet.end() == find(alphabet.begin(), alphabet.end(), 'F'));
}

BOOST_AUTO_TEST_CASE(TestMTF) {
    const char* testConst = "bananaaa"; //ATGC
    char* test = new char[strlen(testConst)];
    memcpy(test, testConst, strlen(testConst) + 1);
    deque<char> alphabet = alphabetFromString("abcdefghijklmnopqrstuvwxyz");
    moveToFrontEncode(test, strlen(test), alphabet); //In-place
    //BAD!! std::string modification in-place
    BOOST_CHECK_EQUAL(1, test[0]);
    BOOST_CHECK_EQUAL(1, test[1]);
    BOOST_CHECK_EQUAL(13, test[2]);
    BOOST_CHECK_EQUAL(1, test[3]);
    BOOST_CHECK_EQUAL(1, test[4]);
    BOOST_CHECK_EQUAL(1, test[5]);
    BOOST_CHECK_EQUAL(0, test[6]);
    BOOST_CHECK_EQUAL(0, test[7]);
}

BOOST_AUTO_TEST_CASE(TestMTFWithCopy) {
    const char* testConst = "bananaaa"; //ATGC
    char* test = new char[strlen(testConst)];
    deque<char> alphabet = alphabetFromString("abcdefghijklmnopqrstuvwxyz");
    moveToFrontEncodeCopy(testConst, strlen(test), test, alphabet); //In-place
    BOOST_CHECK_EQUAL(1, test[0]);
    BOOST_CHECK_EQUAL(1, test[1]);
    BOOST_CHECK_EQUAL(13, test[2]);
    BOOST_CHECK_EQUAL(1, test[3]);
    BOOST_CHECK_EQUAL(1, test[4]);
    BOOST_CHECK_EQUAL(1, test[5]);
    BOOST_CHECK_EQUAL(0, test[6]);
    BOOST_CHECK_EQUAL(0, test[7]);
}


BOOST_AUTO_TEST_CASE(TestGetFilenameFromPath) {
    BOOST_CHECK_EQUAL("test.txt", getFilenameFromPath("test.txt"));
    BOOST_CHECK_EQUAL("test.txt", getFilenameFromPath("/home/test.txt"));
    BOOST_CHECK_EQUAL("test.txt", getFilenameFromPath("/home/user/test.txt"));
    BOOST_CHECK_EQUAL("test.txt", getFilenameFromPath("//test.txt"));
    BOOST_CHECK_EQUAL("test", getFilenameFromPath("tmp/test"));
    BOOST_CHECK_EQUAL("test.txt", getFilenameFromPath("./test.txt"));
}


BOOST_AUTO_TEST_CASE(TestRLE) {
	char* test1 = "AAAABBBC";
	char* test1Target = new char[strlen(test1)*2];
	char* test1Expected = new char[6];
	test1Expected[0] = 'A';
	test1Expected[1] = (char)4;
	test1Expected[2] = 'B';
	test1Expected[3] = (char)3;
	test1Expected[4] = 'C';
	test1Expected[5] = (char)1;
	//Execute RLE and check length
	BOOST_CHECK_EQUAL(6, doRLE(test1, strlen(test1), test1Target));
	BOOST_CHECK_EQUAL(test1Expected[0], test1Target[0]); 
	BOOST_CHECK_EQUAL(test1Expected[1], test1Target[1]); 
	BOOST_CHECK_EQUAL(test1Expected[2], test1Target[2]); 
	BOOST_CHECK_EQUAL(test1Expected[3], test1Target[3]); 
	BOOST_CHECK_EQUAL(test1Expected[4], test1Target[4]); 
	BOOST_CHECK_EQUAL(test1Expected[5], test1Target[5]); 
	delete[] test1Target;
	delete[] test1Expected;
}