#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE 
#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <map>
#include <boost/algorithm/string.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE(TestInfilenameProcessing) {
    string infile = "/home/uli/test.txt";
    infile.erase(0,infile.find_last_of('/')+1);
    BOOST_CHECK_EQUAL("test.txt", infile);
    //Test without slash
    infile = "test.txt";
    infile.erase(0,infile.find_last_of('/')+1);
    BOOST_CHECK_EQUAL("test.txt", infile);
}