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
    test.equals("bac", bwt.getLastColumn(["acb","cba","bac"]));
    test.done();
};

exports.testFindStringInArray = function(test) {
    test.equals(2, bwt.findStringInArray("bac",["acb","cba","bac"]));
    test.equals(-1, bwt.findStringInArray("cab",["acb","cba","bac"]));
    test.done();
};

exports.testBWT = function(test) {
    test.deepEqual(["bca",2], bwt.bwt("cba"));
    test.done();
};

//Test whether the BWT buffer codec has been implemented correctly
exports.testBWTBuffer = function(test) {
	var bwtres = bwt.bwt("cba");
	var buf = bwt.bufferFromBWT(bwtres);
	//Test if decoding the buf yields the original result
	test.deepEqual(bwtres, bwt.extractBWTFromBuf(buf));
	test.done();
};