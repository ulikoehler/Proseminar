var bwt = require("./bwt.js");
var fs = require("fs");
var reader = require("buffered-reader");

var infileName = process.argv[2];
var outfileName = process.argv[3];
var blockSizeStr = process.argv[4];
var linesplit  = (blockSizeStr == "lines" || blockSizeStr == undefined);
var blockSize = parseInt(blockSizeStr);

if(linesplit) {
console.dir("Using line-blocks");	
lineBWTOnFile(infileName, outfileName);
}else {
console.dir("Using blocksize " + blockSize);
bwtOnFile(blockSize, infileName, outfileName);
}


function getFilesizeInBytes(filename) {
	 var stats = fs.statSync(filename)
	 var fileSizeInBytes = stats["size"]
	 return fileSizeInBytes
}

function lineBWTOnFile(infile, outfile) {
	var alreadyProcessed = 0;
	var filesize = getFilesizeInBytes(infile);
	var lastPrinted = 0; //The last printed progress percentage
	var curBlockNum = 0;
	var blockIValues = []; //A list of all block i values (from BWT) - shall improve compression
	//Open the file to write to
	fs.open(outfile, 'w', 0666, function(err, targetFD) {
		//Open a fd
		new reader.DataReader(infile, {encoding : "utf8"})
			.on ("error", function (error){
			    console.log (error);
			})
			.on ("line", function (line, nextByteOffset){
				//Calculate how much we processed so far
				var processedPercentage = 100*alreadyProcessed / filesize;
				if(processedPercentage >= lastPrinted+1) {
					lastPrinted += 1;
					console.dir("Processed " + lastPrinted + " percent of " + infile);
				}
				//Calculate the BWT
				var bwtTuple = bwt.bwt(line);
				var L = bwtTuple[0];
				var i  = bwtTuple[1];
				blockIValues.push(i);
				var bwtBuf = new Buffer(L + "\n");
				var written = fs.writeSync(targetFD, bwtBuf, 0, bwtBuf.length, null);
				if(written != bwtBuf.length) {
					console.dir("Written only " + written + " of  " + bwtBuf.length + " bytes");
				}
				alreadyProcessed += line.length;
			})
			.on ("end", function () {
				fs.closeSync(targetFD);
				console.dir("Finished");
			}).read();
	});
}
 
function blockBWTOnFile(blocksize, infile, outfile) {
	var alreadyProcessed = 0;
	var filesize = getFilesizeInBytes(infile);
	var lastPrinted = 0; //The last printed progress percentage
	var curBlockNum = 0;
	var blockIValues = []; //A list of all block i values (from BWT) - shall improve compression
	//Open the file to write to
	fs.open(outfile, 'w', 0666, function(err, targetFD) {
		//Open a fd
		fs.open(infile, 'r', 0666, function(err, fd) {
				if(err) {console.dir(err);}
				//Read blocksize bytes
				while(1) {
					//Read the next blocksize bytes
					var buf = new Buffer(blocksize);
					var bytesRead = fs.readSync(fd, buf, 0, blocksize, null); //Read blocksize bytes
					//Calculate how much we processed so far
					var processedPercentage = 100*alreadyProcessed / filesize;
					if(processedPercentage >= lastPrinted+1) {
						lastPrinted += 1;
						console.dir("Processed " + lastPrinted + " percent of " + infile);
					}
					//Calculate the BWT
					var bwtTuple = bwt.bwt(buf.toString());
					var L = bwtTuple[0];
					var i  = bwtTuple[1];
					blockIValues.push(i);
					var bwtBuf = new Buffer(L);
					var written = fs.writeSync(targetFD, bwtBuf, 0, bwtBuf.length, null);
					if(written != bwtBuf.length) {
						console.dir("Written only " + written + " of  " + bwtBuf.length + " bytes");
					}
					//Break if the number of bytes read does not equal the expected bytecount (EOF)
					if(bytesRead != blocksize) {//Read blocksize bytes
						console.dir("Stopping...");
						break;
					}
					alreadyProcessed += bytesRead;
				}
				fs.closeSync(fd);
				fs.closeSync(targetFD);
				console.dir("Finished");
		});
	});
}
