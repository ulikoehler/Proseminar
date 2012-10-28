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

//Returns a array (L,I) of a given string representing the BWT of the string
function bwt(s) {
	//var sortedRotations = 
}

module.exports = {
	lastChar: lastChar,
	firstChar: firstChar,
	allButLastChar : allButLastChar,
	allButFirstChar: allButFirstChar,
	rotateLeft:rotateLeft,
	rotateRight:rotateRight,
	allLeftRotations:allLeftRotations,
	getLastColumn: getLastColumn
}