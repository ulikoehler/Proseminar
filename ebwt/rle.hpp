#ifndef RLE_HPP
#define	RLE_HPP
#include <limits>
/**
 * Executes the run-length encoding on an input dataset
 * The output ptr must have the worst case length |input|*2
 * @return The length of the output being used
 */
size_t doRLE(char* input, size_t inputLength, char* output) {
	char* origOutput = output;
	//Initial run == first char
	char currentRunChar = input[0];
	uint8_t currentRunLength = 1; //Number of currentChar in current run
	for(size_t i = 1 ; i < inputLength; i++) {
		char currentChar = input[i];
		//Write the tuple
		if(currentChar != currentRunChar || currentRunLength == 255 || i == (inputLength-1)) {
			//Write the tupel <run character, run length>, each 8 bits, to the output
			output[0] = currentRunChar;
			output++;
			output[0] = currentRunLength;
			output++;
			//Update the state variables
			currentRunLength = 1;
		} else {
			currentRunLength++;
		}
		currentRunChar = currentChar;
	}
	output[0] = currentRunChar;
	output++;
	output[0] = currentRunLength;
	output++;
	return (output - origOutput);
}

#endif