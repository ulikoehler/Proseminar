function firstChar(s) {
	return s.charAt(0)
}

function lastChar(s) {
	return s.charAt(s.length-1)
}

function allButLastChar(s) {
	return s.substr(0,s.length-1);
}

function allButFirstChar(s) {
	return s.substr(1);
}

function rotateLeft(s) {
	return allButFirstChar(s) + firstChar(s);
}

function rotateRight(s) {
	return lastChar(s) + allButLastChar(s);
}

//Builds all left rotations of a given string and returns an array of them
function allLeftRotations(s) {
	var ret = [];
	var last = s;
	for(var i = 0 ; i < s.length; i++) {
		ret.push(last);
		last = rotateLeft(last);
	}
	return ret;
}

//For an array of equally sized strings, returns the concatenation of these strings
function getLastColumn(array) {
	var ret = "";
	for(var i = 0 ; i < array.length; i++) {
		ret += lastChar(array[i]);
	}
	return ret;
}

//Find the index (0-based) of a given string in a given array
function findStringInArray(s, array){
	for(var i = 0; i < array.length;  i++){
		if(array[i] == s) {
			return i;
		}
	}
	return -1;
}

//Returns a array (L,I) of a given string representing the BWT of the string
//Returns -1 if not found
function bwt(s) {
	var sortedRotations = allLeftRotations(s).sort();
	var L = getLastColumn(sortedRotations);
	var i = findStringInArray(s, sortedRotations);
	return [L,i];
}

function bufferFromBWT(bwtarray) {
	//Check how long a buffer containing L is
	var LBuf = new Buffer(bwtarray[0]); //utf8
	var LBufLen = LBuf.length;
	//Allocate a new buffer with L's binary length + 4 bytes for a 32 bit rep of i
	var buf = new Buffer(LBufLen + 4);
	LBuf.copy(buf);
	//Write the uint32 as network byte order (big endian)
	buf.writeUInt32BE(bwtarray[1],  LBufLen);
	return buf;
}

//Extracts the array [L,i] from the given bwt buffer
function extractBWTFromBuf(buf) {
	var i = buf.readUInt32BE(buf.length - 4);
	var L = buf.toString("utf8", 0, buf.length-4);
	return [L,i];
}

//Calculates the BWT from a given buffer and encodes it as buffer
function bwtFromBuf(buf) {
	return bufferFromBWT(bwt(buf.toString()));
}

module.exports = {
	lastChar: lastChar,
	firstChar: firstChar,
	allButLastChar : allButLastChar,
	allButFirstChar: allButFirstChar,
	rotateLeft:rotateLeft,
	rotateRight:rotateRight,
	allLeftRotations:allLeftRotations,
	findStringInArray: findStringInArray,
	getLastColumn: getLastColumn,
	bufferFromBWT: bufferFromBWT,
	extractBWTFromBuf:extractBWTFromBuf,
	bwt:bwt,
	bwtFromBuf:bwtFromBuf
}