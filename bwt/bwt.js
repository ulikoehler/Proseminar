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
	return s.allButFirstChar(s) + firstChar(s);
}

module.exports = {
	lastChar: lastChar,
	firstChar: firstChar,
	allButLastChar : allButLastChar,
	allButFirstChar: allButFirstChar
}