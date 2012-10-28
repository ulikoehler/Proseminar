var bwt = require("./bwt.js")

exports.testFirstChar = function(test){
    test.equals('a', bwt.firstChar("abc"));
    test.done();
};

exports.testLastChar = function(test){
    test.equals('c', bwt.lastChar("abc"));
    test.done();
};

exports.testAllButLastChar = function(test){
    test.equals("ab", bwt.allButLastChar("abc"));
    test.done();
};

exports.testAllButFirstChar = function(test){
    test.equals("bc", bwt.allButFirstChar("abc"));
    test.done();
};