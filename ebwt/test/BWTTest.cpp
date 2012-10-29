#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE 
#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <map>

BOOST_AUTO_TEST_CASE(TestConvertStringToNanodegree) {
    //BOOST_CHECK_EQUAL(1000000000L, coordinateStringToNanodegrees("01.0000000")); //Simple test for starters
    //BOOST_CHECK_EQUAL(49727322000L, coordinateStringToNanodegrees("49.7273220")); //form (3)
    //BOOST_CHECK_EQUAL(-9727322000L, coordinateStringToNanodegrees("-9.7273220"));
}


BOOST_AUTO_TEST_CASE(TestNanodegreeStringToCoordinate) {
    //BOOST_CHECK_EQUAL(1000000000L, nanodegreeStringToCoordinate("1000000000")); //Simple test for starters
    //BOOST_CHECK_EQUAL(49727322000L, nanodegreeStringToCoordinate("49727322000")); //form (3)
    //BOOST_CHECK_EQUAL(-9727322000L, nanodegreeStringToCoordinate("-9727322000"));
}