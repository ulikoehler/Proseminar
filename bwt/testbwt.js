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

exports.testRotateLeft = function(test){
    test.equals("bca", bwt.rotateLeft("abc"));
    test.done();
};

exports.testRotateRight = function(test){
    test.equals("cab", bwt.rotateRight("abc"));
    test.done();
};

exports.testAllLeftRotations = function(test){
    test.deepEqual(["abc","bca","cab"], bwt.allLeftRotations("abc"));
    test.done();
};

//Test if sort works on the array returned by testAllLeftRotations()
exports.testAllLeftRotationsSort = function(test){
    test.deepEqual(["acb","cba","bac"], bwt.allLeftRotations("acb"));
    test.deepEqual(["acb","bac","cba"], bwt.allLeftRotations("acb").sort());
    test.done();
};

exports.testGetLastColumn = function(test){
    test.deepEqual("bac", bwt.getLastColumn(["acb","cba","bac"]));
    test.done();
};